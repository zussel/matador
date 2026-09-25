#ifndef MATADOR_MESSAGE_BUS_HPP
#define MATADOR_MESSAGE_BUS_HPP

#include <atomic>
#include <functional>
#include <memory>
#include <mutex>
#include <shared_mutex>
#include <typeindex>
#include <typeinfo>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <stdexcept>

namespace matador::utils {
class message_bus;

/**
 * @brief Represents a generic, type-safe message object used for communication.
 */
class message {
public:
    /**
     * @brief Default constructor for an empty message.
     */
    message();

    /**
     * @brief Constructs an owning message from a value.
     *
     * @tparam MessageType The type of the message.
     * @param msg The message to be stored. A copy is made.
     */
    template<typename MessageType>
    explicit message(const MessageType& msg) {
        auto strong = std::make_shared<MessageType>(msg);               // allocate
        owner_ = std::static_pointer_cast<void>(strong);       // share ownership as void*
        ptr_ = static_cast<const void*>(strong.get());
        type_ = std::type_index(typeid(MessageType));
    }

    /**
     * @brief Constructs a non-owning message from a reference.
     *
     * @tparam MessageType The type of the reference.
     * @param msg A reference to the message data. The caller must ensure its lifetime during use.
     * @return A non-owning message wrapping the reference.
     */
    template<typename MessageType>
    static message from_ref(const MessageType& msg) {
        message m;
        m.ptr_ = static_cast<const void*>(std::addressof(msg));
        m.type_ = std::type_index(typeid(MessageType));
        // owner_ remains null => non-owning
        return m;
    }

    /**
     * @brief Constructs an owning message from a shared pointer.
     *
     * @tparam MessageType The type of the object managed by the shared pointer.
     * @param msg A shared pointer to the message.
     * @return A message that shares ownership of the object.
     */
    template<typename MessageType>
    static message from_shared(std::shared_ptr<MessageType> msg) {
        message m;
        m.owner_ = std::static_pointer_cast<void>(msg);
        m.ptr_ = static_cast<const void*>(msg.get());
        m.type_ = std::type_index(typeid(MessageType));
        return m;
    }

    /**
     * @brief Checks if the message is empty.
     *
     * @return True if the message is empty, false otherwise.
     */
    [[nodiscard]] bool empty() const;

    /**
     * @brief Gets the type of the object stored in the message.
     *
     * @return The type of the stored object as a std::type_index.
     */
    [[nodiscard]] std::type_index type() const;

    /**
     * @brief Checks if the stored object is of the specified type.
     *
     * @tparam MessageType The type to check.
     * @return True if the object matches the type, false otherwise.
     */
    template<typename MessageType>
    [[nodiscard]] bool is() const {
      return type_ == std::type_index(typeid(MessageType));
    }

    /**
     * @brief Accesses the stored object as a typed reference.
     *
     * @tparam MessageType The expected type of the object.
     * @return A constant reference to the stored object.
     * @throws std::bad_cast if the type does not match.
     * @throws std::runtime_error if the message is empty.
     */
    template<typename MessageType>
    const MessageType& get() const {
        if (!is<MessageType>()) {
          throw std::bad_cast();
        }
        const void* ptr = raw_ptr();
        if (!ptr) {
          throw std::runtime_error("AnyMessage: empty pointer");
        }
        return *static_cast<const MessageType*>(ptr);
    }

    /**
     * @brief Retrieves the raw pointer to the stored object.
     *
     * @return A const void* pointing to the stored object (may be non-owning).
     */
    [[nodiscard]] const void* raw_ptr() const;

private:
    const void* ptr_;                 // non-owning raw pointer (if set)
    std::shared_ptr<void> owner_;     // owning pointer if we made a copy or got shared_ptr
    std::type_index type_{typeid(void)};
};

/**
 * @brief Represents a subscription to a message_bus, providing RAII-style unsubscription.
 */
class subscription {
public:
    /**
     * @brief Default constructor for an empty subscription.
     */
    subscription() = default;
    /**
     * @brief Constructs a subscription for a specific handler and message type.
     *
     * @param bus Pointer to the message bus managing the subscription.
     * @param type The type_index of the message type.
     * @param id The unique ID of the handler.
     */
    subscription(message_bus* bus, std::type_index type, uint64_t id);

    // Prohibit copying of subscription objects.
    subscription(const subscription&) = delete;
    subscription& operator=(const subscription&) = delete;
    // Allow move semantics for subscription objects.
    subscription(subscription&& other) noexcept;
    subscription& operator=(subscription&& other) noexcept;

    /**
     * @brief Destructor that ensures cleanup by unsubscribing from the bus.
     */
    ~subscription();

    /**
     * @brief Unsubscribes the handler from the message bus.
     */
    void unsubscribe();

    /**
     * @brief Checks if the subscription is still valid.
     *
     * @return True if the subscription is valid, false otherwise.
     */
    [[nodiscard]] bool valid() const;

private:
    message_bus* bus_ = nullptr;
    std::type_index type_{typeid(void)};
    uint64_t id_ = 0;
};

/**
 * @brief A thread-safe message bus for runtime message publishing and subscription handling.
 */
class message_bus {
public:
    using HandlerId = uint64_t; /**< Type alias for handler IDs. */

    /**
     * @brief Type alias for message filter functions.
     * @tparam MessageType The type of message the filter operates on.
     */
    template<typename MessageType>
    using Filter = std::function<bool(const MessageType&)>;

