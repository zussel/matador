#include "database/condition.hpp"

namespace oos {

condition& condition::or_(const condition &cond)
{
  // TODO: check valid
  next_.reset(new condition(cond));
  logic_ = "OR ";
  return *this;
}

condition& condition::and_(const condition &cond)
{
  // TODO: check valid
  next_.reset(new condition(cond));
  logic_ = "AND ";
  return *this;
}

std::ostream& condition::print(std::ostream &out, bool prepared) const
{
  out << column_ << op_;
  if (prepared && !value_.empty()) {
    out << "?";
  } else {
    out << value_;
  }
  if (next_) {
    out << " " << logic_;
    next_->print(out, prepared);
  }
  return out;
}

condition cond(const std::string &c)
{
  return condition(c);
}

}
