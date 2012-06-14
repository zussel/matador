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
  , state_(INITIALIZE)
  , width_(0)
{}

TablePrinter::~TablePrinter()
{}

/*
template < class T >
void compare(const object *o, const std::string &name, const std::string &val)
{
  T a;
  o->get(str, b);

  T b = strtol(val.c_str(), NULL, 10);
}
*/

void TablePrinter::print(std::ostream &out, const std::string &type, const std::string &filter)
{
  generic_view gview(type, ostore_);
  
  generic_view::iterator first = gview.begin();
  generic_view::iterator last = gview.end();

  /*
   * create a temporary object
   */
  object *o = ostore_.create(type.c_str());
  if (!o) {
    throw std::logic_error("unknow object type [" + type + "] to create");
  }
  /*
   * print first header line
   * and initialize column width vector
   */
  width_ = 0;
  state_ = INITIALIZE;
  o->write_to(this);
  out << "+" << setfill('-') << setw(width_) << "-+" << "\n";
  out << "| " << setfill(' ') << setw(width_-2) << left << type.c_str() << "|\n";

  print_line(out, COLUMN_SEPARATOR, o, "+");
  print_line(out, COLUMN_HEADER, o, "|");
  print_line(out, COLUMN_SEPARATOR, o, "+");

  /*
   * print items
   */
  while (first != last) {
    generic_view::object_pointer optr = *first++;
    print_line(out, ITEM_ROW, optr.get(), "|");
  }

  /*
   * print column separator
   */
  print_line(out, COLUMN_SEPARATOR, o, "+");

  delete o;
}

void TablePrinter::print_line(std::ostream &out, state_t state, const object *o, const char *prefix)
{
  state_ = state;
  line_.str("");
  o->write_to(this);
  out << prefix << line_.str() << "\n";
}

unsigned int TablePrinter::column_width(const char *id, size_t min) const
{
  return 2 + std::max(strlen(id), min) + 1;
}

void TablePrinter::write(const char *id, char x)
{
  write_column(id, x, 3);
}

void TablePrinter::write(const char *id, float x)
{
  write_column(id, x, 10);
}

void TablePrinter::write(const char *id, double x)
{
  write_column(id, x, 14);
}

void TablePrinter::write(const char *id, short x)
{
  write_column(id, x, 5);
}

void TablePrinter::write(const char *id, int x)
{
  write_column(id, x, 10);
}

void TablePrinter::write(const char *id, long x)
{
  write_column(id, x, 10);
}

void TablePrinter::write(const char *id, unsigned short x)
{
  write_column(id, x, 10);
}

void TablePrinter::write(const char *id, unsigned int x)
{
  write_column(id, x, 10);
}

void TablePrinter::write(const char *id, unsigned long x)
{
  write_column(id, x, 10);
}

void TablePrinter::write(const char *id, bool x)
{
  write_column(id, x, 1);
}

void TablePrinter::write(const char *id, const char *x)
{
  switch (state_) {
    case INITIALIZE:
      column_info_vector_.push_back(column_info_t(id, column_width(id, 20)));
      width_ += column_info_vector_.back().second;
      break;
    case COLUMN_HEADER:
      line_ << " " << setfill(' ') << setw(21) << left << id << " |";
      break;
    case COLUMN_SEPARATOR:
      line_ << setfill('-') << setw(24) << right << "+";
      break;
    case ITEM_ROW:
      line_ << " " << setfill(' ') << setw(21) << left << x << " |";
      break;
  }
}

void TablePrinter::write(const char *id, const std::string &x)
{
  switch (state_) {
    case INITIALIZE:
      column_info_vector_.push_back(column_info_t(id, column_width(id, 40)));
      width_ += column_info_vector_.back().second;
      break;
    case COLUMN_HEADER:
      line_ << " " << setfill(' ') << setw(41) << left << id << " |";
      break;
    case COLUMN_SEPARATOR:
      line_ << setfill('-') << setw(44) << right << "+";
      break;
    case ITEM_ROW:
      line_ << " " << setfill(' ') << setw(41) << left << x.substr(0, 40) << " |";
      break;
  }
}

void TablePrinter::write(const char *id, const oos::varchar_base &x)
{
  switch (state_) {
    case INITIALIZE:
      column_info_vector_.push_back(column_info_t(id, column_width(id, x.capacity())));
      width_ += column_info_vector_.back().second;
      break;
    case COLUMN_HEADER:
      line_ << " " << setfill(' ') << setw(x.capacity() + 1) << left << id << " |";
      break;
    case COLUMN_SEPARATOR:
      line_ << setfill('-') << setw(x.capacity() + 4) << right << "+";
      break;
    case ITEM_ROW:
      line_ << " " << setfill(' ') << setw(x.capacity() + 1) << left << x.str() << " |";
      break;
  }
}

void TablePrinter::write(const char *id, const oos::object_base_ptr &x)
{
  switch (state_) {
    case INITIALIZE:
      column_info_vector_.push_back(column_info_t(id, column_width(id, 10)));
      width_ += column_info_vector_.back().second;
      break;
    case COLUMN_HEADER:
      line_ << " " << setfill(' ') << setw(11) << left << id << " |";
      break;
    case COLUMN_SEPARATOR:
      line_ << setfill('-') << setw(14) << right << "+";
      break;
    case ITEM_ROW:
      line_ << " " << setfill(' ') << setw(11) << right << x.id() << " |";
      break;
  }
}
