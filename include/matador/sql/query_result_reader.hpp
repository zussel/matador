#ifndef QUERY_QUERY_RESULT_READER_HPP
#define QUERY_QUERY_RESULT_READER_HPP

#include "matador/object/data_type_traits.hpp"

#include "matador/object/attribute_reader.hpp"

namespace matador::sql {

class value;

class query_result_reader : public object::attribute_reader
{
public:
  [[nodiscard]] virtual size_t column_count() const = 0;
  [[nodiscard]] virtual const char* column(size_t index) const = 0;
  [[nodiscard]] virtual bool fetch() = 0;
  [[nodiscard]] virtual size_t start_column_index() const;

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
};

}
#endif //QUERY_QUERY_RESULT_READER_HPP
