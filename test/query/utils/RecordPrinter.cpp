#include "RecordPrinter.hpp"

#include "matador/utils/types.hpp"

#include "matador/query/model/record.hpp"

#include <iostream>
#include <iomanip>

namespace matador::test {
RecordPrinter::RecordPrinter(std::ostream &os)
: os_(os) {
  type_widths_ = {
    {utils::basic_type::Int8, 4},
    {utils::basic_type::Int16, 6},
    {utils::basic_type::Int32, 11},
    {utils::basic_type::Int64, 20},
    {utils::basic_type::UInt8, 4},
    {utils::basic_type::UInt16, 6},
    {utils::basic_type::UInt32, 11},
    {utils::basic_type::UInt64, 20},
    {utils::basic_type::Float, 12},
    {utils::basic_type::Double, 15},
    {utils::basic_type::Boolean, 6},
    {utils::basic_type::Varchar, 20},
    {utils::basic_type::Text, 30},
    {utils::basic_type::Date, 12},
    {utils::basic_type::DateTime, 20},
    {utils::basic_type::Time, 10},
    {utils::basic_type::Blob, 10},
    {utils::basic_type::Null, 6}
  };
}

void RecordPrinter::print_header(const query::record &rec) const {
  for (const auto &f: rec.columns()) {
    os_ << std::left << std::setw(width(f)) << f.name() << " ";
  }
  os_ << "\n";

  for (const auto &f: rec.columns()) {
    os_ << std::string(width(f), '-') << " ";
  }
  os_ << "\n";
}

void RecordPrinter::print(const query::record &rec) const {
  for (const auto &f: rec.columns()) {
    os_ << std::left << std::setw(width(f)) << f.str() << " ";
  }
  os_ << "\n";
}

int RecordPrinter::width(const query::field &f) {
  // If it's a varchar/string and has a defined size, use it if it's reasonable, 
  // otherwise fall back to type defaults.
  if ((f.is_varchar() || f.is_string()) && f.size() > 0 && f.size() < 100) {
    return static_cast<int>(std::max(f.name().length(), f.size()));
  }

  // Find the default width for type
  // Note: field class doesn't expose basic_type directly, but we can use value().type()
  // if we had access. For now we use name length as a minimum.
  constexpr size_t w = 15;
  // In a real implementation we would probe the field's underlying type.

  return static_cast<int>(std::max(f.name().length(), w));
}
}
