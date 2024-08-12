#ifndef QUERY_SQLITE_PARAMETER_BINDER_H
#define QUERY_SQLITE_PARAMETER_BINDER_H

#include "matador/object/attribute_binder.hpp"

#include <sqlite3.h>

#include <memory>
#include <vector>

namespace matador::backends::sqlite {

class sqlite_parameter_binder final : public object::attribute_binder
{
public:
  explicit sqlite_parameter_binder(sqlite3 *db, sqlite3_stmt *stmt);

  void bind(size_t pos, char x) override;
  void bind(size_t pos, short x) override;
  void bind(size_t pos, int x) override;
  void bind(size_t pos, long x) override;
  void bind(size_t pos, long long int x) override;
  void bind(size_t pos, unsigned char x) override;
  void bind(size_t pos, unsigned short x) override;
  void bind(size_t pos, unsigned int x) override;
  void bind(size_t pos, unsigned long x) override;
  void bind(size_t pos, unsigned long long int x) override;
  void bind(size_t pos, bool x) override;
  void bind(size_t pos, float x) override;
  void bind(size_t pos, double x) override;
  void bind(size_t pos, const time &x) override;
  void bind(size_t pos, const date &x) override;
  void bind(size_t pos, const char *x) override;
  void bind(size_t pos, const char *x, size_t size) override;
  void bind(size_t pos, const std::string &x) override;
  void bind(size_t pos, const std::string &x, size_t size) override;
  void bind(size_t pos, const utils::blob &x) override;
private:
  sqlite3 *db_{nullptr};
  sqlite3_stmt *stmt_{nullptr};

  std::vector<std::string> host_strings_;
};
}

#endif //QUERY_SQLITE_PARAMETER_BINDER_H
