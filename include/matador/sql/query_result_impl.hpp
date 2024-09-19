#ifndef QUERY_QUERY_RESULT_IMPL_HPP
#define QUERY_QUERY_RESULT_IMPL_HPP

#include "matador/utils/access.hpp"
#include "matador/utils/field_attributes.hpp"
#include "matador/utils/foreign_attributes.hpp"

#include "matador/sql/query_result_reader.hpp"
#include "matador/sql/column_definition.hpp"
#include "matador/object/data_type_traits.hpp"

#include <memory>
#include <string>

namespace matador::sql {

class value;

namespace detail {
class pk_reader
{
public:
  explicit pk_reader(query_result_reader &reader);

  template<class Type>
  void read(Type &obj, const size_t column_index)
  {
    column_index_ = column_index;
    access::process(*this, obj);
  }

  template<typename ValueType>
  void on_primary_key(const char *id, ValueType &value, std::enable_if_t<std::is_integral_v<ValueType> && !std::is_same_v<bool, ValueType>>* = nullptr);
  void on_primary_key(const char *id, std::string &value, size_t size);
  void on_revision(const char * /*id*/, unsigned long long &/*rev*/) {}

  template < class Type >
  void on_attribute(const char * /*id*/, Type &/*x*/, const utils::field_attributes &/*attr*/ = utils::null_attributes) {}
  template < class Pointer >
  void on_belongs_to(const char * /*id*/, Pointer &/*x*/, const utils::foreign_attributes &/*attr*/) {}
  template < class Pointer >
  void on_has_one(const char * /*id*/, Pointer &/*x*/, const utils::foreign_attributes &/*attr*/) {}

  template<class ContainerType>
  void on_has_many(const char * /*id*/, ContainerType &, const char *, const utils::foreign_attributes &/*attr*/) {}
  template<class ContainerType>
  void on_has_many_to_many(const char *id, ContainerType &c, const char *join_column, const char *inverse_join_column, const utils::foreign_attributes &/*attr*/) {}
  template<class ContainerType>
  void on_has_many_to_many(const char *id, ContainerType &c, const utils::foreign_attributes &/*attr*/) {}

private:
  size_t column_index_{};
  query_result_reader &reader_;
};

}

class query_result_impl
{
public:
  query_result_impl(std::unique_ptr<query_result_reader> &&reader, std::vector<column_definition> prototype, size_t column_index = 0);

  template<typename ValueType>
  void on_primary_key(const char *id, ValueType &value, std::enable_if_t<std::is_integral_v<ValueType> && !std::is_same_v<bool, ValueType>>* = nullptr)
  {
    object::data_type_traits<ValueType>::read_value(*reader_, id, column_index_++, value);
  }
  void on_primary_key(const char *id, std::string &value, size_t size);
  void on_revision(const char *id, unsigned long long &rev);

  template < class Type >
  void on_attribute(const char *id, Type &x, const utils::field_attributes &/*attr*/ = utils::null_attributes)
  {
    object::data_type_traits<Type>::read_value(*reader_, id, column_index_++, x);
  }
  void on_attribute(const char *id, char *value, const utils::field_attributes &attr = utils::null_attributes);
  void on_attribute(const char *id, std::string &value, const utils::field_attributes &attr = utils::null_attributes);
  void on_attribute(const char *id, value &val, const utils::field_attributes &attr = utils::null_attributes);

  template < class Pointer >
  void on_belongs_to(const char * /*id*/, Pointer &x, const utils::foreign_attributes &attr)
  {
    if (x.empty()) {
      x = new typename Pointer::value_type;
    }
    if (attr.fetch() == utils::fetch_type::LAZY) {
      pk_reader_.read(*x, column_index_++);
    } else {
      access::process(*this, *x);
    }
  }
  template < class Pointer >
  void on_has_one(const char * /*id*/, Pointer &x, const utils::foreign_attributes &attr)
  {
    if (x.empty()) {
      x = new typename Pointer::value_type;
    }
    if (attr.fetch() == utils::fetch_type::LAZY) {
      pk_reader_.read(*x, column_index_++);
    } else {
      access::process(*this, *x);
    }
  }

  template<class ContainerType>
  void on_has_many_to_many(const char *, ContainerType &, const char * /*join_column*/, const char * /*inverse_join_column*/, const utils::foreign_attributes &/*attr*/) {}
  template<class ContainerType>
  void on_has_many_to_many(const char *, ContainerType &, const utils::foreign_attributes &/*attr*/) {}
  template<class ContainerType>
  void on_has_many(const char *id, ContainerType &cont, const char * join_column, const utils::foreign_attributes &attr) {
      if ( attr.fetch() == utils::fetch_type::LAZY ) {
          // pk_reader_.read(*id, column_index_++);
      } else {
          auto obj = std::make_unique<typename ContainerType::value_type::value_type>();
          // typename ContainerType::value_type x(new typename ContainerType::value_type::value_type);
          access::process(*this, *obj);
          cont.push_back(typename ContainerType::value_type(obj.release()));
      }
      std::cout << "handle attribute " << id << " with join column " << join_column << "(column index: " << column_index_ << ")\n";
  }
  template<class ContainerType>
  void on_has_many(const char *, ContainerType &, const utils::foreign_attributes &/*attr*/) {}

  template<class Type>
  void bind(const Type &) {}

  template<class Type>
  bool fetch(Type &obj)
  {
    column_index_ = reader_->start_column_index();
    if (!reader_->fetch()) {
      return false;
    }
    access::process(*this, obj);
    return true;
  }

  [[nodiscard]] const std::vector<column_definition>& prototype() const;

protected:
  size_t column_index_ = 0;
  std::vector<column_definition> prototype_;
  std::unique_ptr<query_result_reader> reader_;
  detail::pk_reader pk_reader_;
};

namespace detail {

template<typename ValueType>
void detail::pk_reader::on_primary_key(const char *id, ValueType &value, std::enable_if_t<std::is_integral_v<ValueType> && !std::is_same_v<bool, ValueType>> *)
{
  object::data_type_traits<ValueType>::read_value(reader_, id, column_index_++, value);
}

}
}

#endif //QUERY_QUERY_RESULT_IMPL_HPP
