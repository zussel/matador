#include <iostream>
#include <unordered_map>

enum class constraints : unsigned char {
  NONE =        0,
  NOT_NULL =    1 << 0,
  INDEX =       1 << 1,
  UNIQUE =      1 << 2,
  PRIMARY_KEY = 1 << 3,
  FOREIGN_KEY = 1 << 4,
  DEFAULT =     1 << 5
};

std::unordered_map<constraints, std::string> constraints_to_name_map {
  { constraints::NONE, "none" },
  { constraints::NOT_NULL, "not null" },
  { constraints::INDEX, "index" },
  { constraints::UNIQUE, "unique" },
  { constraints::PRIMARY_KEY, "primary key" },
  { constraints::FOREIGN_KEY, "foreign key" },
  { constraints::DEFAULT, "default" }
};

inline constraints operator|(constraints a, constraints b)
{
  return static_cast<constraints>(static_cast<unsigned int>(a) | static_cast<unsigned int>(b));
}

inline constraints operator&(constraints a, constraints b)
{
  return static_cast<constraints>(static_cast<unsigned int>(a) & static_cast<unsigned int>(b));
}

inline bool is_constraint_set(constraints source, constraints needle)
{
  return static_cast<int>(source & needle) > 0;
}
struct field_attributes
{
  field_attributes() = default;
  field_attributes(size_t size) : size_(size) {}
  field_attributes(constraints options) : options_(options) {}
  field_attributes(size_t size, constraints options) : size_(size), options_(options) {}
  ~field_attributes() = default;

  friend std::ostream& operator<<(std::ostream &out, const field_attributes &attr) {
    if (attr.size_ > 0) {
      out << "size: " << attr.size_ << ", ";
    }
    out << "attributes:";
    for (const auto& c : constraints_to_name_map) {
      if (is_constraint_set(attr.options_, c.first)) {
        out << " " << c.second;
      }
    }
    return out;
  }

  size_t size_ = 0;
  constraints options_ = constraints::NONE;
};

struct address {
  unsigned long id {};
};

struct person
{
  unsigned long id;
  std::string name;
  unsigned short height;
  unsigned short age;
  address home;

  template<class Operator>
  void process(Operator &op) {
    op.on_primary_key("id", id);
    op.on_attribute("name", name, { 255, constraints::UNIQUE | constraints::NOT_NULL });
    op.on_attribute("height", height, constraints::NOT_NULL );
    op.on_attribute("age", age);
    op.on_has_one("home", home);
  }
};

struct attribute_printer
{
  template<class PkType>
  void on_primary_key(const char *id, PkType &pk, field_attributes attr = constraints::PRIMARY_KEY ) {
    std::cout << "field " << id << " pk " << pk << " (" << attr << ")\n";
  }

  template<class AttributeType>
  void on_attribute(const char *id, AttributeType &field, field_attributes attr = {}) {
    std::cout << "field " << id << " attribute " << field << " (" << attr << ")\n";
  }

  template<class ForeignType>
  void on_has_one(const char *id, ForeignType &foreign, field_attributes attr = constraints::FOREIGN_KEY) {
    std::cout << "field " << id << " fk " << foreign.id << " (" << attr << ")\n";
  }
};

int main()
{
  person george { 1, "george", 187, 80 };

  attribute_printer ap;

  george.process(ap);
}