#ifndef TABLEPRINTER_HPP
#define TABLEPRINTER_HPP

#include "object/object_atomizer.hpp"

namespace oos {
  class varchar_base;
  class object_base_ptr;
}

template < class T >
class TablePrinter : public oos::object_atomizer
{
public:
  TablePrinter() 
    : print_header_(false)
  {}
  virtual ~TablePrinter() {}

  void print(std::ostream &out)
  {

  }

  virtual void write(const char *id, char x) { }
	virtual void write(const char *id, float x) { }
	virtual void write(const char *id, double x) { }
	virtual void write(const char *id, short x) { }
	virtual void write(const char *id, int x) { }
  virtual void write(const char *id, long x) { }
	virtual void write(const char *id, unsigned short x) { }
	virtual void write(const char *id, unsigned int x) { }
	virtual void write(const char *id, unsigned long x) { }
	virtual void write(const char *id, bool x) { }
	virtual void write(const char *id, const char *x) { }
	virtual void write(const char *id, const std::string &x) { }
  virtual void write(const char *id, const oos::varchar_base &x) { }
  virtual void write(const char *id, const oos::object_base_ptr &x) { }

private:
  bool print_header_;
};

#endif /* TABLEPRINTER_HPP */
