#ifndef OBJECT_SERIALIZER_HPP
#define OBJECT_SERIALIZER_HPP

#include "object/object_atomizer.hpp"
#include "object/byte_buffer.hpp"

class object;

class object_serializer : public object_atomizer
{
public:
  object_serializer();
  virtual ~object_serializer();

  bool serialize(object *o);
  bool deserialize(object *o);

	virtual void write_char(const char* id, char c);
	virtual void write_float(const char* id, float f);
	virtual void write_double(const char* id, double f);
	virtual void write_int(const char* id, int i);
	virtual void write_long(const char* id, long l);
	virtual void write_unsigned(const char* id, unsigned u);
	virtual void write_bool(const char* id, bool b);
	virtual void write_charptr(const char* id, const char *c);
	virtual void write_string(const char* id, const std::string &s);
	virtual void write_date(const char* id, const Date &s);
	virtual void write_time(const char* id, const Time &s);
	virtual void write_object(const char* id, const base_object_ptr &x);
	virtual void write_object_list(const char* id, const object_list_base &x);

	virtual void read_char(const char* id, char &c);
	virtual void read_float(const char* id, float &f);
	virtual void read_double(const char* id, double &f);
	virtual void read_int(const char* id, int &i);
	virtual void read_long(const char* id, long &l);
	virtual void read_unsigned(const char* id, unsigned &u);
	virtual void read_bool(const char* id, bool &b);
	virtual void read_charptr(const char* id, char *&c);
	virtual void read_string(const char* id, std::string &s);
	virtual void read_date(const char* id, Date &s);
	virtual void read_time(const char* id, Time &s);
	virtual void read_object(const char* id, base_object_ptr &x);
	virtual void read_object_list(const char* id, object_list_base &x);
  
private:
  byte_buffer buffer_;
};

#endif /* OBJECT_SERIALIZER_HPP */
