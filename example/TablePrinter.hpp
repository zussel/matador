#ifndef TABLEPRINTER_HPP
#define TABLEPRINTER_HPP

#include "Artist.hpp"

#include "object/object_atomizer.hpp"
#include "object/object_ptr.hpp"
#include "object/object_view.hpp"
#include "object/object_store.hpp"

#include <vector>
#include <sstream>

#ifdef WIN32
#include <functional>
#else
#include <tr1/functional>
#endif

using namespace std::tr1::placeholders;

namespace oos {
  class varchar_base;
  class object_base_ptr;
}

class TablePrinter : public oos::object_atomizer
{
private:
  enum state_t {
    HEADER_SEPARATOR = 0,
    TABLE_HEADER,
    COLUMN_HEADER,
    COLUMN_SEPARATOR,
    ITEM_ROW
  };

public:
  explicit TablePrinter(const oos::object_store &ostore);
  virtual ~TablePrinter();

  void print(const std::string &type, const std::string &filter);

  template < class T >
  void print(std::ostream &out)
  {
    typedef oos::object_view<T> oview_t;
    oview_t oview(ostore_);
    // print header
    print_header(out, oview.node());
    // print row
    state_ = ELEMENT;

    typename oview_t::const_iterator first = oview.begin();
    typename oview_t::const_iterator last = oview.end();

    while (first != last) {
      oos::object_ptr<T> optr = *first++;
//      out << "artist: " << optr->id() << "\n";
      print_element(out, optr);
    }
//    for_each(oview.begin(), oview.end(), std::tr1::bind(&TablePrinter::print_element, this, std::tr1::ref(out), _1));
    // print footer

  }

private:
  void print_header(std::ostream &out, const oos::prototype_node *node);
  void print_element(std::ostream &out, const oos::object_base_ptr &optr);

  unsigned int column_width(const char *id, size_t min) const;

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
  typedef std::pair<std::string, unsigned int> column_info_t;
  typedef std::vector<column_info_t> column_info_vector_t;
  column_info_vector_t column_info_vector_;

  std::stringstream line_;
};

#endif /* TABLEPRINTER_HPP */
