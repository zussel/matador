/*
 * This file is part of OpenObjectStore OOS.
 *
 * OpenObjectStore OOS is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * OpenObjectStore OOS is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with OpenObjectStore OOS. If not, see <http://www.gnu.org/licenses/>.
 */

#include "object/action.hpp"

namespace oos
{

action::action(t_backup_func backup_func, t_restore_func restore_func)
  : backup_func_(backup_func)
  , restore_func_(restore_func)
{}

void action::backup(byte_buffer &to, action *act)
{
  backup_func_(to, act);
}

void action::restore(byte_buffer &from, action *act, object_store *store)
{
  restore_func_(from, act, store);
}

}
