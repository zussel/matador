//
// Created by sascha on 3/8/16.
//

#ifndef OOS_MYSQL_DIALECT_HPP
#define OOS_MYSQL_DIALECT_HPP

#include <sql/column.hpp>
#include "sql/basic_dialect.hpp"

namespace oos {
namespace mysql {

class mysql_dialect : public basic_dialect
{
public:
  mysql_dialect();
  const char* type_string(oos::data_type_t type) const;
  data_type_t string_type(const char *type) const;

  virtual void visit(const oos::detail::basic_value &value) override;
  virtual void visit(const oos::column &col) override;

  virtual void reset() override;
  virtual int bind_count() const override { return bind_count_; }
  virtual int column_count() const override { return column_count_; }

private:
  int bind_count_ = 0;
  int column_count_ = 0;
};

}
}

#endif //OOS_MYSQL_DIALECT_HPP
