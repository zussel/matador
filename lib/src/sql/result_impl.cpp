//
// Created by sascha on 08.04.15.
//

#include "sql/result_impl.hpp"

#include "object/basic_object_holder.hpp"

namespace oos {
namespace detail {

result_impl::result_impl() {}
result_impl::~result_impl() {}

void result_impl::read_foreign_object(const char *, basic_object_holder &)
{
  /*
   * determine and create primary key of object ptr
   *
   */

//  std::shared_ptr<basic_identifier> pk = x.primary_key();
//  if (!pk) {
//    pk.reset(x.create_identifier());
//  }
//
//  pk->deserialize(id, *this);
//  if (!pk->is_valid()) {
//    // no pk is set => null
//    return;
//  }
//
//  // set found primary key into object_base_ptr
//  if (!x.has_primary_key()) {
//    x.reset(pk);
//  }
}

}
}