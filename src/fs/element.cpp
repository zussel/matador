#include "fs/element.hpp"
#include "fs/directory.hpp"

#include <sys/stat.h>
#include <cstring>
#include <cerrno>

#include <iostream>
#include <sstream>

namespace oos {

void throw_error(int ret, const char *what, int success)
{
  if (ret == success) {
    return;
  }
  std::stringstream msg;
  msg << what << " (" << errno << "): " << strerror(errno);
}

element::element()
{}

element::element(const std::string &path)
  : path_(path)
{}

element::~element()
{}

bool element::operator==(const element &x) const
{
  return path_ == x.path_;
}

bool element::operator!=(const element &x) const
{
  return !this->operator==(x);
}


void element::reset(const std::string &path)
{
  path_ = path;
}

element_type element::type() const
{
  if (!path_.empty()) {
    struct stat buffer;
    throw_error(lstat(path_.c_str(), &buffer), "lstat", 0);
    std::cout << "type for: " << path_ << "\n";
    if (S_ISREG(buffer.st_mode)) {
      return file_type;
    } else if (S_ISDIR(buffer.st_mode)) {
      return directory_type;
    } else if (S_ISLNK(buffer.st_mode)) {
      return link_type;
    } else {
      std::cout << "unknown type (" << buffer.st_mode << ")\n";
      return unknown_type;
    }
  } else {
    return unknown_type;
  }
}

bool element::is_directory() const
{
  return is_type(directory_type);
}

bool element::is_file() const
{
  return is_type(file_type);
}

bool element::is_link() const
{
  return is_type(link_type);
}

bool element::is_type(element_type type) const
{
  return this->type() == type;
}

std::string element::name() const
{
  return path_;
}

file element::to_file()
{
  if (!path_.empty()) {
    return file(path_);
  } else {
    return file();
  }
}

link element::to_link()
{
  if (!path_.empty()) {
    return link(path_);
  } else {
    return link();
  }
}

directory element::to_directory()
{
  if (!path_.empty()) {
    return directory(path_);
  } else {
    return directory();
  }
}

template <> file* element_iterator_base<file>::create() const
{
  return new file;
}

template <> link* element_iterator_base<link>::create() const
{
  return new link;
}

template <> directory* element_iterator_base<directory>::create() const
{
  return new directory;
}

template <> file* const_element_iterator_base<file>::create() const
{
  return new file;
}

template <> link* const_element_iterator_base<link>::create() const
{
  return new link;
}

template <> directory* const_element_iterator_base<directory>::create() const
{
  return new directory;
}

template <>
void element_iterator_base<file>::increment()
{
  dirent *elem = 0;
  struct stat buffer;
  while ((elem = readdir(dir_.get())) != 0) {
    std::string f(path_ + elem->d_name);
    throw_error(lstat(f.c_str(), &buffer), "lstat", 0);
//    std::cout << "name: " << elem->d_name << "(mode: " << buffer.st_mode << ")\n";
    if (S_ISREG(buffer.st_mode)) {
      elem_->reset(f.c_str());
      return;
    }
  }
  elem_.reset();
}

template <>
void const_element_iterator_base<file>::increment()
{
  dirent *elem = 0;
  struct stat buffer;
  while ((elem = readdir(dir_.get())) != 0) {
    std::string f(path_ + elem->d_name);
    throw_error(lstat(f.c_str(), &buffer), "lstat", 0);
    if (S_ISREG(buffer.st_mode)) {
      elem_->reset(f.c_str());
      return;
    }
  }
  elem_.reset();
}

template <>
void element_iterator_base<link>::increment()
{
  dirent *elem = 0;
  struct stat buffer;
  while ((elem = readdir(dir_.get())) != 0) {
    std::string f(path_ + elem->d_name);
    throw_error(lstat(f.c_str(), &buffer), "lstat", 0);
//    std::cout << "name: " << elem->d_name << "(mode: " << buffer.st_mode << ")\n";
    if (S_ISLNK(buffer.st_mode)) {
      elem_->reset(f.c_str());
      return;
    }
  }
  elem_.reset();
}

template <>
void const_element_iterator_base<link>::increment()
{
  dirent *elem = 0;
  struct stat buffer;
  while ((elem = readdir(dir_.get())) != 0) {
    std::string f(path_ + elem->d_name);
    throw_error(lstat(f.c_str(), &buffer), "lstat", 0);
    if (S_ISLNK(buffer.st_mode)) {
      elem_->reset(f.c_str());
      return;
    }
  }
  elem_.reset();
}

template <>
void element_iterator_base<directory>::increment()
{
  dirent *elem = 0;
  struct stat buffer;
  while ((elem = readdir(dir_.get())) != 0) {
    std::string f(path_ + elem->d_name);
    throw_error(lstat(f.c_str(), &buffer), "lstat", 0);
//    std::cout << "name: " << elem->d_name << "(mode: " << buffer.st_mode << ")\n";
    if (S_ISDIR(buffer.st_mode)) {
      elem_->reset(f);
      return;
    }
  }
  elem_.reset();
}

template <>
void const_element_iterator_base<directory>::increment()
{
  dirent *elem = 0;
  struct stat buffer;
  while ((elem = readdir(dir_.get())) != 0) {
    std::string f(path_ + elem->d_name);
    throw_error(lstat(f.c_str(), &buffer), "lstat", 0);
    if (S_ISDIR(buffer.st_mode)) {
      elem_->reset(f);
      return;
    }
  }
  elem_.reset();
}

}
