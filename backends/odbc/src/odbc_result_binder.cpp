#include "odbc_result_binder.hpp"

namespace matador::backends::odbc {

void odbc_result_binder::read_value(const char *id, size_t pos, char &x)
{}

void odbc_result_binder::read_value( const char* id, size_t pos, short& x ) {}

void odbc_result_binder::read_value( const char* id, size_t pos, int& x ) {}

void odbc_result_binder::read_value( const char* id, size_t pos, long& x ) {}

void odbc_result_binder::read_value( const char* id, size_t pos, long long int& x ) {}

void odbc_result_binder::read_value( const char* id, size_t pos, unsigned char& x ) {}

void odbc_result_binder::read_value( const char* id, size_t pos, unsigned short& x ) {}

void odbc_result_binder::read_value( const char* id, size_t pos, unsigned int& x ) {}

void odbc_result_binder::read_value( const char* id, size_t pos, unsigned long& x ) {}

void odbc_result_binder::read_value( const char* id, size_t pos, unsigned long long int& x ) {}

void odbc_result_binder::read_value( const char* id, size_t pos, bool& x ) {}

void odbc_result_binder::read_value( const char* id, size_t pos, float& x ) {}

void odbc_result_binder::read_value( const char* id, size_t pos, double& x ) {}

void odbc_result_binder::read_value( const char* id, size_t pos, time& x ) {}

void odbc_result_binder::read_value( const char* id, size_t pos, date& x ) {}

void odbc_result_binder::read_value( const char* id, size_t pos, char* x, size_t size ) {}

void odbc_result_binder::read_value( const char* id, size_t pos, std::string& x ) {}

void odbc_result_binder::read_value( const char* id, size_t pos, std::string& x, size_t size ) {}

void odbc_result_binder::read_value( const char* id, size_t pos, utils::blob& x ) {}

void odbc_result_binder::read_value( const char* id, size_t pos, utils::value& x, size_t ) {}
}