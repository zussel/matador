#ifndef QUERY_POSTGRES_PARAMETER_BINDER_H
#define QUERY_POSTGRES_PARAMETER_BINDER_H

#include "matador/object/attribute_binder.hpp"

#ifdef _MSC_VER
#include <mysql.h>
#else
#include <mysql/mysql.h>
#endif

#include <vector>

namespace matador::backends::mysql {

struct mysql_result_info
{
  unsigned long length = 0;
  my_bool is_null = false;
  my_bool error = false;
//  std::unique_ptr<char[]> buffer;
  char *buffer = nullptr;
  unsigned long buffer_length = 0;
  bool is_allocated = false;

  ~mysql_result_info()
  {
    if (is_allocated) {
      delete [] buffer;
    }
  }
};

class mysql_parameter_binder final : public object::attribute_binder
{
public:
  explicit mysql_parameter_binder(size_t size);

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

  [[nodiscard]] std::vector<MYSQL_BIND>& bind_params();

private:
  struct is_null_t
  {
    my_bool is_null = false;
  };

  std::vector<MYSQL_BIND> bind_params_;
  std::vector<is_null_t> is_null_vector;
  std::vector<mysql_result_info> info_;
};

}

#endif //QUERY_POSTGRES_PARAMETER_BINDER_H
