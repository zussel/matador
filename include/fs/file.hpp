#ifndef FILE_HPP
#define FILE_HPP

#include <string>

namespace oos {

class file
{
public:
  file();
  explicit file(const std::string &filepath);

  ~file();

  bool operator==(const file &x) const;
  bool operator!=(const file &x) const;

  void reset(const char *filepath);

  std::string name() const;
  
private:
  std::string filepath_;
};

}

#endif /* FILE_HPP */
