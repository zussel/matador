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

  void bind(size_t pos, const char &x) override;
  void bind(size_t pos, const short &x) override;
  void bind(size_t pos, const int &x) override;
  void bind(size_t pos, const long &x) override;
  void bind(size_t pos, const long long int &x) override;
  void bind(size_t pos, const unsigned char &x) override;
  void bind(size_t pos, const unsigned short &x) override;
  void bind(size_t pos, const unsigned int &x) override;
  void bind(size_t pos, const unsigned long &x) override;
  void bind(size_t pos, const unsigned long long int &x) override;
  void bind(size_t pos, const bool &x) override;
  void bind(size_t pos, const float &x) override;
  void bind(size_t pos, const double &x) override;
  void bind(size_t pos, const time &x ) override;
  void bind(size_t pos, const date &x ) override;
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
