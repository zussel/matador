#ifndef OOS_ANY_VISITOR_HPP
#define OOS_ANY_VISITOR_HPP

#include "tools/any.hpp"

#include <unordered_map>
#include <functional>
#include <iostream>
#include <vector>
#include <typeindex>

namespace oos {

class any_visitor
{
public:
  using function = std::function<void(oos::any&)>;

  template <typename T>
  void register_visitor(std::function<void(T&)> f) {
    fs.insert(std::make_pair(
      std::type_index(typeid(T)),
      function([&f](oos::any & x) {
        std::cout << "val " << x._<T>() << "\n";
        f(x._<T>());
      })
    ));
  }

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
