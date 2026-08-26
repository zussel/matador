#ifndef MATADOR_VERSION_HPP
#define MATADOR_VERSION_HPP

#include "matador_export.h"

#include "matador/utils/result.hpp"
#include "matador/utils/error.hpp"


#include <string>

namespace matador::utils {

class MATADOR_EXPORT version final {
public:
  version() = default;
  ~version() = default;
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

  [[nodiscard]] std::string str() const;
  friend std::ostream& operator<<(std::ostream &out, const version &v);

  static result<version, error> from_string(const std::string &version_string);

  [[nodiscard]] unsigned int major() const;
  [[nodiscard]] unsigned int minor() const;
  [[nodiscard]] unsigned int patch() const;

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
