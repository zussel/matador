#include "matador/utils/date.hpp"
#include "matador/utils/time.hpp"

#include "matador/object/object_deserializer.hpp"
#include "matador/object/object_store.hpp"

using namespace std::placeholders;
using namespace std;

namespace matador {

object_deserializer::object_deserializer(byte_buffer *buffer, object_store *store)
  : store_(store)
  , buffer_(buffer) {}

void object_deserializer::on_primary_key(const char *id, string &x, size_t size)
{
  on_attribute(id, x, size);
}

void object_deserializer::on_revision(const char *id, unsigned long long int &rev)
{
  on_attribute(id, rev);
}

void object_deserializer::on_attribute(const char *, char *x, const field_attributes &/*attr*/)
{
  size_t len = 0;
  buffer_->release(&len, sizeof(len));
  // TODO: check size of buffer
  buffer_->release(x, len);
}

void object_deserializer::on_attribute(const char *, std::string &x, const field_attributes &/*attr*/)
{
  size_t len = 0;
  buffer_->release(&len, sizeof(len));
  auto str = std::make_unique<char[]>(len);
  buffer_->release(str.get(), len);
  x.assign(str.get(), len);
}

void object_deserializer::on_attribute(const char *, date &x, const field_attributes &/*attr*/)
{
  int julian_date(0);
  buffer_->release(&julian_date, sizeof(julian_date));
  x.set(julian_date);
}

void object_deserializer::on_attribute(const char *, time &x, const field_attributes &/*attr*/)
{
  struct timeval tv{};
  buffer_->release(&tv.tv_sec, sizeof(tv.tv_sec));
  buffer_->release(&tv.tv_usec, sizeof(tv.tv_usec));
  x.set(tv);
}

object_proxy *object_deserializer::find_proxy(unsigned long long oid)
{
  return store_->find_proxy(oid);
}

void object_deserializer::insert_proxy(object_proxy *proxy)
{
  store_->insert_proxy(proxy);
}

std::shared_ptr<detail::object_type_registry_entry_base> object_deserializer::determine_object_type(const std::type_index& type) {
  auto node = store_->find(type.name());
  return node->object_type_entry();
}

}
