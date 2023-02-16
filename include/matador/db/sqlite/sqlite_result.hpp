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

  int transform_index(int index) const override;

  void push_back(char **row_values, int column_count);

protected:
  bool prepare_fetch() override;

  bool finalize_fetch() override;

protected:
  void read_value(const char *id, int index, int row, char &value) override;
  void read_value(const char *id, int index, int row, short &value) override;
  void read_value(const char *id, int index, int row, int &value) override;
  void read_value(const char *id, int index, int row, long &value) override;
  void read_value(const char *id, int index, int row, long long &value) override;
  void read_value(const char *id, int index, int row, unsigned char &value) override;
  void read_value(const char *id, int index, int row, unsigned short &value) override;
  void read_value(const char *id, int index, int row, unsigned int &value) override;
  void read_value(const char *id, int index, int row, unsigned long &value) override;
  void read_value(const char *id, int index, int row, unsigned long long &value) override;
  void read_value(const char *id, int index, int row, bool &value) override;
  void read_value(const char *id, int index, int row, float &value) override;
  void read_value(const char *id, int index, int row, double &value) override;
  void read_value(const char *id, int index, int row, matador::time &value) override;
  void read_value(const char *id, int index, int row, matador::date &value) override;
  void read_value(const char *id, int index, int row, char *value, long size) override;
  void read_value(const char *id, int index, int row, std::string &value) override;
  void read_value(const char *id, int index, int row, std::string &value, long size) override;

private:
  friend class sqlite_connection;

private:
  typedef std::vector<char*> t_row;
  typedef std::vector<t_row> t_result;

  t_result result_;

  size_type fields_ = 0;
};

}

}

#endif /* SQLITE_RESULT_HPP */
