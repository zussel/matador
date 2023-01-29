#include "matador/utils/date.hpp"
#include "matador/utils/time.hpp"
#include "matador/utils/string.hpp"

#include "matador/db/sqlite/sqlite_result.hpp"

#include <cstring>
#include <algorithm>

namespace matador {

namespace sqlite {

sqlite_result::~sqlite_result()
{
  std::for_each(result_.begin(), result_.end(), [](t_result::value_type& row) {
    std::for_each(row.begin(), row.end(), [](const char *val) {
      delete [] val;
    });
  });
}

const char* sqlite_result::column(sqlite_result::size_type c) const
{
  t_row::value_type val = result_.at(pos_).at(c);
  return val;
}

bool sqlite_result::fetch()
{
  return ++pos_ < result_.size();
}

//bool sqlite_result::fetch(serializable *obj)
//{
//  if (pos_ + 1 > result_.size()) {
//    return false;
//  }
//
//  column_ = 0;
//  obj->deserialize(*this);
//  ++pos_;
//
//  return true;
//}

sqlite_result::size_type sqlite_result::affected_rows() const
{
  return 0;
}

sqlite_result::size_type sqlite_result::result_rows() const
{
  return static_cast<sqlite_result::size_type>(result_.size());
}

sqlite_result::size_type sqlite_result::fields() const
{
  return 0;
}

int sqlite_result::transform_index(int index) const
{
  return index;
}

void sqlite_result::push_back(char **row_values, int column_count)
{
  t_row row;
  for(int i = 0; i < column_count; ++i) {
    // copy and store column data;
    if (row_values[i] == nullptr) {
      auto val = new char[1];
      val[0] = '\0';
      row.push_back(val);
    } else {
      size_t size = strlen(row_values[i]);
      auto val = new char[size + 1];
      std::memcpy(val, row_values[i], size);
      val[size] = '\0';
      row.push_back(val);
    }
  }
  result_.push_back(row);
}

void sqlite_result::read_value(const char *id, int index, int row, char &x)
{
  t_row::value_type &val = result_[pos_][index];

  if (strlen(val) > 1) {
    x = val[0];
  }
}

void sqlite_result::read_value(const char *id, int index, int row, short &x)
{
  t_row::value_type &val = result_[pos_][index];
  if (strlen(val) == 0) {
    return;
  }
  char *end;
  x = (short)strtol(val, &end, 10);
  // Todo: check error
}

void sqlite_result::read_value(const char *id, int index, int row, int &x)
{
  t_row::value_type &val = result_[pos_][index];
  if (strlen(val) == 0) {
    return;
  }
  char *end;
  x = (int)strtol(val, &end, 10);
  // Todo: check error
}

void sqlite_result::read_value(const char *id, int index, int row, long &x)
{
  t_row::value_type &val = result_[pos_][index];
  if (strlen(val) == 0) {
    return;
  }
  char *end;
  x = strtol(val, &end, 10);
  // Todo: check error
}

void sqlite_result::read_value(const char *id, int index, int row, long long &x)
{
  t_row::value_type &val = result_[pos_][index];
  if (strlen(val) == 0) {
    return;
  }
  char *end;
  x = strtoll(val, &end, 10);
  // Todo: check error
}

void sqlite_result::read_value(const char *id, int index, int row, unsigned char &x)
{
  t_row::value_type &val = result_[pos_][index];
  if (strlen(val) == 0) {
    return;
  }
  char *end;
  x = (unsigned char)strtoul(val, &end, 10);
  // Todo: check error
}

void sqlite_result::read_value(const char *id, int index, int row, unsigned short &x)
{
  t_row::value_type &val = result_[pos_][index];
  if (strlen(val) == 0) {
    return;
  }
  char *end;
  x = (unsigned short)strtoul(val, &end, 10);
  // Todo: check error
}

void sqlite_result::read_value(const char *id, int index, int row, unsigned int &x)
{
  t_row::value_type &val = result_[pos_][index];
  if (strlen(val) == 0) {
    return;
  }
  char *end;
  x = (unsigned int)strtoul(val, &end, 10);
  // Todo: check error
}

void sqlite_result::read_value(const char *id, int index, int row, unsigned long &x)
{
  char *val = result_[pos_][index];
  if (strlen(val) == 0) {
    return;
  }
  char *end = nullptr;
  x = strtoul(val, &end, 0);
  // Todo: check error
}

void sqlite_result::read_value(const char *id, int index, int row, unsigned long long &x)
{
  char *val = result_[pos_][index];
  if (strlen(val) == 0) {
    return;
  }
  char *end = nullptr;
  x = strtoull(val, &end, 10);
  // Todo: check error
}

void sqlite_result::read_value(const char *id, int index, int row, bool &x)
{
  t_row::value_type &val = result_[pos_][index];
  if (strlen(val) == 0) {
    return;
  }
  char *end;
  x = strtoul(val, &end, 10) > 0;
  // Todo: check error
}

void sqlite_result::read_value(const char *id, int index, int row, float &x)
{
  t_row::value_type &val = result_[pos_][index];
  if (strlen(val) == 0) {
    return;
  }
  char *end;
  x = (float)strtod(val, &end);
  // Todo: check error
}

void sqlite_result::read_value(const char *id, int index, int row, double &x)
{
  t_row::value_type &val = result_[pos_][index];
  if (strlen(val) == 0) {
    return;
  }
  char *end;
  x = strtod(val, &end);
  // Todo: check error
}

void sqlite_result::read_value(const char *id, int index, int row, char *x, long size)
{
  t_row::value_type &val = result_[pos_][index];
  size_t len = strlen(val);
  if (len > (size_t)size) {
#ifdef _MSC_VER
    strncpy_s(x, size, val, len);
#else
    strncpy(x, val, size);
#endif
    x[size-1] = '\n';
  } else {
#ifdef _MSC_VER
    strcpy_s(x, size, val);
#else
    strcpy(x, val);
#endif
  }
}

void sqlite_result::read_value(const char *id, int index, int row, std::string &x, long /*size*/)
{
  t_row::value_type val = result_[pos_][index];
  x.assign(val);
}

void sqlite_result::read_value(const char *id, int index, int row, std::string &x)
{
  t_row::value_type val = result_[pos_][index];
  x = val;
}

void sqlite_result::read_value(const char *id, int index, int row, matador::date &x)
{
  t_row::value_type val = result_[pos_][index];
  x.set(val, date_format::ISO8601);
}

void sqlite_result::read_value(const char *id, int index, int row, matador::time &x)
{
  t_row::value_type val = result_[pos_][index];
  x = matador::time::parse(val, "%Y-%m-%dT%T.%f");
}

bool sqlite_result::prepare_fetch()
{
  column_ = 0;
  return pos_ + 1 <= result_.size();
}

bool sqlite_result::finalize_fetch()
{
  ++pos_;
  return true;
}

}

}
