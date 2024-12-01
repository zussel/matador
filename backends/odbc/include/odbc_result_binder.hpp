#ifndef ODBC_RESULT_BINDER_HPP
#define ODBC_RESULT_BINDER_HPP

#include "matador/utils/attribute_reader.hpp"

namespace matador::backends::odbc {

class odbc_result_binder final : public utils::attribute_reader
{
public:
    void read_value(const char *id, size_t pos, char &x) override;
    void read_value(const char *id, size_t pos, short &x) override;
    void read_value(const char *id, size_t pos, int &x) override;
    void read_value(const char *id, size_t pos, long &x) override;
    void read_value(const char *id, size_t pos, long long int &x) override;
    void read_value(const char *id, size_t pos, unsigned char &x) override;
    void read_value(const char *id, size_t pos, unsigned short &x) override;
    void read_value(const char *id, size_t pos, unsigned int &x) override;
    void read_value(const char *id, size_t pos, unsigned long &x) override;
    void read_value(const char *id, size_t pos, unsigned long long int &x) override;
    void read_value(const char *id, size_t pos, bool &x) override;
    void read_value(const char *id, size_t pos, float &x) override;
    void read_value(const char *id, size_t pos, double &x) override;
    void read_value(const char *id, size_t pos, time &x ) override;
    void read_value(const char *id, size_t pos, date &x ) override;
    void read_value(const char *id, size_t pos, char *x, size_t size) override;
    void read_value(const char *id, size_t pos, std::string &x) override;
    void read_value(const char *id, size_t pos, std::string &x, size_t size) override;
    void read_value(const char *id, size_t pos, utils::blob &x) override;
    void read_value(const char *id, size_t pos, utils::value &x, size_t) override;
};

}

#endif //ODBC_RESULT_BINDER_HPP
