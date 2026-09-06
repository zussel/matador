#ifndef QUERY_INTERMEDIATE_HPP
#define QUERY_INTERMEDIATE_HPP

#include <memory>

namespace matador::query {
struct query_data;

class intermediate {
public:
  intermediate();
  intermediate(const std::shared_ptr<query_data> &context); // NOLINT(*-explicit-constructor)

protected:
  std::shared_ptr<query_data> context_;
};

}

#endif //QUERY_INTERMEDIATE_HPP
