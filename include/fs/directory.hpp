#ifndef DIRECTORY_HPP
#define DIRECTORY_HPP

#include "types.hpp"
#include "element.hpp"

#include <string>
#include <vector>
#include <ostream>
#include <memory>

#include <dirent.h>

namespace oos {

class directory
{
public:
  directory();
  directory(const std::string &path);
  directory(const char *path);
  
  ~directory();

  bool operator==(const directory &x) const;
  bool operator==(const std::string &path) const;
  bool operator!=(const directory &x) const;
  bool operator!=(const std::string &path) const;

  directory& up(unsigned levels = 1);

  directory& down(const std::string &path);

  directory& root();

  directory& reset(const std::string &path);
  directory& reset(const char *path);

  directory& remove(bool recursive);

  std::string str();
  const std::string str() const;

  size_t size(element_type type = all_types) const;
  
  element_iterator begin();
  const_element_iterator begin() const;

  element_iterator end();
  const_element_iterator end() const;

  file_iterator file_begin();
  const_file_iterator file_begin() const;

  file_iterator file_end();
  const_file_iterator file_end() const;

  link_iterator link_begin();
  const_link_iterator link_begin() const;

  link_iterator link_end();
  const_link_iterator link_end() const;

  directory_iterator directory_begin();
  const_directory_iterator directory_begin() const;

  directory_iterator directory_end();
  const_directory_iterator directory_end() const;

private:
  void initialize(std::string path, bool clear = true);

private:
  typedef std::vector<std::string> string_vector_t;

  string_vector_t path_parts_;
};

directory mkdir(const std::string &path);

}

#endif /* DIRECTORY_HPP */
