#ifndef MATADOR_OBJECT_TYPE_REGISTRY_ENTRY_BASE_HPP
#define MATADOR_OBJECT_TYPE_REGISTRY_ENTRY_BASE_HPP

#ifdef _MSC_VER
#ifdef matador_object_EXPORTS
#define MATADOR_OBJECT_API __declspec(dllexport)
    #define EXPIMP_OBJECT_TEMPLATE
#else
#define MATADOR_OBJECT_API __declspec(dllimport)
#define EXPIMP_OBJECT_TEMPLATE extern
#endif
#pragma warning(disable: 4251)
#else
#define MATADOR_OBJECT_API
#endif

#include <typeindex>
#include <memory>

namespace matador {

class object_proxy;
class byte_buffer;
class object_store;
class object_serializer;
class object_deserializer;
class prototype_node;
class object_store;
class basic_identifier;

namespace detail {

class object_inserter;
class object_deleter;

class MATADOR_OBJECT_API object_type_registry_entry_base
{
protected:
  object_type_registry_entry_base(const std::type_index &ti, object_store *store, prototype_node *node);

  virtual ~object_type_registry_entry_base() = default;

public:
  object_store* store() const;
  prototype_node* node() const;

  virtual void sync_id(object_proxy *proxy) const = 0;
  virtual void restore(object_proxy *proxy, byte_buffer &, object_deserializer &) const = 0;
  virtual void backup(object_proxy *proxy, byte_buffer &, object_serializer &) const = 0;
  virtual void create_object(object_proxy *proxy) const = 0;
  virtual void insert_object(object_proxy *proxy) const = 0;
  virtual void delete_object(object_proxy *proxy) const = 0;
  virtual basic_identifier* resolve_identifier(object_proxy *proxy) const = 0;
  virtual void mark_modified(object_proxy *proxy);

  const std::type_index& type_index() const;

protected:
  object_inserter& inserter() const;
  object_deleter& deleter() const;

private:
  object_store *store_ = nullptr;
  prototype_node *node_ = nullptr;
  const std::type_index type_index_;
};

class MATADOR_OBJECT_API null_object_type_registry_entry : public object_type_registry_entry_base
{
private:
  struct MATADOR_OBJECT_API null_type {};

public:
  null_object_type_registry_entry();

  void sync_id(object_proxy *proxy) const override;
  void restore(object_proxy *proxy, byte_buffer &, object_deserializer &) const override;
  void backup(object_proxy *proxy, byte_buffer &, object_serializer &) const override;
  void create_object(object_proxy *proxy) const override;
  void insert_object(object_proxy *proxy) const override;
  void delete_object(object_proxy *proxy) const override;
  basic_identifier* resolve_identifier(object_proxy *proxy) const override;
  void mark_modified(object_proxy *proxy) override;

  static std::shared_ptr<object_type_registry_entry_base> null_type_entry;
};

}
}
#endif //MATADOR_OBJECT_TYPE_REGISTRY_ENTRY_BASE_HPP
