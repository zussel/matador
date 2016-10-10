#ifndef OOS_ANY_VISITOR_HPP
#define OOS_ANY_VISITOR_HPP

#include "tools/any.hpp"

#include <unordered_map>
#include <functional>
#include <iostream>
#include <vector>
#include <typeindex>

namespace oos {

/**
 * @brief An any visitor class
 *
 * This class can be used to implement the visitor
 * pattern for the any type class. Register function
 * for as many types you need. Then call visit to
 * apply the visitor pattern.
 */
class any_visitor
{
public:
  using function = std::function<void(oos::any&)>; /**< Shortcut for the visitor callback function */

  /**
   * @brief Register a callback function for a concrete type
   *
   * @tparam T Type of the argument passed into the function
   * @param f The callback function
   */
  template <typename T>
  void register_visitor(const std::function<void(T&)> &f) {
    fs.insert(std::make_pair(
      std::type_index(typeid(T)),
      function([f](oos::any & x) {
        f(x._<T>());
      })
    ));
  }

  /**
   * @brief Applies the visitor pattern on an any object.
   * @param x The any object the pattern should be applied on
   * @return Returns true if a function could be applied.
   */
  bool visit(oos::any & x) {
    auto it = fs.find(x.type_index());
    if (it != fs.end()) {
      it->second(x);
      return true;
    } else {
      return false;
    }
  }

private:
  std::unordered_map<std::type_index, function> fs;
};

}

#endif //OOS_ANY_VISITOR_HPP
