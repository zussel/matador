//
// Created by sascha on 03.06.19.
//

#ifndef MATADOR_POSTGRESQL_STATEMENT_HPP
#define MATADOR_POSTGRESQL_STATEMENT_HPP

#include "matador/sql/statement_impl.hpp"

namespace matador {

class varchar_base;

class basic_identifier;

namespace postgresql {

class postgresql_connection;

class postgresql_statement : public matador::detail::statement_impl
{
public:
  postgresql_statement(postgresql_connection &db, const std::string &stmt);

  ~postgresql_statement() override;

  void clear() override;

  detail::result_impl *execute() override;

  void reset() override;

protected:
  void serialize(const char *id, char &x) override;

  void serialize(const char *id, short &x) override;

  void serialize(const char *id, int &x) override;

  void serialize(const char *id, long &x) override;

  void serialize(const char *id, unsigned char &x) override;

  void serialize(const char *id, unsigned short &x) override;

  void serialize(const char *id, unsigned int &x) override;

  void serialize(const char *id, unsigned long &x) override;

  void serialize(const char *id, float &x) override;

  void serialize(const char *id, double &x) override;

  void serialize(const char *id, bool &x) override;

  void serialize(const char *id, char *x, size_t s) override;

  void serialize(const char *id, varchar_base &x) override;

  void serialize(const char *id, std::string &x) override;

  void serialize(const char *id, matador::date &x) override;

  void serialize(const char *id, matador::time &x) override;

  void serialize(const char *id, basic_identifier &x) override;

  void serialize(const char *id, identifiable_holder &x, cascade_type) override;

private:
  postgresql_connection &db_;

  std::vector<std::shared_ptr<std::string> > host_strings_;
};

}
}

#endif //MATADOR_POSTGRESQL_STATEMENT_HPP
