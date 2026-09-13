#include "matador/query/intermediates/intermediate.hpp"

#include "matador/query/query_data.hpp"

namespace matador::query {
intermediate::intermediate()
: context_(std::make_shared<query_data>()) {}

intermediate::intermediate(const std::shared_ptr<query_data> &context)
: context_(context)
{}

std::shared_ptr<query_data> intermediate::clone_context() const {
  return std::make_shared<query_data>(*context_);
}

}