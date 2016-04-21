/**
 * @author sascha on 4/21/16.
 */

#include "object/basic_has_many_item.hpp"

namespace oos {

basic_has_many_item::basic_has_many_item()
  : basic_has_many_item("owner_id", "item_id")
{ }

basic_has_many_item::basic_has_many_item(const std::string &owner_id, const std::string &item_id)
  : basic_has_many_item(owner_id, item_id, nullptr)
{ }

basic_has_many_item::basic_has_many_item(const std::string &owner_id, const std::string &item_id,
                                         const std::shared_ptr<basic_identifier> owner)
  : owner_id_(owner_id)
  , item_id_(item_id)
  , owner_(owner)
{ }

void basic_has_many_item::owner_id(const std::string &oid)
{
  owner_id_ = oid;
}

std::string basic_has_many_item::item_id() const
{
  return item_id_;
}

void basic_has_many_item::item_id(const std::string &iid)
{
  item_id_ = iid;
}

basic_identifier *basic_has_many_item::owner() const
{
  return owner_.get();
}

void basic_has_many_item::owner(basic_identifier *owner)
{
  owner_.reset(owner);
}

std::string basic_has_many_item::owner_id() const
{
  return owner_id_;
}

}