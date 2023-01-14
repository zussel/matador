#ifndef MATADOR_OBJECT_DESERIALIZER_HPP
#define MATADOR_OBJECT_DESERIALIZER_HPP

#include "matador/object/export.hpp"

#include "matador/utils/byte_buffer.hpp"
#include "matador/utils/access.hpp"
#include "matador/utils/identifier.hpp"

#include "matador/object/object_ptr.hpp"
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

  template<class T>
  void serialize(const char *, T &x)
  {
    buffer_->release(&x, sizeof(x));
  }

  void serialize(const char *id, char *c, size_t s);
  void serialize(const char *id, std::string &s);
  void serialize(const char *, std::string &s, size_t);
  void serialize(const char *id, date &x);
  void serialize(const char *id, time &x);
  void serialize(const char *id, basic_identifier &x);

  template<class V>
  void serialize(const char *id, identifier<V> &x)
  {
    V val;
    serialize(id, val);
    x.value(val);
  }

  template<class T, object_holder_type OHT>
  void serialize(const char *id, object_pointer<T, OHT> &x, cascade_type cascade)
  {
    /***************
     *
     * extract id and type of serializable from buffer
     * try to find serializable on serializable store
     * if found check type if wrong type throw error
     * else create serializable and set extracted id
     * insert serializable into serializable store
     *
     ***************/
    unsigned long oid = 0;
    serialize(id, oid);

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

  template<class T, template<class ...> class C>
  void serialize(const char *id, basic_has_many<T, C> &x, const char *, const char *, cascade_type cascade)
  {
    serialize(id, x, cascade);
  }

  template<class T, template<class ...> class C>
  void serialize(const char *id, basic_has_many<T, C> &x, cascade_type)
  {
    std::string id_oid(id);
    id_oid += ".oid";
    typename basic_has_many<T, C>::size_type s = 0;
    // deserialize container size
    serialize(id, s);

    x.reset();

    for (typename basic_has_many<T, C>::size_type i = 0; i < s; ++i) {

      // deserialize all items
      unsigned long oid = 0;
      serialize(id_oid.c_str(), oid);


      // and append them to container
      object_proxy *proxy = x.relation_info_->acquire_proxy(oid, *store_);
      if (proxy) {
        insert_proxy(proxy);
      }

      typename has_many_item_holder<T>::object_type val;
      serialize("", val);

      x.append(has_many_item_holder<T>(val, proxy));
    }
  }

private:
  object_proxy *find_proxy(unsigned long oid);

  void insert_proxy(object_proxy *proxy);

  std::shared_ptr<detail::object_type_registry_entry_base> determine_object_type(const std::type_index& type);

private:
  object_store *store_ = nullptr;
  byte_buffer *buffer_ = nullptr;
  basic_identifier_serializer basic_identifier_serializer_;
};
/// @endcond

}
#endif //MATADOR_OBJECT_DESERIALIZER_HPP
