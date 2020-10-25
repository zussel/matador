#ifndef MATADOR_STREAM_PROCESSOR_HPP
#define MATADOR_STREAM_PROCESSOR_HPP

#include <memory>

#include "matador/utils/memory.hpp"

namespace matador {

/// @cond MATADOR_DEV

namespace detail {

template<class T>
class stream_element_processor_iterator;

enum class stream_process_state : unsigned int
{
  INITIAL = 0,
  VALID = 1,
  INVALID = 2,
  FINISHED = 4
};

inline stream_process_state operator|(stream_process_state a, stream_process_state b)
{
  return static_cast<stream_process_state>(static_cast<unsigned int>(a) | static_cast<unsigned int>(b));
}

inline stream_process_state operator&(stream_process_state a, stream_process_state b)
{
  return static_cast<stream_process_state>(static_cast<unsigned int>(a) & static_cast<unsigned int>(b));
}

inline bool is_stream_process_state_set(stream_process_state source, stream_process_state needle)
{
  return static_cast<int>(source & needle) > 0;
}

template<class Out>
class stream_element_processor
{
public:

  typedef stream_element_processor_iterator<Out> iterator;
  typedef Out value_type;
  typedef std::shared_ptr<value_type> value_type_ptr;

  virtual ~stream_element_processor() = default;

  virtual value_type_ptr value() = 0;

  iterator begin()
  {
    return iterator(this, value(), process_impl());
  }

  iterator end()
  {
    return iterator(this, nullptr, stream_process_state::FINISHED);
  }

  stream_process_state process()
  {
    return process_impl();
  }

protected:
  virtual stream_process_state process_impl() = 0;
};

//template<class T>
//using processor_ptr = std::shared_ptr<stream_element_processor<T>>;

template<class T>
class stream_element_processor_iterator : public std::iterator<std::forward_iterator_tag, T>
{
public:
  typedef stream_element_processor_iterator<T> self;      /**< Shortcut for this class. */
  typedef T value_type;                                   /**< Shortcut for the value type. */
  typedef std::shared_ptr<T> value_type_ptr;              /**< Shortcut for shared pointer to value type. */

  stream_element_processor_iterator(stream_element_processor<T> *processor, value_type_ptr value,
                                    stream_process_state state)
    : processor_(std::move(processor)), value_(value), state_(state)
  {}

  bool operator==(const self &i) const
  {
    return (i.state_ == state_ && value_ == i.value_) ||
           (is_stream_process_state_set(state_, stream_process_state::FINISHED) &&
            is_stream_process_state_set(i.state_, stream_process_state::FINISHED));
  }

  bool operator!=(const self &i) const
  {
    return !this->operator==(i);
  }

  // pre inc
  self &operator++()
  {
    state_ = processor_->process();
    if (is_stream_process_state_set(state_, stream_process_state::VALID)) {
      value_ = processor_->value();
    }
    return *this;
  }

  // post inc
  self operator++(int)
  {
    self temp = self(processor_, value_, state_);
    state_ = processor_->process();
    if (is_stream_process_state_set(state_, stream_process_state::VALID)) {
      value_ = processor_->value();
    }
    return temp;
  }

  value_type_ptr operator->() const
  {
    return value_;
  }

  value_type operator*() const
  {
    return *value_;
  }

private:
  stream_element_processor<T> *processor_ = nullptr;
  value_type_ptr value_;
  stream_process_state state_ = stream_process_state::INITIAL;
};

template<class Out, typename Iterator>
class iterator_element_processor : public stream_element_processor<Out>
{
public:
  typedef stream_element_processor<Out> base;
  typedef typename base::value_type_ptr value_type_ptr;
  typedef Iterator iterator_type;

  iterator_element_processor(iterator_type begin, iterator_type end)
  : value_(begin), end_(end)
  {}

