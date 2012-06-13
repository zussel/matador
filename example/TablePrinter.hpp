#ifndef TABLEPRINTER_HPP
#define TABLEPRINTER_HPP

#include "Artist.hpp"

#include "object/object_atomizer.hpp"
#include "object/object_ptr.hpp"
#include "object/object_view.hpp"
#include "object/object_store.hpp"

#include <vector>
#include <sstream>
#include <iomanip>

#ifdef WIN32
#include <functional>
#else
#include <tr1/functional>
#endif

using namespace std::tr1::placeholders;
using namespace std;

namespace oos {
  class varchar_base;
  class object_base_ptr;
}

class TablePrinter : public oos::object_atomizer
{
private:
  enum state_t {
    INITIALIZE = 0,
    COLUMN_HEADER,
    COLUMN_SEPARATOR,
    ITEM_ROW
  };

public:
  explicit TablePrinter(const oos::object_store &ostore);
  virtual ~TablePrinter();

  void print(std::ostream &out, const std::string &type, const std::string &filter);

private:
  unsigned int column_width(const char *id, size_t min) const;

  void print_line(std::ostream &out, state_t state, const oos::object *o, const char *prefix);

  template < class T >
  void write_column(const char *id, T x, unsigned int width)
  {
    switch (state_) {
      case INITIALIZE:
        column_info_vector_.push_back(column_info_t(id, column_width(id, width)));
        width_ += column_info_vector_.back().second;
        break;
      case COLUMN_HEADER:
        line_ << " " << setfill(' ') << setw(width-1) << left << id << " |";
        break;
      case COLUMN_SEPARATOR:
        line_ << setfill('-') << setw(width+2) << right << "+";
        break;
      case ITEM_ROW:
        line_ << setfill(' ') << setw(width) << right << x << " |";
        break;
    }
  }

  virtual void write(const char *id, char x);
	virtual void write(const char *id, float x);
	virtual void write(const char *id, double x);
	virtual void write(const char *id, short x);
	virtual void write(const char *id, int x);
  virtual void write(const char *id, long x);
	virtual void write(const char *id, unsigned short x);
	virtual void write(const char *id, unsigned int x);
	virtual void write(const char *id, unsigned long x);
	virtual void write(const char *id, bool x);
	virtual void write(const char *id, const char *x);
	virtual void write(const char *id, const std::string &x);
  virtual void write(const char *id, const oos::varchar_base &x);
  virtual void write(const char *id, const oos::object_base_ptr &x);

private:
  const oos::object_store &ostore_;
  state_t state_;
  unsigned int width_;
  typedef std::pair<std::string, unsigned int> column_info_t;
  typedef std::vector<column_info_t> column_info_vector_t;
  column_info_vector_t column_info_vector_;

  std::stringstream line_;
};

#endif /* TABLEPRINTER_HPP */
