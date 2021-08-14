#ifndef MATADOR_STREAM_HPP
#define MATADOR_STREAM_HPP

#include "matador/utils/optional.hpp"
#include "matador/utils/stream_processor.hpp"

#include <list>
#include <vector>
#include <algorithm>
#include <memory>

#ifdef _WIN32
#undef min
#undef max
#endif

namespace matador {

/**
 * The stream class allows stream like
 * data processing with lazy evaluation.
 *
 * It provides a couple of stream element
 * processors like "filter", "map" or "skip".
 *
 * There are also some stream terminators allowing
 * to get a single result based on the evaluated stream
 * like "min", "max" or "any".
 *
 * Once a steam is completely processed it can't be
 * processed again.
 *
 * @tparam T Type of the stream elements
 */
template < class T >
class stream
{
public:
  typedef detail::stream_element_processor_iterator<T> iterator; /**< Shortcut to stream iterator type */

  /**
   * Default constructor
   */
  stream() = default;

  /**
   * Copys the steam from given stream x
   *
   * @param x Stream to copy from
   */
  stream(const stream &x) = default;

  /**
   * Copy move stream from given stream x
   *
   * @param x Stream to copy move from
   */
  stream(stream &&x) noexcept = default;

  /**
   * Copy assigns stream from given stream x
   *
   * @param x Stream to copy assign from
   * @return The copy assigned stream
   */
  stream& operator=(const stream &x) = default;

  /**
   * Assign move stream from given stream x
   *
   * @param x Stream to assign move from
   * @return The assign moved stream
   */
  stream& operator=(stream &&x) noexcept = default;

  /**
   * Initialize a new stream with the given stream processor
   *
   * @param processor The initial stream processor
   */
  explicit stream(std::shared_ptr<detail::stream_element_processor<T>> processor);

  /**
   * Creates a new stream by assigning a given stream processor
   *
   * @param processor Stream processor to assign from
   * @return The assigned stream
   */
  stream& operator=(const std::shared_ptr<detail::stream_element_processor<T>> &processor);

  /**
   * Returns the begin iterator of the stream
   *
   * @return Begin iterator of the stream
   */
  iterator begin();

  /**
   * Returns the end iterator of the stream
   *
   * @return End iterator of the stream
   */
  iterator end();

  /**
   * Take only count elements of the data stream.
   * Than the stream id finished.
   *
   * @param count Number of elements to be taken
   * @return The stream enhanced with the take processor.
   */
  stream<T>& take(std::size_t count);

  /**
   * Take only elements of the stream
   * as long as the given predicate if true
   *
   * @tparam Predicate Type of the predicate
   * @param pred Condition to check
   * @return The enhanced stream
   */
  template < typename Predicate >
  stream<T>& take_while(Predicate pred);

  /**
   * Skip the first number elements of the
   * stream defined by the given count.
   *
   * @param count Number of  elements to skip
   * @return The enhanced stream
   */
  stream<T>& skip(std::size_t count);

  /**
   * Skip elements of the stream while given
   * predicate is true. Once the predicate gets false
   * the capacity elements are valid.
   *
   * @tparam Predicate Type of the predicate
   * @param pred Condition to check
   * @return The enhanced stream
   */
  template < typename Predicate >
  stream<T>& skip_while(Predicate pred);

  /**
   * Returns every nth elements of the stream
   * identified by the given count
   *
   * @param count Identifies which elements are valid
   * @return The enhanced stream
   */
  stream<T>& every(std::size_t count);

  /**
   * Filters the stream by the given predicate. Only
   * if the predicate called with the current element evaluates
   * to true the element is valid
   *
   * @tparam Predicate Type of the predicate
   * @param pred Condition to check
   * @return The enhanced stream
   */
  template < typename Predicate >
  stream<T>& filter(Predicate &&pred);

  /**
   * Maps each element of stream to new element creating
   * a new stream of elements. This stream could be of the same
   * type as the origin stream or of a new type.
   *
   * @tparam Predicate Type of the predicate
   * @tparam R Return type of the predicate and type of the new stream
   * @param pred Predicate to apply on every stream element
   * @return New stream created of the result of given predicate
   */
  template < typename Predicate, typename R = typename std::result_of<Predicate&(T)>::type>
  stream<R> map(Predicate &&pred);