  value_type_ptr value() override
  {
    return std::make_shared<Out>(std::move(*value_));
  }

protected:
  stream_process_state process_impl() override
  {
    if (first_) {
      first_ = false;
      return valid_or_completed();
    }
    ++value_;
    return valid_or_completed();
  }

private:
  stream_process_state valid_or_completed() const
  {
    if (value_ == end_) {
      return stream_process_state::FINISHED;
    } else {
      return stream_process_state::VALID;
    }
  }

private:
  bool first_ = true;

  iterator_type value_;
  iterator_type end_;
};

template < class Out, class U, class Enable = void >
class range_element_processor;

template < class Out, class U >
class range_element_processor<Out, U, typename std::enable_if<
  std::is_arithmetic<Out>::value &&
  std::is_arithmetic<U>::value
>::type> : public stream_element_processor<Out>
{
public:
  typedef stream_element_processor<Out> base;
  typedef typename base::value_type value_type;
  typedef typename base::value_type_ptr value_type_ptr;

  range_element_processor(value_type &&from, value_type &&to, const U& increment)
    : to_(to), incr_(increment)
  {
    value_ = std::make_shared<value_type>(from);
  }

  value_type_ptr value() override
  {
    return value_;
  }

protected:
  stream_process_state process_impl() override
  {
    if (first_ && *value_ < to_) {
      first_ = false;
      return stream_process_state::VALID;
    } else if (!first_) {
      *value_ = *value_ + incr_;
      if (*value_ <= to_) {
        return stream_process_state::VALID;
      }
    }
    return stream_process_state::FINISHED;
  }

private:
  bool first_ = true;
  value_type to_;
  U incr_ = 1;
  value_type_ptr value_;
};

template < class Out, class U, class Enable = void >
class counter_element_processor;

template < class Out, class U >
class counter_element_processor<Out, U, typename std::enable_if<
  std::is_arithmetic<Out>::value &&
  std::is_arithmetic<U>::value
>::type> : public stream_element_processor<Out>
{
public:
  typedef stream_element_processor<Out> base;
  typedef typename base::value_type value_type;
  typedef typename base::value_type_ptr value_type_ptr;

  counter_element_processor(value_type &&from, const U& increment)
    : incr_(increment)
  {
    value_ = std::make_shared<value_type>(from);
  }

  value_type_ptr value() override
  {
    return value_;
  }

protected:
  stream_process_state process_impl() override
  {
    if (first_) {
      first_ = false;
      return stream_process_state::VALID;
    }
    *value_ = *value_ + incr_;
    return stream_process_state::VALID;
  }

private:
  bool first_ = true;
  U incr_ = 1;
  value_type_ptr value_;
};

template <class Out, typename Predicate>
class filter_element_processor : public stream_element_processor<Out>
{
public:
  typedef stream_element_processor<Out> base;
  typedef typename base::value_type_ptr value_type_ptr;

  filter_element_processor(std::shared_ptr<stream_element_processor<Out>> successor, Predicate &&pred)
    : successor_(std::move(successor)), pred_(pred)
  {}

  value_type_ptr value() override
  {
    return value_;
  }

protected:
  stream_process_state process_impl() override
  {
    while (is_stream_process_state_set(successor_->process(), stream_process_state::VALID)) {
      value_ = successor_->value();
      if (pred_(*value_)) {
        return stream_process_state::VALID;
      } else {
        value_ = nullptr;
      }
    }
    return stream_process_state::FINISHED;
  }

private:
  std::shared_ptr<stream_element_processor<Out>> successor_;
  Predicate pred_;
  value_type_ptr value_;
};

template <class Out, typename Predicate>
class peek_element_processor : public stream_element_processor<Out>
{
public:
  typedef stream_element_processor<Out> base;
  typedef typename base::value_type_ptr value_type_ptr;

  peek_element_processor(std::shared_ptr<stream_element_processor<Out>> successor, Predicate &&pred)
    : successor_(std::move(successor)), pred_(pred)
  {}

