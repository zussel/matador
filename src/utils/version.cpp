#include <matador/utils/version.hpp>

#include <matador/utils/errors.hpp>

#include <limits>

namespace matador::utils {
namespace {

bool parse_uint_component(const std::string &text, std::size_t &pos, unsigned int &value) {
  if (pos >= text.size() || text[pos] < '0' || text[pos] > '9') {
    return false;
  }

  unsigned int result{};

  while (pos < text.size() && text[pos] >= '0' && text[pos] <= '9') {
    const auto digit = static_cast<unsigned int>(text[pos] - '0');

    if (result > (std::numeric_limits<unsigned int>::max() - digit) / 10) {
      return false;
    }

    result = result * 10 + digit;
    ++pos;
  }

  value = result;
  return true;
}

bool parse_dot(const std::string &text, std::size_t &pos) {
  if (pos >= text.size() || text[pos] != '.') {
    return false;
  }

  ++pos;
  return true;
}

} // namespace
version::version(const unsigned int major, const unsigned int minor, const unsigned int patch)
: major_(major)
, minor_(minor)
, patch_(patch) {
}

bool version::operator==(const version& x) const {
  return major_ == x.major_ &&
    minor_ == x.minor_ &&
    patch_ == x.patch_;
}

bool version::operator!=(const version& x) const {
  return !(*this == x);
}

bool version::operator>(const version& x) const {
  return !(*this <= x);
}

bool version::operator>=(const version& x) const {
  return !(*this < x);
}

bool version::operator<(const version& x) const {
  return (major_ < x.major_) ||
    (major_ == x.major_ && minor_ < x.minor_) ||
    (major_ == x.major_ && minor_ == x.minor_ && patch_ < x.patch_);
}

bool version::operator<=(const version& x) const {
  return *this < x || *this == x;
}

std::string version::str() const {
  return std::to_string(major_) + "." +
    std::to_string(minor_) + "." +
    std::to_string(patch_);
}

std::ostream& operator<<(std::ostream& out, const version& v) {
  out << v.str();
  return out;
}

result<version, error> version::from_string(const std::string& version_string) {
  unsigned int major{};
  unsigned int minor{};
  unsigned int patch{};

  if (std::size_t pos{}; !parse_uint_component(version_string, pos, major) ||
      !parse_dot(version_string, pos) ||
      !parse_uint_component(version_string, pos, minor) ||
      !parse_dot(version_string, pos) ||
      !parse_uint_component(version_string, pos, patch) ||
      pos != version_string.size()) {
    return failure(error(utils_error::InvalidVersionString, version_string));
      }

  return ok(version{major, minor, patch});}

unsigned int version::major() const {
  return major_;
}

unsigned int version::minor() const {
  return minor_;
}

unsigned int version::patch() const {
  return patch_;
}

void version::major(unsigned int m) {
  major_ = m;
}

void version::minor(unsigned int m) {
  minor_ = m;
}

void version::patch(unsigned int p) {
  patch_ = p;
}
}
