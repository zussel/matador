#ifndef MATADOR_OBJECT_CACHE_HPP
#define MATADOR_OBJECT_CACHE_HPP

#include "matador/object/object_proxy.hpp"
#include "matador/object/object_resolver.hpp"

#include "matador/utils/identifier.hpp"
#include "matador/utils/message_bus.hpp"

#include <unordered_map>
#include <memory>
#include <shared_mutex>
#include <typeindex>
#include <utility>
#include <cassert>

namespace matador::object {
struct cache_entry_base {
  virtual ~cache_entry_base() = default;

  /**
   * @brief Gibt an, ob der Eintrag vollständig "tot" ist (weder Proxy noch Entity leben).
   *
   * Wird von @ref object_cache::sweep() verwendet, um Einträge ohne T-Kenntnis zu bereinigen.
   */
  [[nodiscard]] virtual bool is_dead() const noexcept = 0;
};

template<typename Type>
struct cache_entry : cache_entry_base {
  std::weak_ptr<object_proxy<Type> > proxy;
  std::weak_ptr<Type> entity;

  [[nodiscard]] bool is_dead() const noexcept override {
    return proxy.expired() && entity.expired();
  }
};

struct object_cache_event {
  std::type_index type{typeid(void)};
  utils::identifier id{};
  std::chrono::steady_clock::time_point timestamp{};
};

struct object_cache_accessed_event : object_cache_event {};
struct object_cache_proxy_added_event : object_cache_event {};
struct object_cache_entity_attached_event : object_cache_event {};
struct object_cache_imported_event : object_cache_event {};
struct object_cache_erased_event : object_cache_event {};

struct object_cache_sweep_event {
  std::size_t removed{};
  std::chrono::steady_clock::time_point timestamp{};
};

/**
 * @brief Thread-sicherer Cache für Objekt-Proxies und (optional) geladene Entities.
 *
 * Der Cache verwaltet Einträge pro Kombination aus Objekt-Typ @p T und Primärschlüssel/Identifier.
 * Für jede (T, id)-Kombination können sowohl
 * - ein Proxy (@c object_proxy<T>) als auch
 * - die dazugehörige Entity (@c T)
 * zwischengespeichert werden.
 *
 * Der Cache hält dabei nur @c std::weak_ptr auf Proxy und Entity, d.h. er verlängert deren Lebensdauer nicht.
 *
 * @note Thread-Safety: Alle öffentlichen Methoden sind threadsafe (intern synchronisiert).
 * @note Semantik: Ein Eintrag kann aus dem Cache opportunistisch entfernt werden, sobald Proxy und Entity abgelaufen sind.
 */
class object_cache {
public:
  /**
   * @brief Erzeugt einen leeren Cache.
   */
  explicit object_cache(utils::message_bus &bus)
  : bus_(bus) {}

