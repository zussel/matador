//
// Created by sascha on 2/23/16.
//

#ifndef OOS_FIELD_HPP
#define OOS_FIELD_HPP

namespace oos {

class field
{
public:
  field(const char *name)
    :name_(name)
  {}
  field(const std::string &name)
    :name_(name)
  {}

  std::string name() const { return name_; }

private:
  std::string name_;
};

}
#endif //OOS_FIELD_HPP