  value_type_ptr value() override
  {
    return value_;
  }

protected:
  stream_process_state process_impl() override
  {
    while (is_stream_process_state_set(successor_->process(), stream_process_state::VALID)) {
      value_ = successor_->value();
      pred_(*value_);
      return stream_process_state::VALID;
    }
    return stream_process_state::FINISHED;
  }

private:
  std::shared_ptr<stream_element_processor<Out>> successor_;
  Predicate pred_;
  value_type_ptr value_;
};

template<class Out>
class take_element_processor : public stream_element_processor<Out>
{
public:
  typedef stream_element_processor<Out> base;
  typedef typename base::value_type_ptr value_type_ptr;

  explicit take_element_processor(std::shared_ptr<stream_element_processor<Out>> successor, std::size_t count)
  : successor_(std::move(successor)), count_(count)
  {}

  value_type_ptr value() override
  {
    return value_;
  }

protected:
  stream_process_state process_impl() override
  {
    while (is_stream_process_state_set(successor_->process(), stream_process_state::VALID)) {
      if (current_count_++ < count_) {
        value_ = successor_->value();
        return stream_process_state::VALID;
      } else {
        break;
      }
    }
    return stream_process_state::FINISHED;
  }

private:
  std::shared_ptr<stream_element_processor<Out>> successor_;
  std::size_t count_ = 0;
  std::size_t current_count_ = 0;
  value_type_ptr value_;
};

template<class Out, typename Predicate>
class take_while_element_processor : public stream_element_processor<Out>
{
public:
  typedef stream_element_processor<Out> base;
  typedef typename base::value_type_ptr value_type_ptr;

  explicit take_while_element_processor(std::shared_ptr<stream_element_processor<Out>> successor, Predicate pred)
  : successor_(std::move(successor)), pred_(pred)
  {}

  value_type_ptr value() override
  {
    return value_;
  }

protected:
  stream_process_state process_impl() override
  {
    while (is_stream_process_state_set(successor_->process(), stream_process_state::VALID)) {
      value_ = successor_->value();
      if (!pred_(*value_)) {
        continue;
      } else {
        value_ = successor_->value();
        return stream_process_state::VALID;
      }
    }
    return stream_process_state::FINISHED;
  }

private:
  std::shared_ptr<stream_element_processor<Out>> successor_;
  Predicate pred_;
  value_type_ptr value_;
};

template<class Out>
class skip_element_processor : public stream_element_processor<Out>
{
public:
  typedef stream_element_processor<Out> base;
  typedef typename base::value_type_ptr value_type_ptr;

  explicit skip_element_processor(std::shared_ptr<stream_element_processor<Out>> successor, std::size_t count)
  : successor_(std::move(successor)), count_(count)
  {}

  value_type_ptr value() override
  {
    return value_;
  }

protected:
  stream_process_state process_impl() override
  {
    while (is_stream_process_state_set(successor_->process(), stream_process_state::VALID)) {
      if (current_count_++ < count_) {
        continue;
      } else {
        value_ = successor_->value();
        return stream_process_state::VALID;
      }
    }
    return stream_process_state::FINISHED;
  }

private:
  std::shared_ptr<stream_element_processor<Out>> successor_;
  std::size_t count_ = 0;
  std::size_t current_count_ = 0;
  value_type_ptr value_;
};

template<class Out, typename Predicate>
class skip_while_element_processor : public stream_element_processor<Out>
{
public:
  typedef stream_element_processor<Out> base;
  typedef typename base::value_type_ptr value_type_ptr;

  explicit skip_while_element_processor(std::shared_ptr<stream_element_processor<Out>> successor, Predicate pred)
  : successor_(std::move(successor)), pred_(pred)
  {}

  value_type_ptr value() override
  {
    return value_;
  }

protected:
  stream_process_state process_impl() override
  {
    while (is_stream_process_state_set(successor_->process(), stream_process_state::VALID)) {
      value_ = successor_->value();
      if (pred_(*value_)) {
        continue;
      } else {
        value_ = successor_->value();
        return stream_process_state::VALID;
      }
    }
    return stream_process_state::FINISHED;
  }

private:
  std::shared_ptr<stream_element_processor<Out>> successor_;
  Predicate pred_;
  value_type_ptr value_;
};

template<class In, class Out, typename Predicate>
class map_element_processor : public stream_element_processor<Out>
{
public:
  typedef stream_element_processor<Out> base;
  typedef typename base::value_type value_type;
  typedef typename base::value_type_ptr value_type_ptr;
  typedef In input_value_type;