  /**
   * @brief Liefert einen Proxy für (T, id) und erstellt ihn bei Bedarf.
   *
   * Falls bereits ein lebender Proxy im Cache existiert, wird er wiederverwendet.
   * Andernfalls wird ein neuer Proxy erzeugt, im Cache abgelegt und zurückgegeben.
   * Falls für (T, id) bereits eine Entity vorhanden ist, wird sie an den Proxy gebunden.
   *
   * @tparam Type Entity-Typ.
   * @tparam ResolverPointerType Zeiger-Typ auf einen Resolver, typischerweise
   *         @c std::shared_ptr<object_resolver<T>> oder @c std::weak_ptr<object_resolver<T>>.
   * @param id Identifier/Primärschlüssel der Entity.
   * @param resolver_ptr Resolver (shared/weak), der zur Lazy-Auflösung durch den Proxy genutzt wird.
   * @return Shared-Pointer auf den (ggf. neu erstellten) Proxy.
   *
   * @note Diese Methode ist threadsafe.
   */
  template<typename Type, typename ResolverPointerType>
  std::shared_ptr<object_proxy<Type>> acquire_proxy(utils::identifier id, ResolverPointerType &&resolver_ptr) {
    const auto k = make_key<Type>(id);

    std::unique_lock lock(mutex_);

    auto it = map_.find(k);
    if (it != map_.end()) {
      // found entry, return std::shared_ptr of proxy
      auto *entry = entry_cast_<Type>(it->second.get());
      if (auto proxy_ptr = entry->proxy.lock()) {
        bus_.publish<object_cache_accessed_event>({k.type, id, std::chrono::steady_clock::now()});
        return proxy_ptr;
      }

      // if the proxy is dead, but the entity is alive, create a new proxy and attach entity
      auto weak_resolver = to_weak<Type>(std::forward<ResolverPointerType>(resolver_ptr));
      auto proxy_ptr = std::make_shared<object_proxy<Type>>(weak_resolver, id);

      if (auto obj = entry->entity.lock()) {
        proxy_ptr->attach(std::move(obj));
      }

      entry->proxy = proxy_ptr;

      bus_.publish<object_cache_proxy_added_event>({k.type, id, std::chrono::steady_clock::now()});

      // return the shared_ptr of the proxy
      return proxy_ptr;
    }

    // entry couldn't be found, create a new entry
    auto entry_ptr = std::make_unique<cache_entry<Type>>();
    auto *entry = entry_ptr.get();

    // create a weak resolver and shared proxy
    auto weak_resolver = to_weak<Type>(std::forward<ResolverPointerType>(resolver_ptr));
    auto proxy_ptr = std::make_shared<object_proxy<Type>>(weak_resolver, id);

    // lock entity and attach to proxy
    if (auto obj = entry->entity.lock()) {
      proxy_ptr->attach(std::move(obj));
    }

    // set the new proxy into cache entry
    entry->proxy = proxy_ptr;
    map_.emplace(k, std::move(entry_ptr));

    bus_.publish<object_cache_proxy_added_event>({k.type, id, std::chrono::steady_clock::now()});

    // return the shared_ptr of the proxy
    return proxy_ptr;
  }

  template<typename Type, typename ResolverPointerType>
  bool import(const utils::identifier &id, const std::shared_ptr<object_proxy<Type>> &proxy, ResolverPointerType &&resolver_ptr) {
    if (!proxy) {
      return false;
    }

    auto obj = proxy->object();
    auto weak_resolver = to_weak<Type>(std::forward<ResolverPointerType>(resolver_ptr));
    const auto k = make_key<Type>(id);

    std::unique_lock lock(mutex_);

    auto it = map_.find(k);
    if (it != map_.end()) {
      auto *entry = entry_cast_<Type>(it->second.get());

      if (auto existing_proxy = entry->proxy.lock()) {
        if (existing_proxy != proxy) {
          return false;
        }
      }

      proxy->primary_key(id);
      proxy->resolver(std::move(weak_resolver));

      if (obj) {
        entry->entity = obj;
        proxy->attach(std::move(obj));
      }

      entry->proxy = proxy;
      bus_.publish<object_cache_imported_event>({k.type, id, std::chrono::steady_clock::now()});
      return true;
    }

    auto entry_ptr = std::make_unique<cache_entry<Type>>();
    auto *entry = entry_ptr.get();

    proxy->primary_key(id);
    proxy->resolver(std::move(weak_resolver));

    if (obj) {
      entry->entity = obj;
      proxy->attach(std::move(obj));
    }

    entry->proxy = proxy;
    map_.emplace(k, std::move(entry_ptr));

    bus_.publish<object_cache_imported_event>({k.type, id, std::chrono::steady_clock::now()});

    return true;
  }
  /**
   * @brief Verknüpft eine geladene Entity mit einem Cache-Eintrag (Type, id).
   *
   * Speichert die Entity als @c weak_ptr im Cache. Falls bereits ein Proxy existiert,
   * wird die Entity sofort an den Proxy gebunden.
   *
   * @tparam Type Entity-Typ.
   * @param id Identifier/Primärschlüssel der Entity.
   * @param obj Geladene Entity als @c shared_ptr.
   *
   * @note Diese Methode ist threadsafe.
   */
  template<typename Type>
  void attach_entity(const utils::identifier &id, std::shared_ptr<Type> obj) {
    const auto k = make_key<Type>(id);

    std::unique_lock lock(mutex_);

    auto it = map_.find(k);
    if (it == map_.end()) {
      auto entry_ptr = std::make_unique<cache_entry<Type>>();
      auto *entry = entry_ptr.get();
      entry->entity = obj;
      map_.emplace(k, std::move(entry_ptr));
      bus_.publish<object_cache_entity_attached_event>({k.type, id, std::chrono::steady_clock::now()});
      return;
    }

    auto *entry = entry_cast_<Type>(it->second.get());
    entry->entity = obj;

    if (auto proxy = entry->proxy.lock()) {
      proxy->attach(std::move(obj));
    }

    bus_.publish<object_cache_entity_attached_event>({k.type, id, std::chrono::steady_clock::now()});

    prune_if_dead(it);
  }

