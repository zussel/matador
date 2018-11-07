/**
 * @author sascha on 4/21/16.
 */

#ifndef OOS_BASIC_HAS_MANY_ITEM_HPP
#define OOS_BASIC_HAS_MANY_ITEM_HPP

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

#include "matador/utils/basic_identifier.hpp"

#include <string>

namespace matador {

/// @cond MATADOR_DEV

class MATADOR_OBJECT_API basic_has_many_item
{
public:

  basic_has_many_item();
  basic_has_many_item(const std::string &owner_id_, const std::string &item_id_);
  basic_has_many_item(const std::string &owner_id_, const std::string &item_id_, basic_identifier *owner);

  virtual ~basic_has_many_item() = default;

  template < class SERIALIZER >
  void serialize(SERIALIZER &serializer)
  {
    serializer.serialize(owner_id_.c_str(), *owner_);
  }

  std::string owner_id() const;

  void owner_id(const std::string &oid);

  std::string item_id() const;

  void item_id(const std::string &iid);

  void owner(basic_identifier *owner);

  basic_identifier* owner() const;

private:
  std::string owner_id_;
  std::string item_id_;

  basic_identifier *owner_;
};

/// @endcond

}

#endif //OOS_BASIC_HAS_MANY_ITEM_HPP
