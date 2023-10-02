#ifndef MATADOR_VERSION_HPP
#define MATADOR_VERSION_HPP

#include <string>
#include <ostream>

namespace matador {

class version {
public:
  version() = default;
  ~version() =default;
  version(unsigned int major, unsigned int minor, unsigned int patch);
  version(const version& x) = default;
  version& operator=(const version& x) = default;
  version(version&& x) noexcept = default;
  version& operator=(version&& x) noexcept = default;

  bool operator==(const version &x) const;
  bool operator!=(const version &x) const;
  bool operator>(const version &x) const;
  bool operator>=(const version &x) const;
  bool operator<(const version &x) const;
  bool operator<=(const version &x) const;

  std::string str() const;
  friend std::ostream& operator<<(std::ostream &out, const version &v);

  static version from_string(const std::string &version_string);

  unsigned int major() const;
  unsigned int minor() const;
  unsigned int patch() const;

  void major(unsigned int m);
  void minor(unsigned int m);
  void patch(unsigned int p);

private:
  unsigned int major_{};
  unsigned int minor_{};
  unsigned int patch_{};
};

}

#endif //MATADOR_VERSION_HPP
