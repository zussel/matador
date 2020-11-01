#ifndef MATADOR_STREAM_PROCESSOR_HPP
#define MATADOR_STREAM_PROCESSOR_HPP

#include "matador/utils/memory.hpp"
#include "matador/utils/stream.hpp"

#include <memory>
#include <list>
#include <vector>

namespace matador {

/// @cond MATADOR_DEV

template < class T >
class stream;

namespace detail {

template<class T>
class stream_element_processor_iterator;

enum class stream_process_state : unsigned int
{
  INITIAL = 0,
  VALID = 1,
  FINISHED = 2
};

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
    if (!process_impl()) {
      return iterator(this, std::shared_ptr<Out>(nullptr), stream_process_state::FINISHED);
    } else {
      return iterator(this, value(), stream_process_state::VALID);
    }
  }

  iterator end()
  {
    return iterator(this, nullptr, stream_process_state::FINISHED);
  }

  bool process()
  {
    return process_impl();
  }

protected:
  virtual bool process_impl() = 0;
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
           (state_ == stream_process_state::FINISHED &&
            i.state_ ==  stream_process_state::FINISHED);
  }

  bool operator!=(const self &i) const
  {
    return !this->operator==(i);
  }

  // pre inc
  self &operator++()
  {
    if (processor_->process()) {
      state_ = stream_process_state::VALID;
      value_ = processor_->value();
    } else {
      state_ = stream_process_state::FINISHED;
    }
    return *this;
  }

  // post inc
  self operator++(int)
  {
    self temp = self(processor_, value_, state_);
    if (processor_->process()) {
      state_ = stream_process_state::VALID;
      value_ = processor_->value();
    } else {
      state_ = stream_process_state::FINISHED;
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

template < class Out, typename Container >
class container_element_processor : public stream_element_processor<Out>
{
private:
  using iterator_type = decltype(std::begin(std::declval<Container>()));

public:
  typedef stream_element_processor<Out> base;
  typedef typename base::value_type_ptr value_type_ptr;

  explicit container_element_processor(Container &&container)
    : container_{container}
    , value_{std::begin(container_)}
    , end_{std::end(container_)}
  {}

  value_type_ptr value() override
  {
    return std::make_shared<Out>(std::move(*value_));
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

  Container container_;
  iterator_type value_;
  iterator_type end_;
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
    : to_(to), current_(from), incr_(increment)
  {}

  value_type_ptr value() override
  {
    return std::make_shared<value_type>(current_);
  }

protected:
  bool process_impl() override
  {
    if (first_ && current_ < to_) {
      first_ = false;
      return true;
    } else if (!first_) {

      current_ += incr_;
      if (current_ <= to_) {
        return true;
      }
    }
    return false;
  }

private:
  bool first_ = true;
  value_type to_;
  value_type current_;
  U incr_ = 1;
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
    : incr_(increment), current_(from)
  {}

  value_type_ptr value() override
  {
    return std::make_shared<Out>(current_);
  }

protected:
  bool process_impl() override
  {
    if (first_) {
      first_ = false;
      return true;
    }
    current_ += incr_;
    return true;
  }

private:
  bool first_ = true;
  U incr_ = 1;
  value_type current_;
};

template < class Out >
class concat_element_processor : public stream_element_processor<Out>
{
public:
  typedef stream_element_processor<Out> base;
  typedef typename base::value_type_ptr value_type_ptr;

  concat_element_processor(std::shared_ptr<stream_element_processor<Out>> successor, std::shared_ptr<stream_element_processor<Out>> next)
  {
    processors_.push_back(std::move(successor));
    processors_.push_back(std::move(next));
  }

  value_type_ptr value() override
  {
    return value_;
  }

protected:
  bool process_impl() override
  {
    while (!processors_.empty()) {
      if (processors_.front()->process()) {
        value_ = processors_.front()->value();
        return true;
      } else {
        processors_.pop_front();
        continue;
      }
    }
    value_.reset();
    return false;
  }

private:
  std::list<std::shared_ptr<stream_element_processor<Out>>> processors_;
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
  bool process_impl() override
  {
    while (successor_->process()) {
      value_ = successor_->value();
      if (pred_(*value_)) {
        return true;
      } else {
        value_ = nullptr;
      }
    }
    return false;
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
  bool process_impl() override
  {
    while (successor_->process()) {
      value_ = successor_->value();
      pred_(*value_);
      return true;
    }
    return false;
  }

private:
  std::shared_ptr<stream_element_processor<Out>> successor_;
  Predicate pred_;
  value_type_ptr value_;
};

template<class Out>
class every_element_processor : public stream_element_processor<Out>
{
public:
  typedef stream_element_processor<Out> base;
  typedef typename base::value_type_ptr value_type_ptr;

  explicit every_element_processor(std::shared_ptr<stream_element_processor<Out>> successor, std::size_t every)
  : successor_(std::move(successor)), every_(every)
  {}

  value_type_ptr value() override
  {
    return value_;
  }

protected:
  bool process_impl() override
  {
    while (successor_->process()) {
      if (++current_count_ % every_ == 0) {
        value_ = successor_->value();
        return true;
      } else {
        value_ = nullptr;
      }
    }
    return false;
  }

private:
  std::shared_ptr<stream_element_processor<Out>> successor_;
  std::size_t every_ = 0;
  std::size_t current_count_ = 0;
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
  bool process_impl() override
  {
    while (successor_->process()) {
      if (current_count_++ < count_) {
        value_ = successor_->value();
        return true;
      } else {
        break;
      }
    }
    return false;
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
  bool process_impl() override
  {
    while (successor_->process()) {
      value_ = successor_->value();
      if (!pred_(*value_)) {
        continue;
      } else {
        value_ = successor_->value();
        return true;
      }
    }
    return false;
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
  bool process_impl() override
  {
    while (successor_->process()) {
      if (current_count_++ < count_) {
        continue;
      } else {
        value_ = successor_->value();
        return true;
      }
    }
    return false;
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
  bool process_impl() override
  {
    while (successor_->process()) {
      value_ = successor_->value();
      if (pred_(*value_)) {
        continue;
      } else {
        value_ = successor_->value();
        return true;
      }
    }
    return false;
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
  bool process_impl() override
  {
    std::shared_ptr<input_value_type> val;
    if (successor_->process()) {
      val = successor_->value();
      value_ = std::make_shared<value_type>(pred_(*val));
      return true;
    }
    value_.reset();
    return false;
  }

private:
  std::shared_ptr<stream_element_processor<In>> successor_;
  Predicate pred_;
  value_type_ptr value_;
};

template<class In, class Out, typename Predicate>
class flatmap_element_processor : public stream_element_processor<Out>
{
public:
  typedef stream_element_processor<Out> base;
  typedef typename base::value_type value_type;
  typedef typename base::value_type_ptr value_type_ptr;
  typedef In input_value_type;

  flatmap_element_processor(std::shared_ptr<stream_element_processor<In>> successor, Predicate &&pred);

  value_type_ptr value() override;

protected:
  bool process_impl() override;

private:
  std::shared_ptr<stream_element_processor<In>> successor_;
  Predicate pred_;
  value_type_ptr value_;
  bool first_ = true;
  stream<Out> current_stream_;
};

template<class In, class Out = std::vector<In>>
class pack_every_element_processor : public stream_element_processor<Out>
{
public:
  typedef stream_element_processor<Out> base;
  typedef typename base::value_type value_type;
  typedef typename base::value_type_ptr value_type_ptr;
  typedef In input_value_type;

  pack_every_element_processor(std::shared_ptr<stream_element_processor<In>> successor, std::size_t packsize)
    : successor_(std::move(successor)), packsize_(packsize), current_packsize_(packsize)
  {}

  value_type_ptr value() override
  {
    return value_;
  }

protected:
  bool process_impl() override
  {
    if (current_packsize_ >= packsize_) {
      current_packsize_ = 0;
      value_ = std::make_shared<Out>();
    }

    while (current_packsize_++ < packsize_) {
      if (!successor_->process()) {
        return false;
      }
      value_->push_back(*successor_->value());
    }
    return true;
  }

private:
  std::shared_ptr<stream_element_processor<In>> successor_;
  std::size_t packsize_ {};
  std::size_t current_packsize_ {};
  value_type_ptr value_;
};

template<class Out, typename Iterator>
std::shared_ptr<stream_element_processor<Out>> make_from(Iterator begin, Iterator end)
{
  return std::make_shared<iterator_element_processor<Out, Iterator>>(begin, end);
}

template<class Out, typename Container>
std::shared_ptr<stream_element_processor<Out>> make_from(Container &&container)
{
  return std::make_shared<container_element_processor<Out, Container>>(std::forward<Container>(container));
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

template<class Out, typename R = Out>
std::shared_ptr<stream_element_processor<R>> make_every(std::size_t every, std::shared_ptr<stream_element_processor<Out>> successor)
{
  return std::make_shared<every_element_processor<Out>>(successor, every);
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

template<class In, typename Predicate, typename Out = typename std::result_of<Predicate &(In)>::type::value_type>
std::shared_ptr<stream_element_processor<Out>> make_flatmap(Predicate &&pred, std::shared_ptr<stream_element_processor<In>> successor);

template<class Out, typename R = Out>
std::shared_ptr<stream_element_processor<R>> make_concat(std::shared_ptr<stream_element_processor<Out>> successor, std::shared_ptr<stream_element_processor<Out>> next)
{
  return std::make_shared<concat_element_processor<Out>>(successor, next);
}

template<class In, class Out = std::vector<In>>
std::shared_ptr<stream_element_processor<Out>> make_pack_every(std::shared_ptr<stream_element_processor<In>> successor, std::size_t packsize)
{
  return std::make_shared<pack_every_element_processor<In, Out>>(successor, packsize);
}

}

/// @endcond

}
#endif //MATADOR_STREAM_PROCESSOR_HPP
