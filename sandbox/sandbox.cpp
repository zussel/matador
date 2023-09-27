#include "matador/json/json.hpp"

#include "matador/object/typed_object_store_observer.hpp"
#include "matador/object/object_store.hpp"
#include "matador/object/object_ptr.hpp"
#include "matador/object/json_object_mapper.hpp"

// WebSocket:
// https://blog.mi.hdm-stuttgart.de/index.php/2021/02/19/websocket-protokoll-ein-detaillierter-technischer-einblick/
// https://developer.mozilla.org/en-US/docs/Web/API/WebSockets_API/Writing_WebSocket_servers
// https://datatracker.ietf.org/doc/html/rfc6455#section-5.2

class generic_json_service
{
public:
  using t_json_convert_func = std::function<void(const matador::json &)>;

  generic_json_service() = default;

  void register_converter(const std::string &name, const t_json_convert_func &converter)
  {
      converter_map_.insert(std::make_pair(name, converter));
  }

  void insert(const std::string &type, const matador::json &js)
  {
    auto it = converter_map_.find(type);
    if (it == converter_map_.end()) {
      return;
    }
    it->second(js);
  }
private:
  std::unordered_map<std::string, t_json_convert_func> converter_map_;
};

template < class T >
class object_store_json_type_observer : public matador::typed_object_store_observer<T>
{
public:
  object_store_json_type_observer(matador::object_store &store, generic_json_service &gjs)
    : store_(store)
    , gjs_(gjs)
  {}

  void on_attach(matador::prototype_node &node, T &) override
  {
      auto func = [this](const matador::json &js) {
          matador::json_object_mapper mapper;
          auto o = mapper.to_object<T>(js);
          auto p = store_.insert(o.release());
      };

      gjs_.register_converter(node.type(), func);
  }

  void on_detach(matador::prototype_node &, T &) override {}

  void on_insert(matador::object_proxy &) override {}
  void on_update(matador::object_proxy &) override {}
  void on_delete(matador::object_proxy &) override {}

private:
  matador::object_store &store_;
  generic_json_service &gjs_;
};

enum class colors : uint32_t
{
  RED, GREEN, BLUE, YELLOW, BLACK, WHITE, PURPLE, BROWN, GREY
};

std::map<colors, std::string> color_map = {
  { colors::RED, "red"},
  { colors::GREEN, "green"},
  { colors::BLUE, "blue"},
  { colors::YELLOW, "yellow"},
  { colors::BLACK, "black"},
  { colors::WHITE, "white"},
  { colors::PURPLE, "purple"},
  { colors::BROWN, "brown"},
  { colors::GREY, "grey"},
};

class person
{
public:
  unsigned long id_{};
  std::string name_;
  colors color_ { colors::WHITE };

  person() = default;
  person(unsigned long id, std::string name, colors c)
    : id_(id)
    , name_(std::move(name))
    , color_(c)
  {}

  friend std::ostream& operator<<(std::ostream &out, const matador::object_ptr<person> &p) {
    out << p->name_ << " (id: " << p->id_ << ", color: " << color_map[p->color_] << ")";
    return out;
  }

  template < class Operator >
  void process(Operator &op)
  {
    matador::access::primary_key(op, "id", id_);
    matador::access::attribute(op, "name", name_, 255);
    matador::access::attribute(op, "color", color_);
  }
};

namespace matador {

class date;
class time;

class field_serializer
{
public:
  virtual ~field_serializer() = default;
  virtual void serialize(char &x) = 0;
  virtual void serialize(short &x) = 0;
  virtual void serialize(int &x) = 0;
  virtual void serialize(long &x) = 0;
  virtual void serialize(long long &x) = 0;
  virtual void serialize(unsigned char &x) = 0;
  virtual void serialize(unsigned short &x) = 0;
  virtual void serialize(unsigned int &x) = 0;
  virtual void serialize(unsigned long &x) = 0;
  virtual void serialize(unsigned long long &x) = 0;
  virtual void serialize(std::string &x, long size) = 0;
  virtual void serialize(date &x) = 0;
  virtual void serialize(time &x) = 0;
};

class field
{
private:
  class base
  {
  public:
    virtual ~base() = default;
    virtual base *copy() const = 0;
    virtual std::string str() const = 0;
    virtual long size() const = 0;
    virtual void serialize(field_serializer &s) = 0;
  };

  template<typename Type>
  class value : public base
  {
  public:
    using self = value<Type>;

    explicit value(const Type &val, long size = -1)
    : value_(val), size_(size) {}

    base *copy() const final {
      return new self(value_, size_);
    }
    std::string str() const final {
      return std::to_string(value_);
    }
    long size() const final {
      return size_;
    }

    void serialize(field_serializer &s) final {
      s.serialize(value_);
    }

  private:
    Type value_;
    long size_{};
  };

public:
  field();
  template<typename Type>
  explicit field(std::string name, const Type &val)
  : name_(std::move(name)), value_(std::make_unique<value>(val)) {}
  field(const field &x)
  : name_(x.name_)
  , value_(x.value_->copy()) {}
  field &operator=(const field &x) {
    name_ = x.name_;
    value_.reset(x.value_->copy());
    return *this;
  }
  field(field &&x) noexcept
  : name_(std::move(x.name_))
  , value_(std::move(x.value_)) {}
  field &operator=(field &&x) noexcept {
    name_ = std::move(x.name_);
    value_ = std::move(x.value_);
    return *this;
  }

  void serialize(field_serializer &serializer) {
    value_->serialize(serializer);
  }

  const std::string& name() const {
    return name_;
  }
  std::string value_string() const {
    return value_->str();
  }

  long size() const {
    return value_->size();
  }

private:
  std::string name_;
  std::unique_ptr<base> value_;
};

class row
{
public:
  void add_field(const field &f) {
    field_.push_back(f);
  }
  void add_field(field &&f) {
    field_.emplace_back(f);
  }
private:
  std::vector<field> field_;
};
}

int main()
{
  generic_json_service gjs;

  matador::object_store store;

  store.attach<person>("person", { new object_store_json_type_observer<person>(store, gjs) });

  matador::json js = {
    { "name", "george" },
    { "id", 13 },
    { "color", (int)colors::GREEN }
  };
  gjs.insert("person", js);

  matador::object_view<person> persons(store);

  auto george = persons.front();

  std::cout << "object <person> " << george << "\n";

  matador::identifier pk(13UL);

  george = store.get<person>(pk);

  std::cout << "object <person> " << george << "\n";

  return 0;
}