#include <matador/utils/version.hpp>

namespace matador {

version::version(unsigned int major, unsigned int minor, unsigned int patch)
: major_(major)
, minor_(minor)
, patch_(patch)
{}

bool version::operator==(const version &x) const
{
  return major_ == x.major_ &&
         minor_ == x.minor_ &&
         patch_ == x.patch_;
}

bool version::operator!=(const version &x) const
{
  return !(*this == x);
}

bool version::operator>(const version &x) const
{
  return !(*this <= x);
}

bool version::operator>=(const version &x) const
{
  return !(*this < x);
}

bool version::operator<(const version &x) const
{
  return (major_ < x.major_) ||
         (major_ == x.major_ && minor_ < x.minor_) ||
         (major_ == x.major_ && minor_ == x.minor_ && patch_ < x.patch_);
}

bool version::operator<=(const version &x) const
{
  return *this < x || *this == x;
}

std::string version::str() const
{
  char buf[32];
  sprintf(buf, "%d.%d.%d", major_, minor_, patch_);
  return buf;
}

std::ostream &operator<<(std::ostream &out, const version &v)
{
  out << v.str();
  return out;
}

version version::from_string(const std::string &version_string)
{
  version result;
  const auto ret = sscanf(version_string.c_str(), "%u.%u.%u", &result.major_, &result.minor_, &result.patch_);
  if (ret != 3) {
    throw std::logic_error("invalid version string");
  }

  return result;
}

unsigned int version::major() const
{
  return major_;
}

unsigned int version::minor() const
{
  return minor_;
}

unsigned int version::patch() const
{
  return patch_;
}

void version::major(unsigned int m)
{
  major_ = m;
}

void version::minor(unsigned int m)
{
  minor_ = m;
}

void version::patch(unsigned int p)
{
  patch_ = p;
}

}