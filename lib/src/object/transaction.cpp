//
// Created by sascha on 3/9/16.
//

#include <object/object_proxy.hpp>
#include "object/transaction.hpp"

namespace oos {

transaction::transaction(oos::object_store &store)
  : store_(store)
{}

void transaction::begin()
{

}

void transaction::commit()
{

}

void transaction::rollback()
{

}

void transaction::abort()
{

}

}