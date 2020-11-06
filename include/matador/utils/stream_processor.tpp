#include "matador/utils/stream_processor.hpp"

namespace matador {

/// @cond MATADOR_DEV

namespace detail {

template<class In, class Out, typename Predicate>
flatmap_element_processor<In, Out, Predicate>::flatmap_element_processor(
  std::shared_ptr<stream_element_processor<In>> successor, Predicate &&pred)
  : successor_(std::move(successor)), pred_(pred)
{}

template<class In, class Out, typename Predicate>
typename flatmap_element_processor<In, Out, Predicate>::value_type_ptr flatmap_element_processor<In, Out, Predicate>::value()
{
  return value_;
}

template<class In, class Out, typename Predicate>
bool flatmap_element_processor<In, Out, Predicate>::process_impl()
{
  if (!first_ && current_stream_.processor()->process()) {
    value_ = current_stream_.processor()->value();
    return true;
  }

  if (first_) {
    first_ = false;
  }

  while (successor_->process()) {
    current_stream_ = make_stream(std::move(pred_(std::move(*successor_->value()))));
    if (current_stream_.processor()->process()) {
      value_ = current_stream_.processor()->value();
//      value_ = std::make_shared<Out>(Out{});
      return true;
    }
  }

  value_.reset();
  return false;
}

template<class In, typename Predicate, typename Out>
std::shared_ptr<stream_element_processor<Out>>
make_flatmap(Predicate &&pred, std::shared_ptr<stream_element_processor<In>> successor)
{
  return std::make_shared<flatmap_element_processor<In, Out, Predicate>>(successor, std::forward<Predicate>(pred));
}

}

/// @endcond

}