    /**
     * @brief Type alias for member function pointers used as message handlers.
     * @tparam MessageType The type of message the handler processes.
     * @tparam CallerClass The class that contains the member function.
     */
    template<typename MessageType, typename CallerClass>
    using MemberHandler = void (CallerClass::*)(const MessageType&);


    /**
     * @brief Constructs a new message_bus object.
     */
    message_bus();

    /**
     * @brief Subscribes to messages of a specific type using a free function or lambda.
     *
     * @tparam MessageType The type of the message to subscribe to.
     * @param handler The function to execute when a message of type T is published.
     * @param filter An optional filter function to determine if the handler should be invoked.
     * @return A subscription object to manage the handler's registration.
     */
    template<typename MessageType>
    subscription subscribe(std::function<void(const MessageType&)> handler, Filter<MessageType> filter = nullptr) {
        auto id = next_id_.fetch_add(1, std::memory_order_relaxed);
        std::unique_lock write_lock(mutex_);

        auto &vec = handlers_[std::type_index(typeid(MessageType))];
        Entry entry;
        entry.id = id;
        entry.handler = [h = std::move(handler)](const void* p) {
            h(*static_cast<const MessageType*>(p));
        };
        if (filter) {
            entry.filter = [f = std::move(filter)](const void* p) -> bool {
                return f(*static_cast<const MessageType*>(p));
            };
        } else {
            entry.filter = nullptr;
        }
        vec.emplace_back(std::move(entry));
        return {this, std::type_index(typeid(MessageType)), id};
    }

    /**
     * @brief Subscribes to messages using a class member function bound to an instance.
     *
     * @tparam MessageType The type of the message to subscribe to.
     * @tparam CallerClass The class of the instance.
     * @param instance A pointer to the instance.
     * @param memberFn A pointer to the member function to execute.
     * @param filter An optional filter function.
     * @return A subscription object to manage the handler's registration.
     */
    template<typename MessageType, typename CallerClass>
    subscription subscribe(CallerClass* instance,
                           MemberHandler<MessageType, CallerClass> memberFn,
                           Filter<MessageType> filter = nullptr) {
        auto fn = [instance, memberFn](const MessageType& m) { (instance->*memberFn)(m); };
        return subscribe<MessageType>(std::function<void(const MessageType&)>(fn), std::move(filter));
    }

    /**
     * @brief Subscribes to messages using a member function bound to a shared_ptr instance.
     *
     * @tparam MessageType The type of the message to subscribe to.
     * @tparam CallerClass The class of the shared_ptr instance.
     * @param instance A shared pointer to the instance.
     * @param member_func A pointer to the member function to execute.
     * @param filter An optional filter function.
     * @return A subscription object to manage the handler's registration.
     */
    template<typename MessageType, typename CallerClass>
    subscription subscribe(std::shared_ptr<CallerClass> instance,
                           MemberHandler<MessageType, CallerClass> member_func,
                           Filter<MessageType> filter = nullptr) {
        std::weak_ptr<CallerClass> caller_ptr = instance;
        auto handler = [caller_ptr, member_func](const MessageType& msg) {
            if (auto caller = caller_ptr.lock()) {
                (caller.get()->*member_func)(msg);
            }
        };

        std::function<bool(const MessageType&)> local_filter = nullptr;
        if (filter) {
            local_filter = [caller_ptr, filter = std::move(filter)](const MessageType& msg) -> bool {
                if (caller_ptr.expired()) {
                    return false;
                }
                return filter(msg);
            };
        }
        return subscribe<MessageType>(std::move(handler), std::move(local_filter));
    }

    /**
     * @brief Unsubscribes a handler from the bus using its type and ID.
     *
     * @tparam MessageType The type of the message.
     * @param id The unique handler ID to unsubscribe.
     */
    template<typename MessageType>
    void unsubscribe(const HandlerId id) {
        unsubscribe(std::type_index(typeid(MessageType)), id);
    }

    /**
     * @brief Unsubscribes a handler by its type index and ID.
     *
     * @param type The type_index of the message type.
     * @param id The unique handler ID to unsubscribe.
     */
    void unsubscribe(std::type_index type, HandlerId id);

    /**
     * @brief Unsubscribes a handler by its unique ID, regardless of type.
     *
     * @param id The unique handler ID to unsubscribe.
     */
    void unsubscribe(HandlerId id);

    /**
     * @brief Publishes a message of a specific type.
     *
     * @tparam MessageType The type of the message to publish.
     * @param msg The message to publish.
     */
    template<typename MessageType>
    void publish(const MessageType& msg) const {
        std::vector<Entry> snapshot;
        {
            std::shared_lock read_lock(mutex_);
            const auto it = handlers_.find(std::type_index(typeid(MessageType)));
            if (it == handlers_.end()) {
                return;
            }
            snapshot = it->second; // copy list to avoid holding lock during callbacks
        }
        for (const auto &e : snapshot) {
            if (!e.filter || e.filter(&msg)) {
              e.handler(&msg);
            }
        }
    }

    /**
     * @brief Publishes a generic message object.
     *
     * @param msg The message to publish.
     */
    void publish(const message& msg) const;

private:
    friend class subscription;

    struct Entry {
        HandlerId id;
        std::function<void(const void*)> handler;
        std::function<bool(const void*)> filter;
    };

    mutable std::shared_mutex mutex_;
    std::unordered_map<std::type_index, std::vector<Entry>> handlers_;
    std::atomic<HandlerId> next_id_;
};

inline void subscription::unsubscribe() {
    if (bus_) {
        bus_->unsubscribe(type_, id_);
        bus_ = nullptr;
    }
}
}

#endif //MATADOR_MESSAGE_BUS_HPP