  /**
   * @brief Liefert die aktuell geladene Entity für (Type, id), falls vorhanden.
   *
   * @tparam Type Entity-Typ.
   * @param id Identifier/Primärschlüssel der Entity.
   * @return @c shared_ptr auf die Entity oder ein leerer Pointer, falls nicht geladen/abgelaufen.
   *
   * @note Diese Methode ist threadsafe.
   */
  template<typename Type>
  std::shared_ptr<Type> get_entity(const utils::identifier &id) {
    const auto k = make_key<Type>(id);

    std::unique_lock lock(mutex_);
    auto it = map_.find(k);
    if (it == map_.end()) {
      return {};
    }

    auto *entry = entry_cast_<Type>(it->second.get());
    auto entity_ptr = entry->entity.lock();
    if (entity_ptr) {
      bus_.publish<object_cache_accessed_event>({k.type, id, std::chrono::steady_clock::now()});
    }
    prune_if_dead(it);
    return entity_ptr;
  }

  /**
   * @brief Prüft, ob für (Type, id) aktuell eine lebende Entity verfügbar ist.
   *
   * @tparam Type Entity-Typ.
   * @param id Identifier/Primärschlüssel der Entity.
   * @return @c true, wenn die Entity-Referenz nicht abgelaufen ist; sonst @c false.
   *
   * @note Diese Methode ist threadsafe.
   */
  template<typename Type>
  bool is_loaded(const utils::identifier &id) {
    const auto k = make_key<Type>(id);

    std::unique_lock lock(mutex_);
    auto it = map_.find(k);
    if (it == map_.end()) {
      return false;
    }

    auto *entry = entry_cast_<Type>(it->second.get());
    const bool loaded = !entry->entity.expired();
    if (loaded) {
      bus_.publish<object_cache_accessed_event>({k.type, id, std::chrono::steady_clock::now()});
    }
    prune_if_dead(it);
    return loaded;
  }

  template<typename T>
  void erase(utils::identifier id) {
    const auto k = make_key<T>(id);

    std::unique_lock lock(mutex_);
    auto it = map_.find(k);
    if (it == map_.end()) {
      return;
    }

    auto *e = entry_cast_<T>(it->second.get());
    if (auto p = e->proxy.lock()) {
      p->invalidate();
    }

    map_.erase(it);
    bus_.publish<object_cache_erased_event>({k.type, id, std::chrono::steady_clock::now()});
  }

