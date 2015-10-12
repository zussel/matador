//
// Created by sascha on 08.04.15.
//

#include "database/result_impl.hpp"

#include "object/object_exception.hpp"
#include "object/object_ptr.hpp"
#include "object/prototype_tree.hpp"
#include "object/serializable.hpp"

namespace oos {
namespace detail {

result_impl::result_impl(std::shared_ptr<object_base_producer> producer)
  : producer_(producer)
{}

result_impl::~result_impl()
{}

void result_impl::get(serializable *o)
{
  result_index = transform_index(0);
  o->deserialize(*this);
}

void result_impl::read_foreign_object(const char *id, object_base_ptr &x)
{
  /*
   * determine and create primary key of object ptr
   *
   */

  std::shared_ptr<basic_identifier> pk = x.primary_key();
  if (!pk) {
    pk.reset(x.create_identifier());
  }

  pk->deserialize(id, *this);
  if (!pk->is_valid()) {
    // no pk is set => null
    return;
  }

  // set found primary key into object_base_ptr
  if (!x.has_primary_key()) {
    x.reset(pk);
  }
}

std::shared_ptr<object_base_producer> result_impl::producer() const
{
  return producer_;
}

}
}