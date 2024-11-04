#include "matador/utils/default_type_traits.hpp"

#include "matador/utils/attribute_reader.hpp"
#include "matador/utils/attribute_writer.hpp"

namespace matador::utils {

void data_type_traits<nullptr_t>::read_value(attribute_reader &/*reader*/, const char * /*id*/, size_t /*index*/, nullptr_t &/*value*/)
{}

void data_type_traits<nullptr_t>::bind_value(attribute_writer &/*binder*/, size_t /*index*/, nullptr_t &/*value*/)
{}

void data_type_traits<char>::read_value(attribute_reader &reader, const char *id, size_t index, char &value)
{
  reader.read_value(id, index, value);
}

void data_type_traits<char>::bind_value(attribute_writer &binder, size_t index, char &value)
{
  binder.write_value(index, value);
}

void data_type_traits<short>::read_value(attribute_reader &reader, const char *id, size_t index, short &value)
{
  reader.read_value(id, index, value);
}

void data_type_traits<short>::bind_value(attribute_writer &binder, size_t index, short &value)
{
  binder.write_value(index, value);
}

void data_type_traits<int>::read_value(attribute_reader &reader, const char *id, size_t index, int &value)
{
  reader.read_value(id, index, value);
}

void data_type_traits<int>::bind_value(attribute_writer &binder, size_t index, int &value)
{
  binder.write_value(index, value);
}

void data_type_traits<long>::read_value(attribute_reader &reader, const char *id, size_t index, long &value)
{
  reader.read_value(id, index, value);
}

void data_type_traits<long>::bind_value(attribute_writer &binder, size_t index, long &value)
{
  binder.write_value(index, value);
}

void data_type_traits<long long>::read_value(attribute_reader &reader, const char *id, size_t index, long long &value)
{
  reader.read_value(id, index, value);
}

void data_type_traits<long long int>::bind_value(attribute_writer &binder, size_t index, long long int &value)
{
  binder.write_value(index, value);
}

void data_type_traits<unsigned char>::read_value(attribute_reader &reader, const char *id, size_t index, unsigned char &value)
{
  reader.read_value(id, index, value);
}

void data_type_traits<unsigned char>::bind_value(attribute_writer &binder, size_t index, unsigned char &value)
{
  binder.write_value(index, value);
}

void data_type_traits<unsigned short>::read_value(attribute_reader &reader, const char *id, size_t index, unsigned short &value)
{
  reader.read_value(id, index, value);
}

void data_type_traits<unsigned short>::bind_value(attribute_writer &binder, size_t index, unsigned short &value)
{
  binder.write_value(index, value);
}

void data_type_traits<unsigned int>::read_value(attribute_reader &reader, const char *id, size_t index, unsigned int &value)
{
  reader.read_value(id, index, value);
}

void data_type_traits<unsigned int>::bind_value(attribute_writer &binder, size_t index, unsigned int &value)
{
  binder.write_value(index, value);
}

void data_type_traits<unsigned long>::read_value(attribute_reader &reader, const char *id, size_t index, unsigned long &value)
{
  reader.read_value(id, index, value);
}

void data_type_traits<unsigned long>::bind_value(attribute_writer &binder, size_t index, unsigned long &value)
{
  binder.write_value(index, value);
}

void data_type_traits<unsigned long long>::read_value(attribute_reader &reader, const char *id, size_t index, unsigned long long &value)
{
  reader.read_value(id, index, value);
}

void data_type_traits<unsigned long long>::bind_value(attribute_writer &binder, size_t index, unsigned long long &value)
{
  binder.write_value(index, value);
}

void data_type_traits<bool>::read_value(attribute_reader &reader, const char *id, size_t index, bool &value)
{
  reader.read_value(id, index, value);
}

void data_type_traits<bool>::bind_value(attribute_writer &binder, size_t index, bool &value)
{
  binder.write_value(index, value);
}

void data_type_traits<float>::read_value(attribute_reader &reader, const char *id, size_t index, float &value)
{
  reader.read_value(id, index, value);
}

void data_type_traits<float>::bind_value(attribute_writer &binder, size_t index, float &value)
{
  binder.write_value(index, value);
}

void data_type_traits<double>::read_value(attribute_reader &reader, const char *id, size_t index, double &value)
{
  reader.read_value(id, index, value);
}

void data_type_traits<double>::bind_value(attribute_writer &binder, size_t index, double &value)
{
  binder.write_value(index, value);
}

void data_type_traits<const char*>::read_value(attribute_reader &reader, const char *id, size_t index, const char* value, size_t size)
{
  reader.read_value(id, index, const_cast<char*>(value), size);
}

void data_type_traits<const char *>::bind_value(attribute_writer &binder, size_t index, const char *value, size_t size)
{
  binder.write_value(index, value, size);
}

void data_type_traits<char*>::read_value(attribute_reader &reader, const char *id, size_t index, char* value, size_t size)
{
  reader.read_value(id, index, value, size);
}

void data_type_traits<char *>::bind_value(attribute_writer &binder, size_t index, char *value, size_t size)
{
  binder.write_value(index, value, size);
}

void data_type_traits<std::string>::read_value(attribute_reader &reader, const char *id, size_t index, std::string &value, size_t size)
{
  reader.read_value(id, index, value, size);
}

void data_type_traits<std::string>::bind_value(attribute_writer &binder, size_t index, std::string &value, size_t size)
{
  binder.write_value(index, value, size);
}

void data_type_traits<utils::blob, void>::read_value(attribute_reader &reader, const char *id, size_t index, utils::blob &value)
{
  reader.read_value(id, index, value);
}

void data_type_traits<utils::blob, void>::bind_value(attribute_writer &binder, size_t index, utils::blob &value)
{
  binder.write_value(index, value);
}

void data_type_traits<matador::date>::read_value(attribute_reader &reader, const char *id, size_t index, date &value)
{
  reader.read_value(id, index, value);
}

void data_type_traits<matador::date>::bind_value(attribute_writer &binder, size_t index, date &value)
{
  binder.write_value(index, value);
}

void data_type_traits<matador::time>::read_value(attribute_reader &reader, const char *id, size_t index, time &value)
{
  reader.read_value(id, index, value);
}

void data_type_traits<matador::time>::bind_value(attribute_writer &binder, size_t index, time &value)
{
  binder.write_value(index, value);
}

}