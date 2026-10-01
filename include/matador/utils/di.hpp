#ifndef MATADOR_DI_HPP
#define MATADOR_DI_HPP

#include "matador/utils/singleton.hpp"

#include <functional>
#include <memory>
#include <mutex>
#include <stdexcept>
#include <string>
#include <thread>
#include <type_traits>
#include <typeindex>
#include <unordered_map>
#include <utility>

namespace matador::utils::di {

/**
 * Interface for dependency injection creation strategies.
 *
 * @tparam I Interface/base type returned by the strategy.
 */
template<class I>
class strategy
{
public:
  virtual ~strategy() = default;

  /**
   * @brief Acquires an instance.
   *
   * @return Shared pointer to the resolved object.
   */
  virtual std::shared_ptr<I> acquire() = 0;
};

/**
 * @brief Transient dependency injection strategy.
 *
 * Creates a new instance on every acquire().
 *
 * @tparam I Interface/base type
 * @tparam T Concrete implementation type
 */
template<class I, class T>
class transient_strategy final : public strategy<I>
{
  static_assert(std::is_convertible_v<T *, I *>, "implementation type must be convertible to interface type");

public:
  explicit transient_strategy(std::function<std::shared_ptr<T>()> factory)
  : factory_(std::move(factory))
  {}

  std::shared_ptr<I> acquire() override {
    return factory_();
  }

private:
  std::function<std::shared_ptr<T>()> factory_;
};

/**
 * @brief Singleton dependency injection strategy.
 *
 * Creates one instance lazily and returns it for every acquire().
 *
 * @tparam I Interface/base type
 * @tparam T Concrete implementation type
 */
template<class I, class T>
class singleton_strategy final : public strategy<I>
{
  static_assert(std::is_convertible_v<T *, I *>, "implementation type must be convertible to interface type");

public:
  explicit singleton_strategy(std::function<std::shared_ptr<T>()> factory)
  : factory_(std::move(factory))
  {}

  std::shared_ptr<I> acquire() override {
    std::call_once(once_, [this]() {
      instance_ = factory_();
    });

    return instance_;
  }

private:
  std::function<std::shared_ptr<T>()> factory_;
  std::once_flag once_;
  std::shared_ptr<T> instance_;
};

/**
 * @brief Singleton-per-thread dependency injection strategy.
 *
 * Creates one instance per thread id and keeps instances alive as long as the
 * strategy exists. Existing inject<T> objects can still keep instances alive
 * after the strategy was destroyed because acquire() returns shared_ptr.
 *
 * @tparam I Interface/base type
 * @tparam T Concrete implementation type
 */
template<class I, class T>
class singleton_per_thread_strategy final : public strategy<I>
{
  static_assert(std::is_convertible_v<T *, I *>, "implementation type must be convertible to interface type");

public:
  explicit singleton_per_thread_strategy(std::function<std::shared_ptr<T>()> factory)
  : factory_(std::move(factory))
  {}

  std::shared_ptr<I> acquire() override {
    const auto thread_id = std::this_thread::get_id();

    std::lock_guard<std::mutex> lock(instance_mutex_);

    auto it = instance_map_.find(thread_id);
    if (it == instance_map_.end()) {
      it = instance_map_.emplace(thread_id, factory_()).first;
    }

    return it->second;
  }

private:
  std::function<std::shared_ptr<T>()> factory_;
  std::mutex instance_mutex_;
  std::unordered_map<std::thread::id, std::shared_ptr<T>> instance_map_;
};

/**
 * @brief Provides an existing instance on injection.
 *
 * @tparam I Interface/base type
 * @tparam T Concrete implementation type
 */
template<class I, class T>
class instance_strategy final : public strategy<I>
{
  static_assert(std::is_convertible_v<T *, I *>, "implementation type must be convertible to interface type");

public:
  explicit instance_strategy(T &obj)
  : instance_(&obj, [](T *) {})
  {}

  explicit instance_strategy(std::shared_ptr<T> obj)
  : instance_(std::move(obj))
  {}

  std::shared_ptr<I> acquire() override {
    return instance_;
  }

private:
  std::shared_ptr<T> instance_;
};

class proxy_base
{
public:
  virtual ~proxy_base() = default;
};

template<typename I>
class proxy final : public proxy_base
{
public:
  template<typename T, typename... Args, std::enable_if_t<std::is_convertible_v<T *, I *>> * = nullptr>
  void to(Args &&...args) {
    initialize_strategy(std::make_unique<transient_strategy<I, T>>(
      make_factory<T>(std::forward<Args>(args)...)));
  }

  template<typename T, std::enable_if_t<std::is_convertible_v<T *, I *>> * = nullptr>
  void to_instance(T &obj) {
    initialize_strategy(std::make_unique<instance_strategy<I, T>>(obj));
  }

  template<typename T, std::enable_if_t<std::is_convertible_v<T *, I *>> * = nullptr>
  void to_instance(std::shared_ptr<T> obj) {
    initialize_strategy(std::make_unique<instance_strategy<I, T>>(std::move(obj)));
  }

  template<typename T, typename... Args, std::enable_if_t<std::is_convertible_v<T *, I *>> * = nullptr>
  void to_singleton(Args &&...args) {
    initialize_strategy(std::make_unique<singleton_strategy<I, T>>(
      make_factory<T>(std::forward<Args>(args)...)));
  }

