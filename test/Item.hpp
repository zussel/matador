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

#ifndef ITEM_HPP
#define ITEM_HPP

#include <string>
#include <cstring>
#include <ostream>

#include "matador/utils/time.hpp"
#include "matador/utils/date.hpp"
#include "matador/utils/identifier.hpp"
#include "matador/utils/varchar.hpp"

class Item
{
public:
  Item() : Item("") { }

  explicit Item(const std::string &str) : Item(str, -65000) {}

  Item(std::string str, int i)
    : int_(i), string_(std::move(str))
  {
    memset(cstr_, 0, CSTR_LEN);
#ifdef _MSC_VER
    strcpy_s(cstr_, CSTR_LEN, "Hallo");
#else
    strcpy(cstr_, "Hallo");
#endif
    cstr_[5] = '\0';
  }

  ~Item() = default;

public:
  template < class SERIALIZER > void serialize(SERIALIZER &serializer)
  {
    serializer.serialize("id", id_);
    serializer.serialize("val_char", char_);
    serializer.serialize("val_float", float_);
    serializer.serialize("val_double", double_);
    serializer.serialize("val_short", short_);
    serializer.serialize("val_int", int_);
    serializer.serialize("val_long", long_);
    serializer.serialize("val_unsigned_short", unsigned_short_);
    serializer.serialize("val_unsigned_int", unsigned_int_);
    serializer.serialize("val_unsigned_long", unsigned_long_);
    serializer.serialize("val_bool", bool_);
    serializer.serialize("val_cstr", cstr_, (size_t)CSTR_LEN);
    serializer.serialize("val_string", string_);
    serializer.serialize("val_varchar", varchar_);
    serializer.serialize("val_date", date_);
//    serializer.serialize("val_time", time_);
  }

  unsigned long id() const { return id_.value(); }
  void id(unsigned long i) { id_.value(i); }

  void set_char(char x) { char_ = x; }
  void set_float(float x) { float_ = x; }
  void set_double(double x) { double_ = x; }
  void set_short(short x) { short_ = x; }
  void set_int(int x) { int_ = x; }
  void set_long(long x) { long_ = x; }
  void set_unsigned_short(unsigned short x) { unsigned_short_ = x; }
  void set_unsigned_int(unsigned int x) { unsigned_int_ = x; }
  void set_unsigned_long(unsigned long x) { unsigned_long_ = x; }
  void set_bool(bool x) { bool_ = x; }
  void set_cstr(const char *x, size_t size)
  {
    if (CSTR_LEN < size) {
      throw std::logic_error("not enough character size");
    }
//    mark_modified();
#ifdef _MSC_VER
    strcpy_s(cstr_, CSTR_LEN, x);
#else
    strcpy(cstr_, x);
#endif
  }
  void set_string(const std::string &x) { string_ = x; }
  void set_varchar(const matador::varchar_base &x) { varchar_ = x.c_str(); }
  void set_date(const matador::date &d) { date_ = d; }
  void set_time(const matador::time &d) { time_ = d; }

  char get_char() const { return char_; }
  float get_float() const { return float_; }
  double get_double() const { return double_; }
  short get_short() const { return short_; }
  int get_int() const { return int_; }
  long get_long() const { return long_; }
  unsigned short get_unsigned_short() const { return unsigned_short_; }
  unsigned int get_unsigned_int() const { return unsigned_int_; }
  unsigned long get_unsigned_long() const { return unsigned_long_; }
  bool get_bool() const { return bool_; }
  const char* get_cstr() const { return cstr_; }
  std::string get_string() const { return string_; }
  matador::varchar_base get_varchar() const { return varchar_; }
  matador::date get_date() const { return date_; }
  matador::time get_time() const { return time_; }

  friend std::ostream& operator <<(std::ostream &os, const Item &i)
  {
    os << "Item [" << i.get_string() << "] (" << i.get_int() << ")";
    return os;
  }

private:
  enum { CSTR_LEN=256 };

  matador::identifier<unsigned long> id_ = 0;

  char char_ = 'c';
  float float_ = 3.1415f;
  double double_ = 1.1414;
  short short_ = -127;
  int int_ = -65000;
  long long_ = -128000;
  unsigned short unsigned_short_ = 128;
  unsigned int unsigned_int_ = 65000;
  unsigned long unsigned_long_ = 128000;
  bool bool_ = true;
  char cstr_[CSTR_LEN]{};
  std::string string_ = "Welt";
  matador::varchar<64> varchar_ = "Erde";
  matador::date date_;
  matador::time time_;
};

class ItemA : public Item {};
class ItemB : public Item {};
class ItemC : public Item {};

#endif /* ITEM_HPP */
