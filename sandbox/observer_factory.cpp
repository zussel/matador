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

struct basic_node_handler
{
  virtual ~basic_node_handler() = default;
};

template < typename Type >
struct node_handler : basic_node_handler
{
  using prototype_ptr = std::unique_ptr<Type>;
  using observer_vector = std::vector<std::unique_ptr<observer<Type>>>;

  explicit node_handler(prototype_ptr &&proto)
  : prototype(std::move(proto)) {}

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

template <typename Type, template <typename> class... ObserverType>
class observer_creator
{
public:
  using observer_vector = std::vector<std::unique_ptr<observer<Type>>>;
  using iterator = typename observer_vector::iterator;

  observer_creator() {
    build_observer<ObserverType...>();
  }

  iterator begin();
  iterator end() { return observers_.end(); }

private:
  void build_observer() {}

  template <template <typename> class FirstObserverType>
  void build_observer() {
    observers_.emplace_back(std::make_unique<FirstObserverType<Type>>());
  }

  template <template <typename> class FirstObserverType, template <typename> class NextObserverType, template <typename> class... RestObserverType>
  void build_observer() {
    observers_.emplace_back(std::make_unique<FirstObserverType<Type>>());
    build_observer<NextObserverType, RestObserverType...>();
  }

private:
  observer_vector observers_;
};

template<typename Type, template <typename> class... ObserverType>
typename observer_creator<Type, ObserverType...>::iterator observer_creator<Type, ObserverType...>::begin() { return observers_.begin(); }

class store
{
public:
  template<typename Type, template <typename> typename... ObserverType>
  void attach(const std::string &name) {
    attach<Type, ObserverType...>(name, nullptr);
  }

  template<typename Type, typename Super, template <typename> typename... ObserverType>
  void attach(const std::string &name) {
    attach<Type, ObserverType...>(name, typeid(Super).name());
  }

  template<typename Type, template <typename> typename... ObserverType>
  void attach(const std::string &name, const char* super) {
    observer_creator<Type, ObserverType...> creator;
    nodes_.emplace_back(name, std::make_unique<Type>());
    for (auto &&o : creator) {
      std::cout << "handle observer " << typeid(o).name() << "\n";
    }
  }


private:
  std::vector<node> nodes_;
};

struct person {};
struct comment {};
struct student : person {};


int main() {

  store s;
  s.attach<person, logger_observer, logger_observer>("person");
//  s.attach<student, person, logger_observer, logger_observer>("student");
//  s.attach<comment, logger_observer, logger_observer>("comment");

  return 0;
}