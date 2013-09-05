#include "fs/directory.hpp"

#include <stdexcept>
#include <sstream>
#include <iterator>

#include <iostream>

#include <cstring>
#include <unistd.h>
#include <sys/stat.h>

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
  initialize("");
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

bool directory::operator==(const std::string &path) const
{
  return str() == path;
}

bool directory::operator!=(const directory &x) const
{
  return !this->operator==(x);
}

bool directory::operator!=(const std::string &path) const
{
  return !this->operator==(path);
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

directory& directory::remove(bool recursive)
{
  if (recursive) {
    // remove all links and files
    // call remove for all sub directories
    element_iterator first = begin();
    element_iterator last = end();
    while (first != last) {
      element elem = *first++;
      if (elem.is_directory()) {
        elem.to_directory().remove(recursive);
      } else if (elem.is_file() || elem.is_link()) {
        ::unlink((str() + elem.name()).c_str());
      }
    }
    return *this;
  } else if (0 == ::rmdir(str().c_str())) {
    path_parts_.pop_back();
    return *this;
  } else {
    std::stringstream msg;
    msg << "rmdir error (" << errno << "): " << strerror(errno);
    throw std::logic_error(msg.str());
  }
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

void directory::initialize(std::string path, bool clear)
{
  if (clear) {
    path_parts_.clear();
  }
  if (path.empty() || path[0] != '/') {
    // given path is relative
    // get absolute path
    char p[2048];
    if (!getcwd(p, 2048)) {
      throw std::logic_error("couldn't get current directory path");
    }
    path.insert(0, "/");
    path.insert(0, p);
  }
  // cut path into parts
  split(path, '/', path_parts_);
}

directory mkdir(const std::string &path)
{
  if (-1 == ::mkdir(path.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH)) {
    std::stringstream msg;
    msg << "mkdir error (" << errno << "): " << strerror(errno);
    throw std::logic_error(msg.str());
  }
  return directory(path);
}

}
