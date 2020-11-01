#ifndef MATADOR_STREAM_HPP
#define MATADOR_STREAM_HPP

#include "matador/utils/optional.hpp"
#include "matador/utils/stream_processor.hpp"

#include <list>
#include <vector>
#include <algorithm>
#include <memory>

namespace matador {

template < class T >
class stream
{
public:
  typedef detail::stream_element_processor_iterator<T> iterator;

  stream() = default;
  stream(const stream &x) = default;
  stream(stream &&x) noexcept = default;
  stream& operator=(const stream &x) = default;
  stream& operator=(stream &&x) noexcept = default;

  explicit stream(std::shared_ptr<detail::stream_element_processor<T>> processor);

  stream& operator=(const std::shared_ptr<detail::stream_element_processor<T>> &processor);

  iterator begin();

  iterator end();

  stream<T>& take(std::size_t count);

  template < typename Predicate >
  stream<T>& take_while(Predicate pred);

  stream<T>& skip(std::size_t count);

  template < typename Predicate >
  stream<T>& skip_while(Predicate pred);

  stream<T>& every(std::size_t count);

  template < typename Predicate >
  stream<T>& filter(Predicate &&pred);

  template < typename Predicate, typename R = typename std::result_of<Predicate&(T)>::type>
  stream<R> map(Predicate &&pred);

  template < typename Predicate, typename R = typename std::result_of<Predicate&(T)>::type::value_type>
  stream<R> flatmap(Predicate &&pred);

  template < typename Predicate >
  stream<T>& peek(Predicate &&pred);

  stream<T>& concat(const stream<T> &other);

  stream<std::vector<T>> pack_every(std::size_t packsize);

  /*
   * Termination methods
   */
  optional<T> first()
  {
    auto first = begin();
    if (first != end()) {
      return make_optional(*first);
    } else {
      return optional<T>();
    }
  }

  optional<T> last()
  {
    return reduce([](const T &, const T &next) { return next; });
  }

  optional<T> min()
  {
    std::less<T> less_func;
    return reduce([=](const T &x, const T &next) {
      return less_func(x, next) ? x : next;
    });
  }

  optional<T> max()
  {
    std::greater<T> greater_func;
    return reduce([=](const T &x, const T &next) {
      return greater_func(x, next) ? x : next;
    });
  }

  optional<T> at(std::size_t index)
  {
    return skip(index).first();
  }

  template < typename Predicate >
  bool any(Predicate &&pred)
  {
    bool result = false;

    for (const T &val : *this) {
      if (pred(val)) {
        result = true;
        break;
      }
    }
    return result;
  }

  template < typename Predicate >
  bool all(Predicate &&pred)
  {
    bool result = true;

    for (const T &val : *this) {
      if (!pred(val)) {
        result = false;
        break;
      }
    }
    return result;
  }

  template < typename Predicate >
  bool none(Predicate &&pred)
  {
    bool result = true;

    for (const T &val : *this) {
      if (pred(val)) {
        result = false;
        break;
      }
    }
    return result;
  }

  std::size_t count()
  {
    return std::distance(begin(), end());
  }

  template < typename Predicate >
  std::size_t count(Predicate &&pred)
  {
    return filter(std::forward<Predicate>(pred)).count();
  }

  template < typename Accumulator >
  optional<T> reduce(Accumulator &&accu)
  {
    auto first = begin();
    auto last = end();
    if (first == last) {
      return optional<T>();
    }

    auto result = *first;
    ++first;
    while (first != last) {
      result = accu(result, *first++);
    }
    return result;
  }

  template < typename U, typename Accumulator >
  U reduce(const U &identity, Accumulator &&accu)
  {
    auto first = begin();
    auto last = end();
    auto result = identity;
    while (first != last) {
      result = accu(result, *first);
      ++first;
    }
    return result;
  }

  template < typename U, typename Accumulator, typename R = typename std::result_of<U&(T)>::type >
  R reduce_idfunc(const U &identity_fun, Accumulator &&accu)
  {
    auto first = begin();
    auto last = end();
    auto result = identity_fun(*first);
    while (++first != last) {
      result = accu(result, *first);
    }
    return result;
  }

  void print_to(std::ostream &out, const char *delim = " ")
  {
    std::for_each(begin(), end(), [&out, delim](const T &val) {
      out << val << delim;
    });
  }

  template < template < class ... > class C, class Allocator = std::allocator<T>>
  C<T, Allocator> collect();

  std::shared_ptr<detail::stream_element_processor<T>>& processor()
  {
    return processor_;
  }

private:

