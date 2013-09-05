#ifndef ELEMENT_HPP
#define ELEMENT_HPP

#include "types.hpp"
#include "file.hpp"
#include "link.hpp"

#include <iterator>
#include <memory>

#include <dirent.h>

namespace oos {

class directory;

class element
{
public:
  element();
  explicit element(const std::string &path);
  
  ~element();

  bool operator==(const element &x) const;
  bool operator!=(const element &x) const;

  void reset(const std::string &path);

  element_type type() const;
  
  bool is_directory() const;
  bool is_file() const;
  bool is_link() const;
  bool is_type(element_type type) const;

  std::string name() const;

  file to_file();
  
  link to_link();

  directory to_directory();

private:
  std::string path_;
};

template < class T > class const_element_iterator_base;

template < class T >
class element_iterator_base : public std::iterator<std::forward_iterator_tag, T>
{
public:
  typedef element_iterator_base<T> self;	/**< Shortcut for this iterator type. */
  typedef T value_type;                 /**< Shortcut for the value type. */
  typedef value_type* pointer;          /**< Shortcut for the pointer type. */
  typedef value_type& reference;              /**< Shortcut for the reference to the value type. */

  element_iterator_base()
  {}

  element_iterator_base(const std::string &path)
    : path_(path)
    , elem_(create())
  {
    dir_.reset(opendir(path.c_str()), std::bind(&element_iterator_base::close, this, std::placeholders::_1));
    if (dir_ == 0) {
      throw std::logic_error("couldn't open directory");
    }
  }

  bool operator==(const element_iterator_base &x) const
  {
    if (elem_ && x.elem_) {
      return *elem_ == *x.elem_;
    } else {
      return elem_ == x.elem_;
    }
  }

  bool operator!=(const element_iterator_base &x) const
  {
    return !this->operator==(x);
  }

  /**
   * Pre increments the iterator
   * 
   * @return Returns iterators successor.
   */
  self& operator++()
  {
    increment();
    return *this;
  }

  /**
   * Post increments the iterator
   * 
   * @return Returns iterator before incrementing.
   */
  self operator++(int)
  {
    self tmp = *this;
    increment();
    return tmp;
  }

  /**
   * Returns the pointer to the node.
   * 
   * @return The pointer to the node.
   */
  pointer operator->()
  {
    return elem_.get();
  }

  /**
   * Returns the pointer to the node.
   * 
   * @return The pointer to the node.
   */
  pointer operator->() const
  {
    return elem_.get();
  }

  /**
   * Returns the node.
   * 
   * @return The iterators underlaying node.
   */
  const reference operator*() const
  {
    return *elem_;
  }

private:
  void increment()
  {
    dirent *elem = readdir(dir_.get());
    if (elem) {
      elem_->reset(path_ + elem->d_name);
    } else {
      elem_.reset();
    }
  }

  void close(DIR *dir)
  {
    closedir(dir);
  }

  T* create() const
  {
    return new element;
  }

private:
  friend class const_element_iterator_base<T>;

  std::shared_ptr<DIR> dir_;
  std::string path_;
  std::shared_ptr<value_type> elem_;
};

template <> file* element_iterator_base<file>::create() const;
template <> link* element_iterator_base<link>::create() const;
template <> directory* element_iterator_base<directory>::create() const;

template <> void element_iterator_base<file>::increment();
template <> void element_iterator_base<link>::increment();
template <> void element_iterator_base<directory>::increment();

typedef element_iterator_base<element>    element_iterator;
typedef element_iterator_base<file>       file_iterator;
typedef element_iterator_base<directory>  directory_iterator;
typedef element_iterator_base<link>       link_iterator;

template < class T >
class const_element_iterator_base : public std::iterator<std::forward_iterator_tag, T, std::ptrdiff_t, const T*, const T&>
{
public:
  typedef const_element_iterator_base<T> self;	/**< Shortcut for this iterator type. */
  typedef T value_type;                         /**< Shortcut for the value type. */
  typedef const value_type* pointer;            /**< Shortcut for the pointer type. */
  typedef value_type& reference;                /**< Shortcut for the reference to the value type. */

  const_element_iterator_base()
  {}

  const_element_iterator_base(const std::string &path)
    : path_(path)
    , elem_(create())
  {
    dir_.reset(opendir(path.c_str()), std::bind(&const_element_iterator_base::close, this, std::placeholders::_1));
    if (dir_ == 0) {
      throw std::logic_error("couldn't open directory");
    }
  }

  const_element_iterator_base(const element_iterator_base<T> &x)
    : dir_(x.dir_)
    , path_(x.path_)
    , elem_(x.elem_)
  {}

  bool operator==(const_element_iterator_base &x) const
  {
    if (elem_ && x.elem_) {
      return *elem_ == *x.elem_;
    } else {
      return elem_ == x.elem_;
    }
  }

  bool operator!=(const_element_iterator_base &x) const
  {
    return !this->operator==(x);
  }

  /**
   * Pre increments the iterator
   * 
   * @return Returns iterators successor.
   */
  self& operator++()
  {
    increment();
    return *this;
  }

  /**
   * Post increments the iterator
   * 
   * @return Returns iterator before incrementing.
   */
  self operator++(int)
  {
    self tmp = *this;
    increment();
    return tmp;
  }

  /**
   * Returns the pointer to the node.
   * 
   * @return The pointer to the node.
   */
  pointer operator->() const
  {
    return &elem_;
  }

  /**
   * Returns the node.
   * 
   * @return The iterators underlaying node.
   */
  const value_type operator*() const
  {
    return *elem_;
  }

private:
  void increment()
  {
    dirent *elem = readdir(dir_.get());
    if (elem) {
      elem_->reset(path_ + elem->d_name);
    } else {
      elem_.reset();
    }
  }

  void close(DIR *dir)
  {
    closedir(dir);
  }

  T* create() const
  {
    return new element;
  }

private:
  std::shared_ptr<DIR> dir_;
  std::string path_;
  std::shared_ptr<value_type> elem_;
};

template <> file* const_element_iterator_base<file>::create() const;
template <> link* const_element_iterator_base<link>::create() const;
template <> directory* const_element_iterator_base<directory>::create() const;

template <> void const_element_iterator_base<file>::increment();
template <> void const_element_iterator_base<link>::increment();
template <> void const_element_iterator_base<directory>::increment();

typedef const_element_iterator_base<element>    const_element_iterator;
typedef const_element_iterator_base<file>       const_file_iterator;
typedef const_element_iterator_base<directory>  const_directory_iterator;
typedef const_element_iterator_base<link>       const_link_iterator;

}

#endif /* ELEMENT_HPP */
