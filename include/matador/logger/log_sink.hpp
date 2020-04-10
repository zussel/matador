#ifndef MATADOR_LOG_SINK_HPP
#define MATADOR_LOG_SINK_HPP

#include <cstddef>
#include <memory>

namespace matador {

class log_sink
{
public:
  virtual ~log_sink() = default;

  virtual void write(const char *message, std::size_t size) = 0;
};

using sink_ptr = std::shared_ptr<log_sink>;

}

#endif //MATADOR_LOG_SINK_HPP
