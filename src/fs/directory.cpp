#include "fs/directory.hpp"

#include <stdexcept>
#include <sstream>
#include <iterator>

#include <unistd.h>

namespace oos {

size_t split(const std::string &str, char delim, std::vector<std::string> &values)
{
  std::stringstream ss(str);
  std::string item;
  while (std::getline(ss, item, delim)) {
    values.push_back(item);
  }
  return values.size();
}

directory::directory()
{
  char path[2048];
  if (!getcwd(path, 2048)) {
    throw std::logic_error("couldn't get current directory path");
  }
  initialize(path);
}

directory::directory(const std::string &path)
{
  initialize(path);
}

directory::directory(const char *path)
{
  initialize(path);
}

directory::~directory()
{
}

bool directory::operator==(const directory &x) const
{
  return str() == x.str();
}

bool directory::operator!=(const directory &x) const
{
  return !this->operator==(x);
}

directory& directory::up(unsigned levels)
{
  while (!path_parts_.empty() && levels--) {
    path_parts_.pop_back();
  }
  return *this;
}

directory& directory::down(const std::string &path)
{
  if (!path.empty()) {
    if (path[0] == '/') {
      throw std::logic_error("path is in absolute format [" + path + "]");
    }
    initialize(path, false);
  }
  return *this;
}

directory& directory::root()
{
  path_parts_.clear();
  path_parts_.push_back("");
  return *this;
}

directory& directory::reset(const std::string &path)
{
  initialize(path);
  return *this;
}

directory& directory::reset(const char *path)
{
  initialize(path);
  return *this;
}

std::string directory::str()
{
  std::ostringstream p;
  std::ostream_iterator<std::string> i(p, "/");
  std::copy(path_parts_.begin(), path_parts_.end(), i);
  return p.str();
}

const std::string directory::str() const
{
  std::ostringstream p;
  std::ostream_iterator<std::string> i(p, "/");
  std::copy(path_parts_.begin(), path_parts_.end(), i);
  return p.str();
}

size_t directory::size(element_type type) const
{
  switch (type) {
    case file_type:
      return std::distance(file_begin(), file_end());
    case link_type:
      return std::distance(link_begin(), link_end());
    case all_types:
    default:
      return std::distance(begin(), end());
  }
}

element_iterator directory::begin() {
  return ++element_iterator(str());
}

const_element_iterator directory::begin() const {
  return ++const_element_iterator(str());
}

element_iterator directory::end() {
  return element_iterator();
}

const_element_iterator directory::end() const {
  return const_element_iterator();
}

file_iterator directory::file_begin()
{
  return ++file_iterator(str());
}

const_file_iterator directory::file_begin() const
{
  return ++const_file_iterator(str());
}

file_iterator directory::file_end()
{
  return file_iterator();
}

const_file_iterator directory::file_end() const
{
  return const_file_iterator();
}

link_iterator directory::link_begin()
{
  return ++link_iterator(str());
}

const_link_iterator directory::link_begin() const
{
  return ++const_link_iterator(str());
}

link_iterator directory::link_end()
{
  return link_iterator();
}

const_link_iterator directory::link_end() const
{
  return const_link_iterator();
}

directory_iterator directory::directory_begin()
{
  return ++directory_iterator(str());
}

const_directory_iterator directory::directory_begin() const
{
  return ++const_directory_iterator(str());
}

directory_iterator directory::directory_end()
{
  return directory_iterator();
}

const_directory_iterator directory::directory_end() const
{
  return const_directory_iterator();
}

void directory::initialize(const std::string &path, bool clear)
{
  if (clear) {
    path_parts_.clear();
  }
  // cut path into parts
  split(path, '/', path_parts_);
}

}