  /**
   * Flattens a nested container (list, vectors, i.e.)
   * of an element type into one new stream containing
   * all elements of the container
   *
   * @tparam Predicate Type of the predicate
   * @tparam R Return type of the predicate and type of the new stream
   * @param pred Predicate to apply on every stream element
   * @return New stream created of the result of given predicate
   */
  template < typename Predicate, typename R = typename std::result_of<Predicate&(T)>::type::value_type>
  stream<R> flatmap(Predicate &&pred);

  /**
   * Peeks each valid element with the given predicate. This
   * processor can be used to check or debug each stream element.
   *
   * @tparam Predicate Type of the predicate
   * @param pred Condition to check
   * @return The enhanced stream
   */
  template < typename Predicate >
  stream<T>& peek(Predicate &&pred);

  /**
   * Concatenates the current stream with the
   * given stream of the same type. Once the current
   * stream finishes the elements of the next stream are
   * processed seamlessly.
   *
   * @param other The stream to concatenate
   * @return The enhanced stream
   */
  stream<T>& concat(const stream<T> &other);

  /**
   * Pack every n elemets of the stream into a container (vector)
   * and create a ne wstream of it.
   *
   * @param packsize Number of elements to pack
   * @return Stream of packed elements
   */
  stream<std::vector<T>> pack_every(std::size_t packsize);

  /*
   * Termination methods
   */

  /**
   * Return the first element of the current processed
   * stream. The element is returned as an optional because
   * it can be not available.
   *
   * @return An optional possibly containing the first element.
   */
  optional<T> first();

  /**
   * Return the last element of the current processed
   * stream. The element is returned as an optional because
   * it can be not available.
   *
   * @return An optional possibly containing the last element.
   */
  optional<T> last();

  /**
   * Returns the minimum of the stream. This value is calculated
   * using the std::less algorithm. The return value is an optional
   * because the stream could be empty.
   *
   * @return An optional possibly containing the minimum value.
   */
  optional<T> min();

  /**
   * Returns the maximum of the stream. This value is calculated
   * using the std::less algorithm. The return value is an optional
   * because the stream could be empty.
   *
   * @return An optional possibly containing the maximum value.
   */
  optional<T> max();

  /**
   * Returns an element at the given position index. The value is returned
   * as an optional because the requested position could be invalid
   *
   * @param index Position of the requested element
   * @return An optional possibly containing the value.
   */
  optional<T> at(std::size_t index);

  /**
   * Returns true if any element of the stream matches the
   * given predicate.
   *
   * @tparam Predicate Type of the predicate
   * @param pred Condition to check
   * @return True if any element matches
   */
  template < typename Predicate >
  bool any(Predicate &&pred);

  /**
   * Returns true if all elements of the stream matches the
   * given predicate.
   *
   * @tparam Predicate Type of the predicate
   * @param pred Condition to check
   * @return True if all elements matches
   */
  template < typename Predicate >
  bool all(Predicate &&pred);

  /**
   * Returns true if none of the elements of the
   * stream matches the given predicate.
   *
   * @tparam Predicate Type of the predicate
   * @param pred Condition to check
   * @return True if none element matches
   */
  template < typename Predicate >
  bool none(Predicate &&pred);

  /**
   * Returns the number of elements in the stream
   *
   * @return The number of elements
   */
  std::size_t count();

  /**
   * Returns the number of elements matching the
   * given predicate.
   *
   * @tparam Predicate Type of the predicate
   * @param pred Condition to check
   * @return Number of matching elements
   */
  template < typename Predicate >
  std::size_t count(Predicate &&pred);

  /**
   * All elements of the stream are reduced to one element
   * using the given accumulator function. This function takes
   * the current element and the current accumulated value
   * and returns the new accumulated value.
   * The returned value is an optional because the stream could
   * be empty and thus the result.
   *
   * @tparam Accumulator Type of the accumulator
   * @param accu The accumulator function to be applied
   * @return An optional possibly containing the accumulated value.
   */
  template < typename Accumulator >
  optional<T> reduce(Accumulator &&accu);

  /**
   * All elements of the stream are reduced to one element
   * using the given accumulator function. This function takes
   * the current element and the current accumulated value
   * and returns the new accumulated value. The initial value for
   * this reduction is the given identity value. If the stream is empty,
   * at least this value is returned.
   *
   * @tparam U Type of the return and identity value
   * @tparam Accumulator Type of the accumulator function
   * @param identity Initial value
   * @param accu The accumulator function to be applied
   * @return The reduced value, at least the given identity value
   */
  template < typename U, typename Accumulator >
  U reduce(const U &identity, Accumulator &&accu);

