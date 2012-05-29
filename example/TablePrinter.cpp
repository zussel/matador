#include "TablePrinter.hpp"

#include "object/object.hpp"

using namespace oos;

TablePrinter::TablePrinter(const oos::object_store &ostore)
  : ostore_(ostore)
  , state_(HEADER)
  , width_(0)
{}

TablePrinter::~TablePrinter()
{}

void TablePrinter::print_header(const prototype_node *node)
{
  // determine width
  width_ = 0;
  object *o = node->producer->create();
  o->read_from(this);
  delete o;
  // print header
}

unsigned int TablePrinter::column_width(const char *id, unsigned int min) const
{
  return 2 + std::max(strlen(id), min) + 1;
}

void TablePrinter::write(const char *id, char x)
{
  switch (state_) {
    case HEADER:
      width_ += column_width(id, 3);
      break;
    case ELEMENT:
      break;
    case FOOTER:
      break;
  }
}

void TablePrinter::write(const char *id, float x)
{
  switch (state_) {
    case HEADER:
      width_ += column_width(id, 10);
      break;
    case ELEMENT:
      break;
    case FOOTER:
      break;
  }
}

void TablePrinter::write(const char *id, double x)
{
}

void TablePrinter::write(const char *id, short x)
{
}

void TablePrinter::write(const char *id, int x)
{
}

void TablePrinter::write(const char *id, long x)
{
}

void TablePrinter::write(const char *id, unsigned short x)
{
}

void TablePrinter::write(const char *id, unsigned int x)
{
}

void TablePrinter::write(const char *id, unsigned long x)
{
}

void TablePrinter::write(const char *id, bool x)
{
}

void TablePrinter::write(const char *id, const char *x)
{
}

void TablePrinter::write(const char *id, const std::string &x)
{
}

void TablePrinter::write(const char *id, const oos::varchar_base &x)
{
}

void TablePrinter::write(const char *id, const oos::object_base_ptr &x)
{
}
