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

#include "SessionTestUnit.hpp"

#include "../Item.hpp"

#include "database/session.hpp"

#include <iostream>
#include <fstream>

using namespace oos;
using namespace std;

SessionTestUnit::SessionTestUnit(const std::string &name, const std::string &msg, const std::string &db)
  : unit_test(name, msg)
  , db_(db)
  , session_(nullptr)
{
  add_test("open_close", std::bind(&SessionTestUnit::test_open_close, this), "open database test");
  add_test("create_drop", std::bind(&SessionTestUnit::test_create_drop, this), "create drop database test");
  add_test("reopen", std::bind(&SessionTestUnit::test_reopen, this), "reopen database test");
//  add_test("drop", std::bind(&SessionTestUnit::test_drop, this), "drop database test");
}

SessionTestUnit::~SessionTestUnit()
{}

void
SessionTestUnit::initialize()
{
  ostore_.insert_prototype<album>("album");
  ostore_.insert_prototype<track>("track");

  ostore_.insert_prototype<person>("person");
  ostore_.insert_prototype<employee>("employee");
  ostore_.insert_prototype<department>("department");

  // create session
  session_ = create_session();
}

void
SessionTestUnit::finalize()
{
  delete session_;
  ostore_.clear(true);
}

void
SessionTestUnit::test_open_close()
{
  // create database and make serializable store known to the database
  UNIT_ASSERT_FALSE(session_->is_open(), "session must not be open");

  session_->open();

  UNIT_ASSERT_TRUE(session_->is_open(), "couldn't open database database");

  session_->close();

  UNIT_ASSERT_FALSE(session_->is_open(), "couldn't close database database");
}

void
SessionTestUnit::test_create_drop()
{
  UNIT_ASSERT_FALSE(session_->is_open(), "session must not be open");

  session_->open();

  UNIT_ASSERT_TRUE(session_->is_open(), "couldn't open database database");

  session_->create();

  session_->drop();

  session_->close();

  UNIT_ASSERT_FALSE(session_->is_open(), "couldn't close database database");
}

void SessionTestUnit::test_drop()
{
  UNIT_ASSERT_FALSE(session_->is_open(), "session must not be open");

  session_->open();

  UNIT_ASSERT_TRUE(session_->is_open(), "couldn't open database database");

  session_->drop();

  session_->close();

  UNIT_ASSERT_FALSE(session_->is_open(), "couldn't close database database");
}

void SessionTestUnit::test_reopen()
{
  UNIT_ASSERT_FALSE(session_->is_open(), "session must not be open");

  session_->open();

  UNIT_ASSERT_TRUE(session_->is_open(), "couldn't open database database");

  session_->close();

  session_->open();

  UNIT_ASSERT_TRUE(session_->is_open(), "couldn't open database database");

  session_->close();

  UNIT_ASSERT_FALSE(session_->is_open(), "couldn't close database database");
}

session* SessionTestUnit::create_session()
{
  return new session(ostore_, db_);
}