  /**
   * All elements of the stream are reduced to one element
   * using the given accumulator function. This function takes
   * the current element and the current accumulated value
   * and returns the new accumulated value.
   * To create the initial value the given identity function is
   * used. The returned value is an optional because the stream could
   * be empty and thus the result.
   *
   * @tparam U Type of the identity function
   * @tparam Accumulator Type of the accumulator function
   * @tparam R Type of the returned value
   * @param identity_fun Function to create the initial value
   * @param accu The accumulator function to be applied
   * @return An optional possibly containing the accumulated value.
   */
  template < typename U, typename Accumulator, typename R = typename std::result_of<U&(T)>::type >
  optional<R> reduce_idfunc(const U &identity_fun, Accumulator &&accu);

  /**
   * Prints all elements of the stream to the given std::ostream. The elements are
   * separated by the given delimiter string. The default delimiter is a blank.
   *
   * @param out The ostream to write to
   * @param delim The delimiter for the elements
   */
  void print_to(std::ostream &out, const char *delim = " ");

  /**
   * The collect method collects all elements into new
   * container of type C. The containers template
   * parameters are the type T and an allocator type Allocator.
   * The default for the allocator is std::allocator<T>
   *
   * @tparam C Type of the container
   * @tparam Allocator Allocator type
   * @return The container containing all elements
   */
  template < template < class ... > class C, class Allocator = std::allocator<T>>
  C<T, Allocator> collect();

  /**
   * Iterates over all stream elements and
   * applies the given predicate to each element.
   *
   * @tparam Predicate Type of predicate
   * @param pred Function to apply
   */
  template < typename Predicate >
  void for_each(Predicate &&pred);

  /**
   * Returns the current sentinel processor of the stream
   * This processor represents the last stream processor
   * and contains its successor to ensure a valid
   * processing chain for each element.
   *
   * @return The sentinel stream element processor
   */
  std::shared_ptr<detail::stream_element_processor<T>>& processor()
  {
    return processor_;
  }

  std::string join();
  std::string join(const std::string &delimiter);
  std::string join(const std::string &delimiter, const std::string &prefix, const std::string &suffix);

private:

