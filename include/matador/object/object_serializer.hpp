#ifndef OBJECT_SERIALIZER_HPP
#define OBJECT_SERIALIZER_HPP

#include "matador/object/export.hpp"

#include "matador/utils/byte_buffer.hpp"
#include "matador/utils/access.hpp"
#include "matador/utils/identifier.hpp"

#include "matador/object/object_holder.hpp"
#include "matador/object/basic_has_many.hpp"

#include <string>
#include <cstring>

namespace matador {

class object_holder;

/**
 * @cond MATADOR_DEV
 * @class object_serializer
 * @brief serialize objects to a given buffer.
 *
 * The object_serializer class serialize a given
 * serializable to a given buffer or deserialize a given
 * serializable from a given buffer.
 * On deserialization the class doesn't take care
 * of the correctness of the serializable and the current
 * memory the buffer points to.
 * The application is responsible for this correctness.
 */
class MATADOR_OBJECT_API
object_serializer
{
public:
  /**
   * Creates an object_serializer
   */
  object_serializer() = default;
  explicit object_serializer(byte_buffer *buffer);

  ~object_serializer() = default;

  /**
   * Serialize the given object to the given buffer
   *
   * @param o The serializable to serialize.
   * @param buffer The byte_buffer to serialize to.
   * @return True on success.
   */
  template < class T >
  void backup(T *o, byte_buffer *buffer)
  {
    buffer_ = buffer;
    matador::access::serialize(*this, *o);
    buffer_ = nullptr;
  }

public:
  template < class T >
  void serialize(T &obj)
  {
    matador::access::serialize(*this, obj);
  }

  template < class V >
  void on_primary_key(const char* id, V &x, long size = -1)
  {
    on_attribute(id, x, size);
  }
  template < class T >
  void on_attribute(const char *, T &x, long /*size*/ = -1)
  {
    buffer_->append(&x, sizeof(x));
  }
  void on_attribute(const char* id, char *c, long size);
  void on_attribute(const char* id, std::string &s, long /*size*/ = -1);
  void on_attribute(const char* id, date &x, long /*size*/ = -1);
  void on_attribute(const char* id, time &x, long /*size*/ = -1);
  void on_belongs_to(const char* id, object_holder &x, cascade_type cascade);
  void on_has_one(const char* id, object_holder &x, cascade_type cascade);

  template<class T, template<class ...> class C>
  void on_has_many(const char *id, basic_has_many<T, C> &x, const char *, const char *, cascade_type cascade)
  {
    on_has_many(id, x, cascade);
  }

  template<class T, template<class ...> class C>
  void on_has_many(const char *id, basic_has_many<T, C> &x, cascade_type)
  {
    std::string id_oid(id);
    id_oid += ".oid";
    typename basic_has_many<T, C>::size_type s = x.size();
    on_attribute(id, s);

    typename basic_has_many<T, C>::iterator first = x.begin();
    typename basic_has_many<T, C>::iterator last = x.end();

    while (first != last) {
      unsigned long oid(0);
      if (first.holder_item().item_proxy()) {
        oid = first.holder_item().item_proxy()->id();
      }
      // serialize holder proxy id
      on_attribute(id_oid.c_str(), oid);
      // serialize value
      on_attribute("", *first++);
    }
  }

private:
  byte_buffer *buffer_ = nullptr;
};
/// @endcond
}

#endif /* OBJECT_SERIALIZER_HPP */
