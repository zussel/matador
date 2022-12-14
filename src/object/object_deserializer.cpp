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

void object_deserializer::serialize(const char *, char *c, size_t)
{
  size_t len = 0;
  buffer_->release(&len, sizeof(len));
  // TODO: check size of buffer
  buffer_->release(c, len);
}

void object_deserializer::serialize(const char *, std::string &s, size_t)
{
  size_t len = 0;
  buffer_->release(&len, sizeof(len));
  char *str = new char[len];
  buffer_->release(str, len);
  s.assign(str, len);
  delete [] str;
}

void object_deserializer::serialize(const char *, std::string &s)
{
  size_t len = 0;
  buffer_->release(&len, sizeof(len));
  char *str = new char[len];
  buffer_->release(str, len);
  s.assign(str, len);
  delete [] str;
}

void object_deserializer::serialize(const char *, date &x)
{
  int julian_date(0);
  buffer_->release(&julian_date, sizeof(julian_date));
  x.set(julian_date);
}

void object_deserializer::serialize(const char *, time &x)
{
  struct timeval tv{};
  buffer_->release(&tv.tv_sec, sizeof(tv.tv_sec));
  buffer_->release(&tv.tv_usec, sizeof(tv.tv_usec));
  x.set(tv);
}

void object_deserializer::serialize(const char *, basic_identifier &x)
{
  basic_identifier_serializer_.deserialize(x, *buffer_);
}

object_proxy *object_deserializer::find_proxy(unsigned long oid)
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
