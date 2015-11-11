//
// Created by sascha on 25.09.15.
//

#ifndef OOS_BASIC_IDENTIFIER_HPP
#define OOS_BASIC_IDENTIFIER_HPP

#ifdef _MSC_VER
#ifdef oos_EXPORTS
#define OOS_API __declspec(dllexport)
#define EXPIMP_TEMPLATE
#else
#define OOS_API __declspec(dllimport)
#define EXPIMP_TEMPLATE extern
#endif
#pragma warning(disable: 4251)
#else
#define OOS_API
#endif

#include "object/object_atomizer.hpp"

#include <typeindex>
#include <iosfwd>
#include <stdexcept>

namespace oos {

template < typename T, class Enable = void >
class identifier;

class OOS_API basic_identifier
{
public:
  basic_identifier();
  virtual ~basic_identifier();

  bool operator==(const basic_identifier &x) const;
  bool operator!=(const basic_identifier &x) const;
  bool operator<(const basic_identifier &x) const;

  virtual void serialize(const char*, object_writer&) const = 0;
  virtual void deserialize(const char*, object_reader&) = 0;

  virtual bool less(const basic_identifier &x) const = 0;
  virtual bool equal_to(const basic_identifier &x) const = 0;

  virtual size_t hash() const = 0;

  virtual bool is_same_type(const basic_identifier &x) const = 0;
  virtual const std::type_index &type_index() const = 0;

  virtual basic_identifier *clone() const = 0;
  virtual basic_identifier* share() = 0;
  virtual void isolate() = 0;
  virtual void share_with(basic_identifier &id) = 0;

  virtual std::ostream &print(std::ostream &out) const = 0;

  friend std::ostream &operator<<(std::ostream &os, const basic_identifier &x);

  virtual bool is_valid() const = 0;

  template<class T>
  T id() const;
};

template<class T>
T basic_identifier::id() const
{
  if (type_index() == std::type_index(typeid(identifier < T > ))) {
    return static_cast<const identifier <T> &>(*this).value();
  } else {
    throw std::logic_error("not the same type");
  }
}

}

#endif //OOS_BASIC_IDENTIFIER_HPP
