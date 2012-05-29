#ifndef TABLEPRINTER_HPP
#define TABLEPRINTER_HPP

#include "object/object_atomizer.hpp"
#include "object/object_view.hpp"
#include "object/object_store.hpp"

namespace oos {
  class varchar_base;
  class object_base_ptr;
}

class TablePrinter : public oos::object_atomizer
{
private:
  enum state_t {
    HEADER = 0,
    ELEMENT,
    FOOTER
  };

public:
  explicit TablePrinter(const oos::object_store &ostore);
  virtual ~TablePrinter();

  template < class T >
  void print(std::ostream &out)
  {
    oos::object_view<T> oview(ostore_);
    // print header
    print_header(oview.node());
    // print row
    // print footer

  }

private:
  void print_header(const oos::prototype_node *node);

  unsigned int column_width(const char *id, unsigned int min) const;

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
};

#endif /* TABLEPRINTER_HPP */
