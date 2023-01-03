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

  virtual ~object_serializer() = default;

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

  template < class T >
  void serialize(const char *, T &x)
  {
    buffer_->append(&x, sizeof(x));
  }

  void serialize(const char* id, char *c, size_t s);
  void serialize(const char* id, std::string &s);
  void serialize(const char *, std::string &s, size_t);

  void serialize(const char* id, date &x);
  void serialize(const char* id, time &x);

  void serialize(const char *id, basic_identifier &x);

  template < class V >
  void serialize(const char* id, identifier<V> &x)
  {
    V val(x.value());
    serialize(id, val);
  }

  void serialize(const char* id, object_holder &x, cascade_type cascade);

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
    typename basic_has_many<T, C>::size_type s = x.size();
    serialize(id, s);

    typename basic_has_many<T, C>::iterator first = x.begin();
    typename basic_has_many<T, C>::iterator last = x.end();

    while (first != last) {
      unsigned long oid(0);
      if (first.holder_item().item_proxy()) {
        oid = first.holder_item().item_proxy()->id();
      }
      // serialize holder proxy id
      serialize(id_oid.c_str(), oid);
      // serialize value
      serialize("", *first++);
    }
  }

private:
  byte_buffer *buffer_ = nullptr;
  basic_identifier_serializer basic_identifier_serializer_;
};
/// @endcond
}

#endif /* OBJECT_SERIALIZER_HPP */
