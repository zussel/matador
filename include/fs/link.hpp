#ifndef LINK_HPP
#define LINK_HPP

#include <string>

namespace oos {

class link
{
public:
  link();
  explicit link(const std::string &linkpath);

  ~link();

  bool operator==(const link &x) const;
  bool operator!=(const link &x) const;

  void reset(const char *linkpath);

  std::string name() const;
  
private:
  std::string linkpath_;
};

}

#endif /* LINK_HPP */