  std::shared_ptr<detail::stream_element_processor<T>> processor_;
};

template < class T, template < class ... > class C, class Allocator = std::allocator<T> >
stream<T> make_stream(C<T, Allocator> &&container);

//template < class T, template < class ... > class C >
//stream<T> make_stream(const C<T> &container);
//
template < typename T >
stream<T> make_stream(T &&from, T &&to);

template < typename T >
stream<T> make_stream(std::initializer_list<T> elems);

template < typename T >
stream<T> make_stream_counter(T &&from);

template < typename T, typename U >
stream<T> make_stream(T &&from, T &&to, const U& increment);

template < typename T, typename U >
stream<T> make_stream_counter(T &&from, const U& increment);

template<class T>
stream<T>::stream(std::shared_ptr <detail::stream_element_processor<T>> processor)
  : processor_(std::move(processor))
{}

template<class T>
stream <T> &stream<T>::operator=(const std::shared_ptr <detail::stream_element_processor<T>> &processor)
{
  processor_ = processor;
  return *this;
}

template<class T>
typename stream<T>::iterator stream<T>::begin()
{
  return processor_->begin();
}

template<class T>
typename stream<T>::iterator stream<T>::end()
{
  return processor_->end();
}

template<class T>
stream <T> &stream<T>::take(std::size_t count)
{
  processor_ = make_take(count, processor_);
  return *this;
}

template<class T>
template<typename Predicate>
stream <T> &stream<T>::take_while(Predicate pred)
{
  processor_ = make_take_while(pred, processor_);
  return *this;
}

template<class T>
stream <T> &stream<T>::skip(std::size_t count)
{
  processor_ = make_skip(count, processor_);
  return *this;
}

template<class T>
template<typename Predicate>
stream <T> &stream<T>::skip_while(Predicate pred)
{
  processor_ = make_skip_while(pred, processor_);
  return *this;
}

template<class T>
stream <T> &stream<T>::every(std::size_t count)
{
  processor_ = make_every(count, processor_);
  return *this;
}

template<class T>
template<typename Predicate>
stream <T> &stream<T>::filter(Predicate &&pred)
{
  processor_ = make_filter(std::forward<Predicate>(pred), processor_);
  return *this;
}

template<class T>
template<typename Predicate, typename R>
stream<R> stream<T>::map(Predicate &&pred)
{
  return stream<R>(make_mapper(std::forward<Predicate>(pred), processor_));
}

template<class T>
template<typename Predicate, typename R>
stream<R> stream<T>::flatmap(Predicate &&pred)
{
  return stream<R>(make_flatmap(std::forward<Predicate>(pred), processor_));
}

template<class T>
template<typename Predicate>
stream <T> &stream<T>::peek(Predicate &&pred)
{
  processor_ = make_peek(std::forward<Predicate>(pred), processor_);
  return *this;
}

template<class T>
stream <T> &stream<T>::concat(const stream <T> &other)
{
  processor_ = make_concat(processor_, other.processor_);
  return *this;
}

template<class T>
stream<std::vector<T>> stream<T>::pack_every(std::size_t packsize)
{
  return stream<std::vector<T>>(make_pack_every(processor_, packsize));
}

template<class T, template<class ...> class C, class Allocator = std::allocator <T> >
C<T, Allocator> collect(stream <T> &s)
{
  using container_type = C<T, Allocator>;
  container_type result;

  std::for_each(s.begin(), s.end(), [&result](T &&val) {
    result.push_back(std::forward<T>(val));
  });

  return result;
}

template<class T>
template<template<class ...> class C, class Allocator>
C<T, Allocator> stream<T>::collect()
{
  return matador::collect<T, C>(*this);
}

template < class T, template < class ... > class C, class Allocator >
stream<T> make_stream(C<T, Allocator> &&container)
{
  return stream<T>(detail::make_from<T>(std::forward<C<T, Allocator>>(container)));
}

//template < class T, template < class ... > class C >
//stream<T> make_stream(C<T> container)
//{
//  return stream<T>(detail::make_from<T>(std::begin(container), std::end(container)));
//}

template < typename T >
stream<T> make_stream(T &&from, T &&to)
{
  return stream<T>(detail::make_range<T>(std::forward<T>(from), std::forward<T>(to), 1));
}

template < typename T >
stream<T> make_stream(std::initializer_list<T> elems)
{
  return stream<T>(detail::make_from<T>(std::begin(elems), std::end(elems)));
}

template < typename T >
stream<T> make_stream_counter(T &&from)
{
  return stream<T>(detail::make_counter<T>(std::forward<T>(from), 1));
}

template < typename T, typename U >
stream<T> make_stream(T &&from, T &&to, const U& increment)
{
  return stream<T>(detail::make_range<T>(std::forward<T>(from), std::forward<T>(to), increment));
}

template < typename T, typename U >
stream<T> make_stream_counter(T &&from, const U& increment)
{
  return stream<T>(detail::make_counter<T>(std::forward<T>(from), increment));
}

}

#include "matador/utils/stream_processor.tpp"

#endif //MATADOR_STREAM_HPP
