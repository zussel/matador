#ifndef MATADOR_OBJECT_DESERIALIZER_HPP
#define MATADOR_OBJECT_DESERIALIZER_HPP

#include "matador/object/export.hpp"

#include "matador/utils/byte_buffer.hpp"
#include "matador/utils/access.hpp"
#include "matador/utils/field_attributes.hpp"

#include "matador/object/basic_has_many.hpp"

#include <string>
#include <cstring>

namespace matador {

class object_holder;

/**
 * @cond MATADOR_DEV
 * @class object_deserializer
 * @brief serialize objects to a given buffer.
 *
 * The object_deserializer class serialize a given
 * object to a given buffer or deserialize a given
 * serializable from a given buffer.
 * On deserialization the class doesn't take care
 * of the correctness of the serializable and the current
 * memory the buffer points to.
 * The application is responsible for this correctness.
 */
class MATADOR_OBJECT_API object_deserializer
{
public:
  /**
   * Creates an object_serializer
   */
  object_deserializer() = default;
  object_deserializer(byte_buffer *buffer, object_store *store);

  ~object_deserializer() = default;

  /**
   * Serialize the given object to the given buffer
   *
   * @param o The object to deserialize.
   * @param buffer The byte_buffer to deserialize from.
   * @param store The object_store where the serializable resides.
   * @return True on success.
   */
  template<class T>
  void restore(T *o, byte_buffer *buffer, object_store *store)
  {
    store_ = store;
    buffer_ = buffer;
    matador::access::serialize(*this, *o);
    buffer_ = nullptr;
    store_ = nullptr;
  }

public:
  template<class T>
  void serialize(T &obj)
  {
    matador::access::serialize(*this, obj);
  }

  template<class V>
  void on_primary_key(const char *id, V &x, const field_attributes &attr = {})
  {
    on_attribute(id, x, attr);
  }

  template<class T>
  void on_attribute(const char *, T &x, const field_attributes &/*attr*/ = {})
  {
    buffer_->release(&x, sizeof(x));
  }
  void on_attribute(const char *id, char *x, const field_attributes &attr);
  void on_attribute(const char *id, std::string &x, const field_attributes &attr = {});
  void on_attribute(const char *id, date &x, const field_attributes &/*attr*/ = {});
  void on_attribute(const char *id, time &x, const field_attributes &/*attr*/ = {});

  template<class T>
  void on_belongs_to(const char *id, matador::object_ptr<T> &x, cascade_type cascade)
  {
    on_foreign_object(id, x, cascade);
  }

  template<class T>
  void on_has_one(const char *id, matador::object_ptr<T> &x, cascade_type cascade)
  {
    on_foreign_object(id, x, cascade);
  }

  template<class T, template<class ...> class C>
  void on_has_many(const char *id, basic_has_many<T, C> &x, const char *, const char *, cascade_type cascade)
  {
    on_has_many(id, x, cascade);
  }

  template<class T, template<class ...> class C>
  void on_has_many(const char *id, basic_has_many<T, C> &x, cascade_type cascade)
  {
    std::string id_oid(id);
    id_oid += ".oid";
    typename basic_has_many<T, C>::size_type s{};
    // deserialize container size
    on_attribute(id, s);

    x.reset();

    for (typename basic_has_many<T, C>::size_type i = 0; i < s; ++i) {

      // deserialize all items
      unsigned long long oid = 0;
      on_attribute(id_oid.c_str(), oid);

      // and append them to container
      object_proxy *proxy = x.relation_info_->acquire_proxy(oid, *store_);
      if (proxy) {
        insert_proxy(proxy);
      }

      typename has_many_item_holder<T>::object_type val;
      process_has_many_item(val, cascade);

      x.append(has_many_item_holder<T>(val, proxy));
    }
  }

private:
  template<class T>
  void process_has_many_item(T &obj, cascade_type cascade, typename std::enable_if<!matador::is_builtin<T>::value>::type* = 0)
  {
    on_foreign_object("", obj, cascade);
  }

  template<class T>
  void process_has_many_item(T &attr, cascade_type /*cascade*/, typename std::enable_if<matador::is_builtin<T>::value>::type* = 0)
  {
    on_attribute("", attr);
  }

  template<class T>
  void on_foreign_object(const char *id, matador::object_ptr<T> &x, cascade_type cascade)
  {
    /***************
     *
     * extract id and type of serializable from buffer
     * try to find object on object store
     * if found check type if wrong type throw error
     * else create object and set extracted id
     * insert object into object store
     *
     ***************/
    unsigned long long oid = 0;
    on_attribute(id, oid);

    if (oid > 0) {
      object_proxy *oproxy = find_proxy(oid);
      if (!oproxy) {
        auto object_type = determine_object_type(std::type_index(typeid(T)));
        oproxy = new object_proxy(new T, oid, object_type);
        insert_proxy(oproxy);
      }
      x.reset(oproxy, cascade);
    }
  }
private:
  object_proxy *find_proxy(unsigned long long oid);

  void insert_proxy(object_proxy *proxy);

  std::shared_ptr<detail::object_type_registry_entry_base> determine_object_type(const std::type_index& type);

private:
  object_store *store_ = nullptr;
  byte_buffer *buffer_ = nullptr;
};
/// @endcond

}
#endif //MATADOR_OBJECT_DESERIALIZER_HPP
