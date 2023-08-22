#ifndef SQLITE_RESULT_HPP
#define SQLITE_RESULT_HPP

#include "matador/sql/result_impl.hpp"

#include <vector>

namespace matador {

class row;
class serializable;

namespace sqlite {

class sqlite_result : public detail::result_impl
{
public:
  sqlite_result(const sqlite_result&) = delete;
  sqlite_result& operator=(const sqlite_result&) = delete;

public:
  typedef detail::result_impl::size_type size_type;

public:
  sqlite_result() = default;
  ~sqlite_result() override;
  
  const char* column(size_type c) const override;
  bool fetch() override;
  size_type affected_rows() const override;
  size_type result_rows() const override;
  size_type fields() const override;

  size_type reset_column_index() const override;

  void push_back(char **row_values, int column_count);

protected:
  bool prepare_fetch() override;
  bool finalize_fetch() override;

protected:
  void read_value(const char *id, size_type index, char &value) override;
  void read_value(const char *id, size_type index, short &value) override;
  void read_value(const char *id, size_type index, int &value) override;
  void read_value(const char *id, size_type index, long &value) override;
  void read_value(const char *id, size_type index, long long &value) override;
  void read_value(const char *id, size_type index, unsigned char &value) override;
  void read_value(const char *id, size_type index, unsigned short &value) override;
  void read_value(const char *id, size_type index, unsigned int &value) override;
  void read_value(const char *id, size_type index, unsigned long &value) override;
  void read_value(const char *id, size_type index, unsigned long long &value) override;
  void read_value(const char *id, size_type index, bool &value) override;
  void read_value(const char *id, size_type index, float &value) override;
  void read_value(const char *id, size_type index, double &value) override;
  void read_value(const char *id, size_type index, matador::time &value) override;
  void read_value(const char *id, size_type index, matador::date &value) override;
  void read_value(const char *id, size_type index, char *value, size_t size) override;
  void read_value(const char *id, size_type index, std::string &value) override;
  void read_value(const char *id, size_type index, std::string &value, size_t size) override;

private:
  friend class sqlite_connection;

private:
  typedef std::vector<char*> t_row;
  typedef std::vector<t_row> t_result;

  t_result result_;

  size_type row_index_ = 0;
  size_type fields_ = 0;
};

}

}

#endif /* SQLITE_RESULT_HPP */
