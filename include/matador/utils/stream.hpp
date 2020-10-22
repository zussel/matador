#ifndef MATADOR_STREAM_HPP
#define MATADOR_STREAM_HPP

#include "matador/utils/stream_processor.hpp"
#include "matador/utils/optional.hpp"

#include <list>
#include <vector>
#include <algorithm>

namespace matador {

template < class T >
class stream
{
public:
  typedef detail::stream_element_processor_iterator<T> iterator;

  explicit stream(std::shared_ptr<detail::stream_element_processor<T>> processor)
    : processor_(std::move(processor))
  {}

  iterator begin()
  {
    return processor_->begin();
  }

  iterator end()
  {
    return processor_->end();
  }

  stream<T>& take(std::size_t count)
  {
    processor_ = make_take(count, processor_);
    return *this;
  }

  template < typename Predicate >
  stream<T>& take_while(Predicate pred)
  {
    processor_ = make_take_while(pred, processor_);
    return *this;
  }

  stream<T>& skip(std::size_t count)
  {
    processor_ = make_skip(count, processor_);
    return *this;
  }

  template < typename Predicate >
  stream<T>& skip_while(Predicate pred)
  {
    processor_ = make_skip_while(pred, processor_);
    return *this;
  }

  template < typename Predicate >
  stream<T>& filter(Predicate &&pred)
  {
    processor_ = make_filter(std::forward<Predicate>(pred), processor_);
    return *this;
  }

  template < typename Predicate, typename R = typename std::result_of<Predicate&(T)>::type>
  stream<R> map(Predicate &&pred)
  {
    return stream<R>(make_mapper(std::forward<Predicate>(pred), processor_));
  }

  template < typename Predicate >
  stream<T>& peek(Predicate &&pred)
  {
    processor_ = make_peek(std::forward<Predicate>(pred), processor_);
    return *this;
  }

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

  template < typename Accumulator >
  optional<T> reduce(Accumulator &&accu)
  {
    auto first = begin();
    auto last = end();
    if (first == last) {
      return optional<T>();
    }

    auto result = *first++;
    while (first != last) {
      result = accu(result, *first++);
    }
    return result;
  }

//  template < typename U, typename Accumulator, typename R >
//  R reduce(const U &identity, Accumulator &&accu);

  template < typename U, typename Accumulator, typename R = U >
  R reduce(const U &identity, Accumulator &&accu, typename std::enable_if<!std::is_function<U>::value>::type* = 0)
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
  R reduce(const U &identity_fun, Accumulator &&accu, typename std::enable_if<std::is_function<U>::value>::type* = 0)
  {
    auto first = begin();
    auto last = end();
    auto result = *first;
    while (first != last) {
      result = accu(result, *first);
      ++first;
    }
    return result;
  }

  void print_to(std::ostream &out, const char *delim = " ")
  {
    std::for_each(begin(), end(), [&out, delim](const T &val) {
      out << val << delim;
    });
  }

  std::vector<T> to_vector()
  {
    std::vector<T> result;

    std::for_each(begin(), end(), [&result](const T &val) {
      result.push_back(val);
    });

    return result;
  }

  std::list<T> to_list()
  {
    return std::list<T>();
  }

private:
  std::shared_ptr<detail::stream_element_processor<T>> processor_;
};

template < class T, template < class ... > class C >
stream<T> make_stream(C<T> &&container)
{
  return stream<T>(detail::make_from<T>(std::begin(container), std::end(container)));
}

template < class T, template < class ... > class C >
stream<T> make_stream(const C<T> &container)
{
  return stream<T>(detail::make_from<T>(std::begin(container), std::end(container)));
}

template < typename T >
stream<T> make_stream(T &&from, T &&to)
{
  return stream<T>(detail::make_range<T>(std::forward<T>(from), std::forward<T>(to), 1));
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
#endif //MATADOR_STREAM_HPP
