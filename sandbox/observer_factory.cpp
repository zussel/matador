#include <iostream>
#include <memory>
#include <typeindex>
#include <utility>
#include <vector>
#include <unordered_map>

class basic_observer
{
protected:
  explicit basic_observer(const std::type_index &index)
  : type_index_(index)
  {}
public:
  virtual ~basic_observer() = default;

  [[nodiscard]] const std::type_index& index() const { return type_index_; }

private:
  std::type_index type_index_;

};
template < typename Type >
class observer : public basic_observer
{
public:
  observer() : basic_observer(std::type_index(typeid(Type))) {}

  virtual void on_attach(const Type& obj) = 0;
  virtual void on_detach(const Type& obj) = 0;
};


class basic_observer_producer
{
public:
  virtual ~basic_observer_producer() = default;
};


template < typename Type >
class observer_producer : public basic_observer_producer
{
public:
  [[nodiscard]] virtual std::unique_ptr<observer<Type>> produce_observer() const = 0;
};

template < typename Type >
class logger_observer : public observer<Type>
{
public:
  void on_attach(const Type& obj) override {
    std::cout << "attaching type " << typeid(Type).name() << "\n";
  };
  void on_detach(const Type& obj)  override {
    std::cout << "detaching type " << typeid(Type).name() << "\n";
  };

};

template < typename Type >
class logger_observer_producer : public observer_producer<Type>
{
public:
  [[nodiscard]] std::unique_ptr<observer<Type>> produce_observer() const override {
    return std::make_unique<logger_observer<Type>>();
  };
};

class basic_observer_factory
{
public:
  virtual ~basic_observer_factory() = default;

  template<typename Type>
  [[nodiscard]] std::shared_ptr<observer_producer<Type>> produce() const {
    std::type_index type(typeid(Type));
    return std::static_pointer_cast<observer_producer<Type>>(producer_.at(type));
  }

protected:
  std::unordered_map<std::type_index, std::shared_ptr<basic_observer_producer>> producer_;
};

class oof : public basic_observer_factory
{
public:

};

template < template < class T > typename ObserverProducerType >
class observer_factory : public basic_observer_factory
{
public:
  template<typename Type>
  void register_type() {
    auto item = std::make_shared<ObserverProducerType<Type>>();
    producer_.insert(std::make_pair(std::type_index(typeid(Type)), item));
  }
};

struct basic_node_handler
{
  virtual ~basic_node_handler() = default;
  virtual void register_producer(basic_observer_factory &factory) = 0;
  virtual void register_observer(const std::unique_ptr<basic_observer_producer> &observer_producer) = 0;
};

template < typename Type >
struct node_handler : basic_node_handler
{
  using prototype_ptr = std::unique_ptr<Type>;
  using observer_vector = std::vector<std::unique_ptr<observer<Type>>>;

  explicit node_handler(prototype_ptr &&proto)
  : prototype(std::move(proto)) {}

  void register_producer(basic_observer_factory &factory) override {
  }

  void register_observer(const std::unique_ptr<basic_observer_producer> &observer_producer) override {
    auto *producer = dynamic_cast<::observer_producer<Type>*>(observer_producer.get());
    if (producer) {
      auto obs = producer->produce_observer();
      observers.push_back(std::move(obs));
    }
  }

  prototype_ptr prototype;
  observer_vector observers;
};

struct node
{
  template<typename Type>
  node(std::string name, std::unique_ptr<Type> &&proto)
  : name(std::move(name))
  , handler(std::make_unique<node_handler<Type>>(std::move(proto)))
  {}

  const std::string name;
  std::unique_ptr<basic_node_handler> handler;
};

class store
{
public:
  template < template < class T > typename ObserverProducerType >
  void register_observer() {
    factory_.push_back(observer_factory<ObserverProducerType>{});
    for (auto &n : nodes_) {
      n.handler->register_producer(factory_.back());
    }
  }

//  template<typename Type>
//  void attach(const std::string &name) {
//    nodes_.emplace_back(name, std::make_unique<Type>());
//  }

  template<typename Type, template <typename> typename... ObserverType>
  void attach(const std::string &name) {
    nodes_.emplace_back(name, std::make_unique<Type>());
  }

  template<typename Type, typename Super, template <typename> typename... ObserverType>
  void attach(const std::string &name) {
    nodes_.emplace_back(name, std::make_unique<Type>());
  }

private:
  std::vector<basic_observer_factory> factory_;
  std::vector<node> nodes_;
};

struct person {};
struct comment {};
struct student : person {};


int main() {

  store s;
  s.attach<person, logger_observer, logger_observer>("person");
  s.attach<student, person, logger_observer, logger_observer>("student");
  s.attach<comment, logger_observer, logger_observer>("comment");

  s.register_observer<logger_observer_producer>();

  return 0;
}