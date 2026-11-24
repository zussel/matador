#ifndef MATADOR_RECORD_PRINTER_HPP
#define MATADOR_RECORD_PRINTER_HPP

#include "matador/query/model/record.hpp"

#include <iosfwd>
#include <map>

namespace matador::query {
class field;
class record;
}

namespace matador::test {
class RecordPrinter final {
public:
  explicit RecordPrinter(std::ostream &os);

  void print_header(const query::record &rec) const;
  void print(const query::record &rec) const;

  template < class Type, template <typename ...> class ContainerType >
  void print(ContainerType<Type> &records) const {
    bool first = true;
    for (const auto &rec: records) {
      if (first) {
        print_header(rec);
        first = false;
      }
      print(rec);
    }
  }

private:
  [[nodiscard]] static int width(const query::field &f) ;

private:
  std::ostream &os_;
  std::map<utils::basic_type, int> type_widths_;
};
}

#endif //MATADOR_RECORD_PRINTER_HPP