  std::shared_ptr<detail::stream_element_processor<T>> processor_;
};

/**
 * Creates a new stream of the given container.
 *
 * @tparam T Type of the stream elements
 * @tparam C Type of the container
 * @tparam Allocator Type of the allocator
 * @param container Container to create the stream from
 * @return The created stream
 */
template < class T, template < class ... > class C, class Allocator = std::allocator<T> >
stream<T> make_stream(C<T, Allocator> &&container);

//template < class T, template < class ... > class C >
//stream<T> make_stream(const C<T> &container);
//

/**
 * Creates a stream of given begin an end iterator.
 *
 * @tparam T Type of the iterator
 * @param from begin iterator
 * @param to end iterator
 * @return The created stream
 */
template < typename T >
stream<T> make_stream(T &&from, T &&to);

/**
 * Creates a stream fro an initializer list of elements
 *
 * @tparam T Type of elements
 * @param elems Elements to create the stream from
 * @return The created stream
 */
template < typename T >
stream<T> make_stream(std::initializer_list<T> elems);

/**
 * Creates a continuing stream source of type T.
 * Value of T is increased with the ++ operator so
 * T must provide this operator.
 *
 * @tparam T Type of elements
 * @param from Starting value of elements
 * @return The created stream
 */
template < typename T >
stream<T> make_stream_counter(T &&from);

/**
 * Creates a stream range given limits from and
 * to and an incrementing value. Once the to value
 * is reached or exceeded the stream is finished.
 *
 * @tparam T Type of elements
 * @tparam U Type of increment value
 * @param from Starting value of stream
 * @param to Closing value of stream
 * @param increment Incrementing value
 * @return The created stream
 */
template < typename T, typename U >
stream<T> make_stream(T &&from, T &&to, const U& increment);

/**
 * Creates a continuing stream source of type T beginning
 * with from and incremented by the given incrementing value.
 *
 * @tparam T Type of elements
 * @tparam U Type of increment value
 * @param from Starting value of elements
 * @param increment Incrementing value
 * @return The created stream
 */
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

/// @cond MATADOR_DEV

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

template < class T >
std::string join(stream<T> &s)
{
  std::string result;

  std::for_each(s.begin(), s.end(), [&result](T &&val) {
    result.append(std::forward<T>(val));
  });

  return result;
}

template < class T >
std::string join(stream<T> &s, const std::string &delimiter, const std::string &prefix, const std::string &suffix)
{
  std::string result(prefix);

  auto first = s.begin();
  auto last = s.end();
  result.append(*first);
  ++first;
  if (first != last) {
    std::for_each(first, last, [&result, &delimiter](T &&val) {
      result.append(delimiter);
      result.append(std::forward<T>(val));
    });
  }

  result.append(suffix);
  return result;
}

/// @endcond

template<class T>
template<template<class ...> class C, class Allocator>
C<T, Allocator> stream<T>::collect()
{
  return matador::collect<T, C>(*this);
}

template<class T>
std::string stream<T>::join()
{
  return matador::join<T>(*this);
}

template<class T>
std::string stream<T>::join(const std::string &delimiter)
{
  return matador::join<T>(*this, delimiter, "", "");
}

template<class T>
std::string stream<T>::join(const std::string &delimiter, const std::string &prefix, const std::string &suffix)
{
  return matador::join<T>(*this, delimiter, prefix, suffix);
}

template<class T>
template<typename Predicate>
void stream<T>::for_each(Predicate &&pred)
{
  std::for_each(begin(), end(), pred);
}

template<class T>
optional<T> stream<T>::first()
{
  auto first = begin();
  if (first != end()) {
    return make_optional(*first);
  } else {
    return optional<T>();
  }
}

template<class T>
optional<T> stream<T>::last()
{
  return reduce([](const T &, const T &next) { return next; });
}

template<class T>
optional<T> stream<T>::min()
{
  std::less<T> less_func;
  return reduce([=](const T &x, const T &next) {
    return less_func(x, next) ? x : next;
  });
}

template<class T>
optional<T> stream<T>::max()
{
  std::greater<T> greater_func;
  return reduce([=](const T &x, const T &next) {
    return greater_func(x, next) ? x : next;
  });
}

template<class T>
optional<T> stream<T>::at(std::size_t index)
{
  return skip(index).first();
}

template<class T>
template<typename Predicate>
bool stream<T>::any(Predicate &&pred)
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

template<class T>
template<typename Predicate>
bool stream<T>::all(Predicate &&pred)
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

template<class T>
template<typename Predicate>
bool stream<T>::none(Predicate &&pred)
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

template<class T>
std::size_t stream<T>::count()
{
  return std::distance(begin(), end());
}

template<class T>
template<typename Predicate>
std::size_t stream<T>::count(Predicate &&pred)
{
  return filter(std::forward<Predicate>(pred)).count();
}

template<class T>
template<typename Accumulator>
optional<T> stream<T>::reduce(Accumulator &&accu)
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

template<class T>
template<typename U, typename Accumulator>
U stream<T>::reduce(const U &identity, Accumulator &&accu)
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

template<class T>
template<typename U, typename Accumulator, typename R>
optional<R> stream<T>::reduce_idfunc(const U &identity_fun, Accumulator &&accu)
{
  auto first = begin();
  auto last = end();
  if (first == last) {
    return optional<R>();
  }

  auto result = identity_fun(*first);
  while (++first != last) {
    result = accu(result, *first);
  }
  return result;
}

template<class T>
void stream<T>::print_to(std::ostream &out, const char *delim)
{
  std::for_each(begin(), end(), [&out, delim](const T &val) {
    out << val << delim;
  });
}

template < class T, template < class ... > class C, class Allocator >
stream<T> make_stream(C<T, Allocator> &&container)
{
  return stream<T>(detail::make_from<T>(std::forward<C<T, Allocator>>(container)));
}

template < class T, template < class ... > class C, class Allocator >
stream<T> make_stream(const C<T, Allocator> &container)
{
  return stream<T>(detail::make_from<T>(container.begin(), container.end()));
//  return stream<T>(detail::make_from<T>(std::begin(container), std::end(container)));
}

template < typename T >
stream<T> make_stream(T &&from, T &&to)
{
  return stream<T>(detail::make_range<T>(std::forward<T>(from), std::forward<T>(to), 1));
}

template < class T, template < class U = T > class C >
stream<T> make_stream(C<T> &&container)
{
  return stream<T>(detail::make_from<T>(std::forward<C<T>>(container)));
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
