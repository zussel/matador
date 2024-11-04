#ifndef MATADOR_MYSQL_RESULT_BINDER_HPP
#define MATADOR_MYSQL_RESULT_BINDER_HPP

#include "matador/utils/attribute_reader.hpp"

#include "mysql_result_info.hpp"

#ifdef _MSC_VER
#include <mysql.h>
#else
#include <mysql/mysql.h>
#endif

#include <vector>

namespace matador::backends::mysql {

class mysql_result_binder final : public utils::attribute_reader
{
public:
  explicit mysql_result_binder(size_t size);

  void read_value(const char *id, size_t pos, char &x) override;
  void read_value(const char *id, size_t pos, short &x) override;
  void read_value(const char *id, size_t pos, int &x) override;
  void read_value(const char *id, size_t pos, long &x) override;
  void read_value(const char *id, size_t pos, long long int &x) override;
  void read_value(const char *id, size_t pos, unsigned char &x) override;
  void read_value(const char *id, size_t pos, unsigned short &x) override;
  void read_value(const char *id, size_t pos, unsigned int &x) override;
  void read_value(const char *id, size_t pos, unsigned long &x) override;
  void read_value(const char *id, size_t pos, unsigned long long int &x) override;
  void read_value(const char *id, size_t pos, bool &x) override;
  void read_value(const char *id, size_t pos, float &x) override;
  void read_value(const char *id, size_t pos, double &x) override;
  void read_value(const char *id, size_t pos, time &x ) override;
  void read_value(const char *id, size_t pos, date &x ) override;
  void read_value(const char *id, size_t pos, char *x, size_t size) override;
  void read_value(const char *id, size_t pos, std::string &x) override;
  void read_value(const char *id, size_t pos, std::string &x, size_t size) override;
  void read_value(const char *id, size_t pos, utils::blob &x) override;
  void read_value(const char *id, size_t pos, utils::value &x, size_t) override;

  [[nodiscard]] std::vector<MYSQL_BIND>& result_bindings();
  [[nodiscard]] std::vector<mysql_result_info>& result_infos();

private:
  struct is_null_t
  {
    my_bool is_null = false;
  };

  std::vector<MYSQL_BIND> result_bindings_;
  std::vector<is_null_t> is_null_vector;
  std::vector<mysql_result_info> result_infos_;
};

}

#endif //MATADOR_MYSQL_RESULT_BINDER_HPP
