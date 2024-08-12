#ifndef QUERY_HAS_MANY_TO_MANY_RELATION_HPP
#define QUERY_HAS_MANY_TO_MANY_RELATION_HPP

#include "matador/utils/access.hpp"
#include "matador/utils/foreign_attributes.hpp"

#include "matador/object/object_ptr.hpp"

namespace matador::sql {

template < class LocalType, class ForeignType >
class has_many_to_many_relation
{
public:
  has_many_to_many_relation() = default;
  has_many_to_many_relation(std::string local_name, std::string remote_name)
  : local_name_(std::move(local_name))
  , remote_name_(std::move(remote_name)) {}

  template<class Operator>
  void process(Operator &op) {
    namespace field = matador::access;
    field::belongs_to(op, local_name_.c_str(), local_, utils::default_foreign_attributes);
    field::belongs_to(op, remote_name_.c_str(), remote_, utils::default_foreign_attributes);
  }

  object_ptr<LocalType> local() const { return local_; }
  object_ptr<LocalType> remote() const { return remote_; }

private:
  std::string local_name_;
  std::string remote_name_;
  object_ptr<LocalType> local_;
  object_ptr<ForeignType> remote_;
};

}
#endif //QUERY_HAS_MANY_TO_MANY_RELATION_HPP
