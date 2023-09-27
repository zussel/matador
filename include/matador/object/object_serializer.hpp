#ifndef OBJECT_SERIALIZER_HPP
#define OBJECT_SERIALIZER_HPP

#include "matador/object/export.hpp"

#include "matador/utils/byte_buffer.hpp"
#include "matador/utils/access.hpp"
#include "matador/utils/field_attributes.hpp"

#include "matador/object/object_holder.hpp"
#include "matador/object/container.hpp"

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
    matador::access::process(*this, *o);
    buffer_ = nullptr;
  }

public:
  template < class T >
  void serialize(T &obj)
  {
    matador::access::process(*this, obj);
  }

  template < class V >
  void on_primary_key(const char* id, V &x, typename std::enable_if<std::is_integral<V>::value && !std::is_same<bool, V>::value>::type* = 0)
  {
    on_attribute(id, x);
  }
  void on_primary_key(const char *id, std::string &pk, size_t size)
  {
    on_attribute(id, pk, size);
  }
  void on_revision(const char *id, unsigned long long &rev)
  {
    on_attribute(id, rev);
  }
  template < class T >
  void on_attribute(const char *, T &x, const field_attributes &/*attr*/ = null_attributes)
  {
    buffer_->append(&x, sizeof(x));
  }
  void on_attribute(const char* id, char *c, const field_attributes &attr = null_attributes);
  void on_attribute(const char* id, std::string &s, const field_attributes &/*attr*/ = null_attributes);
  void on_attribute(const char* id, date &x, const field_attributes &/*attr*/ = null_attributes);
  void on_attribute(const char* id, time &x, const field_attributes &/*attr*/ = null_attributes);
  void on_belongs_to(const char* id, object_holder &x, cascade_type cascade);
  void on_has_one(const char* id, object_holder &x, cascade_type cascade);

  template<class T, template<class ...> class C>
  void on_has_many(const char *id, container<T, C> &x, const char *, const char *, cascade_type cascade)
  {
    on_has_many(id, x, cascade);
  }

  template<class T, template<class ...> class C>
  void on_has_many(const char *id, container<T, C> &x, cascade_type cascade)
  {
    std::string id_oid(id);
    id_oid += ".oid";
    typename container<T, C>::size_type s = x.size();
    on_attribute(id, s);

    typename container<T, C>::iterator first = x.begin();
    typename container<T, C>::iterator last = x.end();

    while (first != last) {
      unsigned long long oid(0);
      if (first.holder_item().item_proxy()) {
        oid = first.holder_item().item_proxy()->id();
      }
      // serialize holder proxy id
      on_attribute(id_oid.c_str(), oid);
      // serialize value
      process_has_many_item(*first++, cascade);
    }
  }

private:
  void on_foreign_object(const char *id, object_holder &x, cascade_type cascade);

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

private:
  byte_buffer *buffer_ = nullptr;
};
/// @endcond
}

#endif /* OBJECT_SERIALIZER_HPP */
