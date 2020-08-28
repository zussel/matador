#ifndef MATADOR_STREAM_PROCESSOR_HPP
#define MATADOR_STREAM_PROCESSOR_HPP

#include <memory>

namespace matador {

template < class T >
class stream_element_processor_iterator;

template < class Out  >
class stream_element_processor
{
public:
  typedef stream_element_processor_iterator<Out> iterator;

  virtual ~stream_element_processor() = default;
  virtual std::shared_ptr<Out> value() = 0;
  bool process()
  {
    return process_impl();
  }

  iterator begin()
  {
    process_impl();
    return iterator(this, value());
  }
  iterator end()
  {
    return iterator(nullptr, value());
  }

protected:
  virtual bool process_impl() = 0;
};

template < class T >
using processor_ptr = std::shared_ptr<stream_element_processor<T>>;

template < class T >
class stream_element_processor_iterator : public std::iterator<std::forward_iterator_tag, T>
{
public:
  typedef stream_element_processor_iterator<T> self;      /**< Shortcut for this class. */
  typedef T value_type;                                   /**< Shortcut for the value type. */
  typedef std::shared_ptr<T> value_type_ptr;              /**< Shortcut for the value type pointer. */
  typedef value_type* pointer;                            /**< Shortcut for the pointer type. */
  typedef value_type& reference;                          /**< Shortcut for the reference type */

  explicit stream_element_processor_iterator(stream_element_processor<T> *processor, value_type_ptr value)
    : processor_(std::move(processor))
    , value_(value)
  {}

  bool operator==(const self &i) const
  {
    return (*value_ == *i.value_);
  }
  bool operator!=(const self &i) const
  {
    return !this->operator==(i);
  }

  // pre inc
  self& operator++()
  {
    if (processor_->process()) {
      value_ = processor_->value();
    }
    return *this;
  }
  // post inc
  self operator++(int)
  {
    self temp = self(processor_, value_);
    if (processor_->process()) {
      value_ = processor_->value();
    }
    return temp;
  }

  T* operator->() const
  {
    return value_.get();
  }

  T& operator*() const
  {
    return *value_;
  }

private:
  stream_element_processor<T> *processor_ = nullptr;
  std::shared_ptr<T> value_;
};

template < class Out, typename Iterator >
class iterator_element_processor : public stream_element_processor<Out>
{
public:
  typedef Iterator iterator_type;

  iterator_element_processor(iterator_type begin, iterator_type end)
  : value_(begin), end_(end)
  {}

  std::shared_ptr<Out> value() override
  {
    return std::make_shared<Out>(*value_);
  }

protected:
  bool process_impl() override
  {
    if (first_) {
      first_ = false;
      return value_ != end_;
    }
    ++value_;
    return value_ != end_;
  }

private:
  bool first_ = true;
  iterator_type value_;
  iterator_type end_;
};

template < class Out, typename Predicate >
class filter_element_processor : public stream_element_processor<Out>
{
public:
  explicit filter_element_processor(std::shared_ptr<stream_element_processor<Out>> successor, Predicate pred)
  : successor_(std::move(successor))
  , pred_(pred)
  {}

  std::shared_ptr<Out> value() override
  {
    return value_;
  }

protected:
  bool process_impl() override
  {
    while (successor_->process()) {
      value_ = successor_->value();
      if (pred_(*value_)) {
        return true;
      }
      value_.reset();
    }
    return false;
  }

private:
  std::shared_ptr<stream_element_processor<Out>> successor_;
  Predicate pred_;
  std::shared_ptr<Out> value_;
};

template < class In, class Out, typename Predicate >
class map_element_processor : public stream_element_processor<Out>
{
public:
  explicit map_element_processor(std::shared_ptr<stream_element_processor<In>> successor, Predicate pred)
  : successor_(std::move(successor))
  , pred_(pred)
  {}

  std::shared_ptr<Out> value() override
  {
    return value_;
  }

protected:
  bool process_impl() override
  {
    if (successor_->process()) {
      std::shared_ptr<In> val = successor_->value();
      value_ = std::make_shared<Out>(pred_(*val));
      return true;
    }
    value_.reset();
    return false;
  }

private:
  std::shared_ptr<stream_element_processor<In>> successor_;
  Predicate pred_;
  std::shared_ptr<Out> value_;
};


template < class Out, typename Iterator >
std::shared_ptr<stream_element_processor<Out>> make_range(Iterator begin, Iterator end)
{
  return std::make_shared<iterator_element_processor<Out, Iterator>>(begin, end);
}

template < class Out, typename Predicate, typename R = Out>
std::shared_ptr<stream_element_processor<R>> make_filter(Predicate pred, std::shared_ptr<stream_element_processor<Out>> successor)
{
  return std::make_shared<filter_element_processor<Out, Predicate>>(successor, pred);
}

template < class In, typename Predicate, typename Out = typename std::result_of<Predicate&(In)>::type>
std::shared_ptr<stream_element_processor<Out>> make_mapper(Predicate pred, std::shared_ptr<stream_element_processor<In>> successor)
{
  return std::make_shared<map_element_processor<In, Out, Predicate>>(successor, pred);
}


}
#endif //MATADOR_STREAM_PROCESSOR_HPP
