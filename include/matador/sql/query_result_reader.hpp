#ifndef QUERY_QUERY_RESULT_READER_HPP
#define QUERY_QUERY_RESULT_READER_HPP

#include "matador/sql/object_result_binder.hpp"

#include "matador/object/data_type_traits.hpp"
#include "matador/object/attribute_reader.hpp"

namespace matador::sql {

namespace detail {

class empty_binder : public object::attribute_reader
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
};

}

class value;

class query_result_reader : public object::attribute_reader
{
public:
  [[nodiscard]] virtual size_t column_count() const = 0;
  [[nodiscard]] virtual const char* column(size_t index) const = 0;
  [[nodiscard]] virtual bool fetch() = 0;
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
  virtual void read_value(const char *id, size_t index, value &val, size_t size);

protected:
  virtual utils::blob read_blob(size_t index);
  virtual object::attribute_reader& result_binder();

private:
  sql::detail::empty_binder empty_result_binder_;
  object_result_binder object_binder_;
};

}
#endif //QUERY_QUERY_RESULT_READER_HPP
