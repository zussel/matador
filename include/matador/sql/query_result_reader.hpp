#ifndef QUERY_QUERY_RESULT_READER_HPP
#define QUERY_QUERY_RESULT_READER_HPP

#include "matador/sql/object_result_binder.hpp"
#include "matador/sql/sql_error.hpp"

#include "matador/utils/attribute_reader.hpp"
#include "matador/utils/result.hpp"

namespace matador::sql {

namespace detail {

class empty_binder final : public utils::attribute_reader
{
public:
  void read_value(const char *, size_t, char &) override {}
  void read_value(const char *, size_t, short &) override {}
  void read_value(const char *, size_t, int &) override {}
  void read_value(const char *, size_t, long &) override {}
  void read_value(const char *, size_t, long long int &) override {}
  void read_value(const char *, size_t, unsigned char &) override {}
  void read_value(const char *, size_t, unsigned short &) override {}
  void read_value(const char *, size_t, unsigned int &) override {}
  void read_value(const char *, size_t, unsigned long &) override {}
  void read_value(const char *, size_t, unsigned long long int &) override {}
  void read_value(const char *, size_t, bool &) override {}
  void read_value(const char *, size_t, float &) override {}
  void read_value(const char *, size_t, double &) override {}
  void read_value(const char *, size_t, time &) override {}
  void read_value(const char *, size_t, date &) override {}
  void read_value(const char *, size_t, char *, size_t) override {}
  void read_value(const char *, size_t, std::string &) override {}
  void read_value(const char *, size_t, std::string &, size_t) override {}
  void read_value(const char *, size_t, utils::blob &) override {}
  void read_value(const char *, size_t, utils::value &, size_t) override {}
};

}

class query_result_reader : public utils::attribute_reader
{
public:
  [[nodiscard]] virtual size_t column_count() const = 0;
  [[nodiscard]] virtual const char* column(size_t index) const = 0;
  [[nodiscard]] virtual utils::result<bool, sql_error> fetch() = 0;
  [[nodiscard]] virtual size_t start_column_index() const;

  template<class Type>
  void bind(Type &obj) {
    object_binder_.reset();
    object_binder_.bind(obj, result_binder());
  }

public:
  void read_value(const char *id, size_t index, char &value) override;
  void read_value(const char *id, size_t index, short &value) override;
  void read_value(const char *id, size_t index, int &value) override;
  void read_value(const char *id, size_t index, long &value) override;
  void read_value(const char *id, size_t index, long long &value) override;
  void read_value(const char *id, size_t index, unsigned char &value) override;
  void read_value(const char *id, size_t index, unsigned short &value) override;
  void read_value(const char *id, size_t index, unsigned int &value) override;
  void read_value(const char *id, size_t index, unsigned long &value) override;
  void read_value(const char *id, size_t index, unsigned long long &value) override;
  void read_value(const char *id, size_t index, bool &value) override;
  void read_value(const char *id, size_t index, float &value) override;
  void read_value(const char *id, size_t index, double &value) override;
  void read_value(const char *id, size_t index, matador::time &value) override;
  void read_value(const char *id, size_t index, matador::date &value) override;
  void read_value(const char *id, size_t index, char *value, size_t s) override;
  void read_value(const char *id, size_t index, std::string &value) override;
  void read_value(const char *id, size_t index, std::string &value, size_t size) override;
  void read_value(const char *id, size_t index, utils::blob &value) override;
  void read_value(const char *id, size_t index, utils::value &val, size_t size) override;

protected:
  virtual utils::blob read_blob(size_t index);
  virtual utils::attribute_reader& result_binder();

private:
  sql::detail::empty_binder empty_result_binder_;
  object_result_binder object_binder_;
};

}
#endif //QUERY_QUERY_RESULT_READER_HPP