  explicit map_element_processor(std::shared_ptr<stream_element_processor<In>> successor, Predicate &&pred)
  : successor_(std::move(successor)), pred_(pred)
  {}

  value_type_ptr value() override
  {
    return value_;
  }

protected:
  stream_process_state process_impl() override
  {
    std::shared_ptr<input_value_type> val;
    auto state = successor_->process();
    switch (state) {
      case stream_process_state::VALID:
        val = successor_->value();
        value_ = std::make_shared<value_type>(pred_(*val));
        return stream_process_state::VALID;
      case stream_process_state::INVALID:
      case stream_process_state::FINISHED:
      default:
        value_.reset();
        return state;
    }
  }

private:
  std::shared_ptr<stream_element_processor<In>> successor_;
  Predicate pred_;
  value_type_ptr value_;
};


template<class Out, typename Iterator>
std::shared_ptr<stream_element_processor<Out>> make_from(Iterator begin, Iterator end)
{
  return std::make_shared<iterator_element_processor<Out, Iterator>>(begin, end);
}

template < class Out, typename U >
std::shared_ptr<stream_element_processor<Out>> make_range(Out &&from, Out &&to, const U& increment)
{
  return std::make_shared<range_element_processor<Out, U>>(std::forward<Out>(from), std::forward<Out>(to), increment);
}

template < class Out, typename U >
std::shared_ptr<stream_element_processor<Out>> make_counter(Out &&from, const U& increment)
{
  return std::make_shared<counter_element_processor<Out, U>>(std::forward<Out>(from), increment);
}

template<class Out, typename Predicate, typename R = Out>
std::shared_ptr<stream_element_processor<R>> make_filter(Predicate &&pred, std::shared_ptr<stream_element_processor<Out>> successor)
{
  return std::make_shared<filter_element_processor<Out, Predicate>>(successor, std::forward<Predicate>(pred));
}

template<class Out, typename Predicate, typename R = Out>
std::shared_ptr<stream_element_processor<R>> make_peek(Predicate &&pred, std::shared_ptr<stream_element_processor<Out>> successor)
{
  return std::make_shared<peek_element_processor<Out, Predicate>>(successor, std::forward<Predicate>(pred));
}

template<class Out, typename R = Out>
std::shared_ptr<stream_element_processor<R>> make_take(std::size_t count, std::shared_ptr<stream_element_processor<Out>> successor)
{
  return std::make_shared<take_element_processor<Out>>(successor, count);
}

template<class Out, typename Predicate, typename R = Out>
std::shared_ptr<stream_element_processor<R>> make_take_while(Predicate pred, std::shared_ptr<stream_element_processor<Out>> successor)
{
  return std::make_shared<take_while_element_processor<Out, Predicate>>(successor, pred);
}

template<class Out, typename R = Out>
std::shared_ptr<stream_element_processor<R>> make_skip(std::size_t count, std::shared_ptr<stream_element_processor<Out>> successor)
{
  return std::make_shared<skip_element_processor<Out>>(successor, count);
}

template<class Out, typename Predicate, typename R = Out>
std::shared_ptr<stream_element_processor<R>> make_skip_while(Predicate pred, std::shared_ptr<stream_element_processor<Out>> successor)
{
  return std::make_shared<skip_while_element_processor<Out, Predicate>>(successor, pred);
}

template<class In, typename Predicate, typename Out = typename std::result_of<Predicate &(In)>::type>
std::shared_ptr<stream_element_processor<Out>> make_mapper(Predicate &&pred, std::shared_ptr<stream_element_processor<In>> successor)
{
  return std::make_shared<map_element_processor<In, Out, Predicate>>(successor, std::forward<Predicate>(pred));
}

}

/// @endcond

}
#endif //MATADOR_STREAM_PROCESSOR_HPP
