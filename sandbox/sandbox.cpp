#include "matador/json/json.hpp"

#include "matador/object/typed_object_store_observer.hpp"
#include "matador/object/object_store.hpp"
#include "matador/object/object_ptr.hpp"
#include "matador/object/json_object_mapper.hpp"

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
  matador::identifier<unsigned long> id_;
  std::string name_;
  colors color_ { colors::WHITE };

  person() = default;
  person(unsigned long id, std::string name, colors c)
    : id_(id)
    , name_(std::move(name))
    , color_(c)
  {}

  template < matador::object_holder_type OHT >
  friend std::ostream& operator<<(std::ostream &out, const matador::object_pointer<person, OHT> &p) {
    out << p->name_ << " (id: " << p->id_ << ", color: " << color_map[p->color_] << ")";
    return out;
  }

  template < class T >
  void serialize(T &serializer)
  {
    serializer.serialize("id", id_);
    serializer.serialize("name", name_, 255);
    serializer.serialize("color", color_);
  }
};

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

  auto pk = matador::make_identifier(13UL);

  george = store.get<person>(pk);

  std::cout << "object <person> " << george << "\n";

  return 0;
}