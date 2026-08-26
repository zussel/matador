#include "matador/utils/error.hpp"

#include <utility>
#include <algorithm>

namespace matador::utils {
std::string error::error_code() const {
  return ec_.message();
}

std::string error::message() const {
  return error_message_;
}

std::string error::category() const {
  return ec_.category().name();
}

std::error_code error::ec() const {
  return ec_;
}

const std::vector<error> & error::error_trace() const {
  return error_trace_;
}

std::optional<std::string> error::error_info(const std::string &key) const {
  const auto info = error_infos_.find(key);
  if (info == error_infos_.end()) {
    return std::nullopt;
  }
  return info->second;
}

void error::add_error_info(const std::string &key, std::string value) {
  error_infos_[key] = std::move(value);
}

void error::remove_error_info(const std::string &key) {
  error_infos_.erase(key);
}

std::vector<std::string> error::error_info_keys() const {
  std::vector<std::string> keys;

  std::transform(
      std::begin(error_infos_),
      std::end(error_infos_),
      std::back_inserter(keys),
      [](const auto &pair){return pair.first;});

  return keys;
}

bool operator==(const error &lhs, const error &rhs) {
  return lhs.ec_ == rhs.ec_;
}

bool operator==(const error &lhs, const std::error_code &rhs) {
  return lhs.ec_ == rhs;
}

bool operator==(const error &lhs, const std::error_condition &rhs) {
  return lhs.ec_ == rhs;
}

bool operator!=(const error &lhs, const error &rhs) {
  return !(lhs == rhs);
}

bool operator!=(const error &lhs, const std::error_code &rhs) {
  return !(lhs == rhs);
}

bool operator!=(const error &lhs, const std::error_condition &rhs) {
  return !(lhs == rhs);
}

bool operator<(const error &lhs, const error &rhs) {
  return lhs.ec_ < rhs.ec_;
}

bool operator<(const error &lhs, const std::error_code &rhs) {
  return lhs.ec_ < rhs;
}

std::ostream & operator<<(std::ostream &out, const error &err) {
  out << err.ec_ << " " << err.ec_.message() << "\n";
  for (const auto & [key, value] : err.error_infos_) {
    out << "  " << key << ": " << value << "\n";
  }
  return out;
}

}