  template<typename T, typename... Args, std::enable_if_t<std::is_convertible_v<T *, I *>> * = nullptr>
  void to_singleton_per_thread(Args &&...args) {
    initialize_strategy(std::make_unique<singleton_per_thread_strategy<I, T>>(
      make_factory<T>(std::forward<Args>(args)...)));
  }

  std::shared_ptr<I> get() const {
    if (!strategy_) {
      throw std::logic_error("type is bound but no dependency injection strategy was configured");
    }

    return strategy_->acquire();
  }

private:
  template<typename T, typename... Args>
  static std::function<std::shared_ptr<T>()> make_factory(Args &&...args) {
    return [args_tuple = std::make_tuple(std::forward<Args>(args)...)]() mutable {
      return std::apply(
        [](auto &...values) {
          return std::make_shared<T>(values...);
        },
        args_tuple);
    };
  }

  void initialize_strategy(std::unique_ptr<strategy<I>> &&strategy) {
    strategy_ = std::move(strategy);
  }

private:
  std::unique_ptr<strategy<I>> strategy_;
};

class module
{
private:
  using t_type_proxy_map = std::unordered_map<std::type_index, std::shared_ptr<proxy_base>>;

public:
  void clear()
  {
    default_map_.clear();
    module_map_.clear();
  }

  template<typename I>
  std::shared_ptr<proxy<I>> bind() {
    return bind<I>(default_map_);
  }

  template<typename I>
  std::shared_ptr<proxy<I>> bind(const std::string &name) {
    auto i = module_map_.find(name);
    if (i == module_map_.end()) {
      i = module_map_.insert(std::make_pair(name, t_type_proxy_map{})).first;
    }
    return bind<I>(i->second);
  }

  template<typename I>
  std::shared_ptr<I> resolve() {
    return resolve<I>(default_map_);
  }

  template<typename I>
  std::shared_ptr<I> resolve(const std::string &name) {
    auto i = module_map_.find(name);
    if (i == module_map_.end()) {
      throw std::logic_error("unknown dependency injection module name: " + name);
    }
    return resolve<I>(i->second);
  }

private:
  template<typename I>
  std::shared_ptr<proxy<I>> bind(t_type_proxy_map &type_proxy_map) {
    const auto key = std::type_index(typeid(I));

    auto i = type_proxy_map.find(key);
    if (i == type_proxy_map.end()) {
      auto di_proxy_ptr = std::make_shared<proxy<I>>();
      i = type_proxy_map.emplace(key, di_proxy_ptr).first;
    }

    return std::static_pointer_cast<proxy<I>>(i->second);
  }

  template<typename I>
  static std::shared_ptr<I> resolve(t_type_proxy_map &type_proxy_map) {
    const auto i = type_proxy_map.find(std::type_index(typeid(I)));
    if (i == type_proxy_map.end()) {
      throw std::logic_error(std::string{"unknown dependency injection type: "} + typeid(I).name());
    }

    return std::static_pointer_cast<proxy<I>>(i->second)->get();
  }

private:
  t_type_proxy_map default_map_;
  std::unordered_map<std::string, t_type_proxy_map> module_map_{};
};

class repository : public utils::singleton<repository>
{
public:
  void clear()
  {
    module_.clear();
  }

  void install(const std::function<void(module &)> &builder)
  {
    module_.clear();
    builder(module_);
  }

  void append(const std::function<void(module &)> &builder)
  {
    builder(module_);
  }

  template<typename I>
  std::shared_ptr<I> resolve() {
    return module_.resolve<I>();
  }

  template<typename I>
  std::shared_ptr<I> resolve(const std::string &name) {
    return module_.resolve<I>(name);
  }

private:
  module module_;
};

/**
 * @brief Resolves and provides the requested service.
 *
 * The class inject acts as a holder for the requested service.
 * It keeps the resolved object alive through std::shared_ptr.
 *
 * @tparam T Type of the interface
 */
template<class T>
class inject
{
public:
  inject()
  : obj_(repository::instance().resolve<T>())
  {}

  explicit inject(const std::string &name)
  : obj_(repository::instance().resolve<T>(name))
  {}

  explicit inject(module &m)
  : obj_(m.resolve<T>())
  {}

  inject(module &m, const std::string &name)
  : obj_(m.resolve<T>(name))
  {}

  bool operator==(const inject &x) const {
    return obj_ == x.obj_;
  }

  bool operator!=(const inject &x) const {
    return obj_ != x.obj_;
  }

  T *operator->() const {
    return obj_.get();
  }

  std::shared_ptr<T> get() const {
    return obj_;
  }

  T &operator*() const {
    return *obj_;
  }

  explicit operator bool() const {
    return static_cast<bool>(obj_);
  }

private:
  std::shared_ptr<T> obj_;
};

inline void clear_module()
{
  repository::instance().clear();
}

inline void install_module(const std::function<void(module &)> &builder)
{
  repository::instance().install(builder);
}

inline void append_module(const std::function<void(module &)> &builder)
{
  repository::instance().append(builder);
}

}

#endif //MATADOR_DI_HPP