  /**
   * @brief Führt einen Cleanup-Lauf aus und entfernt Einträge, deren Proxy und Entity abgelaufen sind.
   *
   * @return Anzahl der entfernten Einträge.
   *
   * @note Diese Methode ist threadsafe.
   */
  [[nodiscard]] std::size_t sweep() {
    std::unique_lock lock(mutex_);
    std::size_t removed = 0;

    for (auto it = map_.begin(); it != map_.end(); ) {
      if (it->second->is_dead()) {
        it = map_.erase(it);
        ++removed;
      } else {
        ++it;
      }
    }

    if (removed > 0) {
      bus_.publish<object_cache_sweep_event>({removed, std::chrono::steady_clock::now()});
    }

    return removed;
  }

private:
  struct key {
    std::type_index type;
    utils::identifier id{};
    bool operator==(key const &other) const {
      return type == other.type && id == other.id;
    }
  };

  struct key_hash {
    size_t operator()(key const &k) const noexcept {
      // (3) robustere Hash-Kombination als XOR
      size_t seed = std::hash<std::type_index>()(k.type);
      const size_t h2 = std::hash<utils::identifier>()(k.id);
      seed ^= h2 + 0x9e3779b97f4a7c15ULL + (seed << 6) + (seed >> 2);
      return seed;
    }
  };

  template<typename T>
  static key make_key(const utils::identifier &id) {
    return key{std::type_index(typeid(T)), id};
  }

  // (4) Debug-Absicherung für type-erased Casts
  template<typename T>
  static cache_entry<T> *entry_cast_(cache_entry_base *base) {
#ifndef NDEBUG
    auto *p = dynamic_cast<cache_entry<T> *>(base);
    assert(p && "object_cache: Type mismatch in cache entry (key/type invariant violated)");
    return p;
#else
    return static_cast<cache_entry<T> *>(base);
#endif
  }

  template<typename T, typename U>
  using enable_if_resolver_shared_ptr_t = std::enable_if_t<std::is_base_of_v<object_resolver<T>, U>, int>;

  template<typename T, typename U>
  using enable_if_resolver_weak_ptr_t = std::enable_if_t<std::is_base_of_v<object_resolver<T>, U>, int>;

  // shared_ptr<Derived> -> weak_ptr<Base>
  template<typename T, typename U, enable_if_resolver_shared_ptr_t<T, U> = 0>
  static std::weak_ptr<object_resolver<T>> to_weak(const std::shared_ptr<U> &s) {
    return std::weak_ptr<object_resolver<T>>(std::static_pointer_cast<object_resolver<T>>(s));
  }

  // weak_ptr<Derived> -> weak_ptr<Base>
  template<typename T, typename U, enable_if_resolver_weak_ptr_t<T, U> = 0>
  static std::weak_ptr<object_resolver<T>> to_weak(const std::weak_ptr<U> &w) {
    return std::weak_ptr<object_resolver<T>>(w);
  }

  // (2) Opportunistischer Cleanup: entferne Entry, wenn beide weak_ptr abgelaufen sind.
  template<typename IteratorType>
  bool prune_if_dead(IteratorType &it) {
    if (it == map_.end()) {
      return false;
    }
    if (it->second && it->second->is_dead()) {
      map_.erase(it);
      it = map_.end();
      return true;
    }
    return false;
  }

  template<typename Type>
  void prune_if_dead_typed(typename std::unordered_map<key, std::unique_ptr<cache_entry_base>, key_hash>::iterator &it) {
      (void)Type{}; // T bleibt im Interface, damit bestehende Call-Sites unverändert bleiben können.
      prune_if_dead(it);
  }

private:
  mutable std::shared_mutex mutex_{};
  utils::message_bus &bus_;
  std::unordered_map<key, std::unique_ptr<cache_entry_base>, key_hash> map_;
};

// --- Inline Anpassungen: typed prune nach Zugriff (damit wir T haben) ---

// ... existing code ...
// (Die typed prune wird oben benutzt; wir rufen sie direkt im Codepfad anstelle von prune_if_dead_)

} // namespace matador::object
#endif //MATADOR_OBJECT_CACHE_HPP
