//
// Created by sascha on 08.09.19.
//

#ifndef MATADOR_SQLITE_PARAMETER_BINDER_HPP
#define MATADOR_SQLITE_PARAMETER_BINDER_HPP

#include "matador/sql/parameter_binder.hpp"

#include <sqlite3.h>

#include <vector>

namespace matador {

namespace sqlite {

class sqlite_parameter_binder : public detail::parameter_binder_impl
{
public:
  sqlite_parameter_binder(sqlite3 *db, sqlite3_stmt *stmt);

  void reset() override;

  void initialize_index(size_t index) override;
  size_t next_index() override;
  size_t current_index() const override;

  void bind(char i, size_t size) override;
  void bind(short i, size_t size) override;
  void bind(int i, size_t size) override;
  void bind(long i, size_t size) override;
  void bind(unsigned char i, size_t size) override;
  void bind(unsigned short i, size_t size) override;
  void bind(unsigned int i, size_t size) override;
  void bind(unsigned long i, size_t size) override;
  void bind(bool b, size_t size) override;
  void bind(float d, size_t size) override;
  void bind(double d, size_t size) override;
  void bind(const char *string, size_t size, size_t size1) override;
  void bind(const std::string &string, size_t size) override;
  void bind(const std::string &x, size_t s, size_t size) override;
  void bind(const matador::time &time, size_t size) override;
  void bind(const matador::date &date, size_t size) override;

private:
  sqlite3 *db_ = nullptr;
  sqlite3_stmt *stmt_ = nullptr;

  std::vector<std::shared_ptr<std::string> > host_strings_;

  size_t index_ = 1;
};

}
}
#endif //MATADOR_SQLITE_PARAMETER_BINDER_HPP
