#ifndef QUERY_QUERY_RESULT_READER_HPP
#define QUERY_QUERY_RESULT_READER_HPP

#include "matador/query/internal/result_binder.hpp"
#include "matador/query/value_reader.hpp"

#include "matador/utils/error.hpp"
#include "matador/utils/result.hpp"

namespace matador::query {
class query_result_reader : public value_reader {
public:
  [[nodiscard]] virtual size_t column_count() const = 0;
  [[nodiscard]] virtual const char* column(size_t index) const = 0;
  [[nodiscard]] virtual result<bool, error> fetch() = 0;
  [[nodiscard]] virtual size_t start_column_index() const = 0;
  virtual void unshift() = 0;

  template<class Type>
  void bind(Type &obj) {
    object_binder_.reset();
    object_binder_.bind(obj, result_binder());
  }

protected:
  virtual value_reader& result_binder() = 0;

private:
  object_result_binder object_binder_;
};
}
#endif //QUERY_QUERY_RESULT_READER_HPP
