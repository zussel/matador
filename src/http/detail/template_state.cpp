#include "matador/http/detail/template_state.hpp"

namespace matador {
namespace http {
namespace detail {

void global_state::execute()
{
}

bool global_state::is_executable() const
{
  return false;
}

void global_state::append(const std::string &part)
{
  rendered_.append(part);
}

void global_state::append(char c)
{
  rendered_ += c;
}

std::string global_state::str() const
{
  return rendered_;
}

bool foreach_state::is_executable() const
{
  return false;
}

void foreach_state::execute()
{

}

void foreach_state::append(const std::string &part)
{

}

void foreach_state::append(char c)
{

}

std::string foreach_state::str() const
{
  return std::string();
}

}
}
}
