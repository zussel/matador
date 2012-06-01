#include "TablePrinter.hpp"

#include "object/object.hpp"
#include "tools/varchar.hpp"

#include <numeric>
#include <iomanip>

using namespace oos;

using std::setw;
using std::setfill;
using std::left;
using std::right;

TablePrinter::TablePrinter(const oos::object_store &ostore)
  : ostore_(ostore)
  , state_(HEADER)
{}

TablePrinter::~TablePrinter()
{}

void TablePrinter::print_header(std::ostream &out, const prototype_node *node)
{
  state_ = HEADER;
  /* determine width for all columns
   */
  column_info_vector_.clear();
  object *o = node->producer->create();
  o->write_to(this);
  delete o;
  // print header
  std::stringstream table_header, column_header, column_names;
  table_header << "|";
  column_header << "+";
  column_names << "|";
  column_info_vector_t::const_iterator first = column_info_vector_.begin();
  column_info_vector_t::const_iterator last = column_info_vector_.end();
  unsigned int width(0);
  while (first != last) {
    const column_info_t &info = *first++;
    width += info.second + 1;
    column_header << setfill('-') << setw(info.second) << "-" << "+";
    column_names << " " << setfill(' ') << setw(info.second-1) << left << info.first << "|";
  }
  out << "+" << setfill('-') << setw(width-1) << "-" << "+\n";
  table_header << " " << setfill(' ') << setw(width-2) << left << node->type << "|";
  out << table_header.str() << "\n" << column_header.str() << "\n" << column_names.str() << "\n" << column_header.str() << "\n";
}

void TablePrinter::print_element(std::ostream &out, const oos::object_base_ptr &optr)
{
  line_.str("");
  out << "|";
  optr.ptr()->write_to(this);
  out << line_.str() << "\n";
}

unsigned int TablePrinter::column_width(const char *id, size_t min) const
{
  return 2 + std::max(strlen(id), min) + 1;
}

void TablePrinter::write(const char *id, char x)
{
  switch (state_) {
    case HEADER:
      column_info_vector_.push_back(column_info_t(id, column_width(id, 3)));
      break;
    case ELEMENT:
      line_ << setfill(' ') << setw(3) << x << " |";
      break;
    case FOOTER:
      break;
  }
}

void TablePrinter::write(const char *id, float x)
{
  switch (state_) {
    case HEADER:
      column_info_vector_.push_back(column_info_t(id, column_width(id, 10)));
      break;
    case ELEMENT:
      line_ << setfill(' ') << setw(10) << x << " |";
      break;
    case FOOTER:
      break;
  }
}

void TablePrinter::write(const char *id, double x)
{
  switch (state_) {
    case HEADER:
      column_info_vector_.push_back(column_info_t(id, column_width(id, 14)));
      break;
    case ELEMENT:
      line_ << setfill(' ') << setw(14) << x << " |";
      break;
    case FOOTER:
      break;
  }
}

void TablePrinter::write(const char *id, short x)
{
  switch (state_) {
    case HEADER:
      column_info_vector_.push_back(column_info_t(id, column_width(id, 5)));
      break;
    case ELEMENT:
      line_ << setfill(' ') << setw(5) << x << " |";
      break;
    case FOOTER:
      break;
  }
}

void TablePrinter::write(const char *id, int x)
{
  switch (state_) {
    case HEADER:
      column_info_vector_.push_back(column_info_t(id, column_width(id, 10)));
      break;
    case ELEMENT:
      line_ << setfill(' ') << setw(10) << x << " |";
      break;
    case FOOTER:
      break;
  }
}

void TablePrinter::write(const char *id, long x)
{
  switch (state_) {
    case HEADER:
      column_info_vector_.push_back(column_info_t(id, column_width(id, 10)));
      break;
    case ELEMENT:
      line_ << setfill(' ') << setw(12) << right << x << " |";
      break;
    case FOOTER:
      break;
  }
}

void TablePrinter::write(const char *id, unsigned short x)
{
  switch (state_) {
    case HEADER:
      column_info_vector_.push_back(column_info_t(id, column_width(id, 10)));
      break;
    case ELEMENT:
      line_ << setfill(' ') << setw(10) << x << " |";
      break;
    case FOOTER:
      break;
  }
}

void TablePrinter::write(const char *id, unsigned int x)
{
  switch (state_) {
    case HEADER:
      column_info_vector_.push_back(column_info_t(id, column_width(id, 10)));
      break;
    case ELEMENT:
      line_ << setfill(' ') << setw(10) << x << " |";
      break;
    case FOOTER:
      break;
  }
}

void TablePrinter::write(const char *id, unsigned long x)
{
  switch (state_) {
    case HEADER:
      column_info_vector_.push_back(column_info_t(id, column_width(id, 10)));
      break;
    case ELEMENT:
      line_ << setfill(' ') << setw(10) << x << " |";
      break;
    case FOOTER:
      break;
  }
}

void TablePrinter::write(const char *id, bool x)
{
  switch (state_) {
    case HEADER:
      column_info_vector_.push_back(column_info_t(id, column_width(id, 1)));
      break;
    case ELEMENT:
      line_ << setfill(' ') << setw(1) << x << " |";
      break;
    case FOOTER:
      break;
  }
}

void TablePrinter::write(const char *id, const char *x)
{
  switch (state_) {
    case HEADER:
      column_info_vector_.push_back(column_info_t(id, column_width(id, 20)));
      break;
    case ELEMENT:
      line_ << " " << setfill(' ') << setw(21) << left << x << " |";
      break;
    case FOOTER:
      break;
  }
}

void TablePrinter::write(const char *id, const std::string &x)
{
  switch (state_) {
    case HEADER:
      column_info_vector_.push_back(column_info_t(id, column_width(id, 40)));
      break;
    case ELEMENT:
      line_ << " " << setfill(' ') << setw(41) << left << x.substr(0, 40) << " |";
      break;
    case FOOTER:
      break;
  }
}

void TablePrinter::write(const char *id, const oos::varchar_base &x)
{
  switch (state_) {
    case HEADER:
      column_info_vector_.push_back(column_info_t(id, column_width(id, x.capacity())));
      break;
    case ELEMENT:
      line_ << " " << setfill(' ') << setw(x.capacity() + 1) << left << x.str() << " |";
      break;
    case FOOTER:
      break;
  }
}

void TablePrinter::write(const char *id, const oos::object_base_ptr &x)
{
  switch (state_) {
    case HEADER:
      column_info_vector_.push_back(column_info_t(id, column_width(id, 10)));
      break;
    case ELEMENT:
      line_ << " " << setfill(' ') << setw(11) << right << x.id() << " |";
      break;
    case FOOTER:
      break;
  }
}
