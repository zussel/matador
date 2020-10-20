#ifndef MATADOR_STREAM_HPP
#define MATADOR_STREAM_HPP

#include "matador/utils/stream_processor.hpp"

#include <list>
#include <vector>

namespace matador {

template < class T >
class stream
{
public:
  typedef stream_element_processor_iterator<T> iterator;

  explicit stream(std::shared_ptr<stream_element_processor<T>> processor)
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

  stream<T>& skip(size_t val)
  {
    return *this;
  }

  template < typename Predicate >
  stream<T>& filter(Predicate pred)
  {
    processor_ = make_filter(pred, processor_);
    return *this;
  }

  template < typename Predicate, typename R = typename std::result_of<Predicate&(T)>::type>
  stream<R> map(Predicate pred)
  {
    return stream<R>(make_mapper(pred, processor_));
  }

  std::vector<T> to_vector()
  {
    std::vector<T> result;

    auto first = begin();
    auto last = end();
    while (first != last) {
      result.push_back(*first++);
    }

    return result;
  }

  std::list<T> to_list()
  {
    return std::list<T>();
  }

private:
  std::shared_ptr<stream_element_processor<T>> processor_;
};

template < class T, template < class ... > class C >
stream<T> make_stream(C<T> &&container);

template < class T, template < class ... > class C >
stream<T> make_stream(const C<T> &container);

template < class T >
stream<T> make_stream(std::list<T> &&container)
{
  return stream<T>();
}

template < class T >
stream<T> make_stream(std::list<T> &container)
{
  return stream<T>(make_range<T>(std::begin(container), std::end(container)));
}

}
#endif //MATADOR_STREAM_HPP
