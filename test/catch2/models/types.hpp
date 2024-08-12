#ifndef QUERY_TYPES_HPP
#define QUERY_TYPES_HPP

#include "matador/utils/access.hpp"
#include "matador/utils/types.hpp"

namespace matador::test {

struct types
{
    enum { CSTR_LEN=255 };

    unsigned long id_ = 0;
    char char_ = 'c';
    short short_ = -127;
    int int_ = -65000;
    long long_ = -128000;
    long long long64_ = -1234567890;
    unsigned char unsigned_char_ = 'H';
    unsigned short unsigned_short_ = 128;
    unsigned int unsigned_int_ = 65000;
    unsigned long unsigned_long_ = 128000;
    unsigned long long unsigned_long64_ = 1234567890;
    float float_ = 3.1415f;
    double double_ = 1.1414;
    bool bool_ = true;
    char cstr_[CSTR_LEN]{};
    std::string string_ = "Welt";
    std::string varchar_ = "Erde";
    matador::date date_;
    matador::time time_;
    utils::blob binary_{ 1, 2, 3, 4 };

    template < class Operator >
    void process(Operator &op)
    {
        namespace field = matador::access;
        using namespace matador::utils;
        field::primary_key(op, "id", id_);
        field::attribute(op, "val_char", char_);
        field::attribute(op, "val_float", float_);
        field::attribute(op, "val_double", double_);
        field::attribute(op, "val_short", short_);
        field::attribute(op, "val_int", int_);
        field::attribute(op, "val_long", long_);
        field::attribute(op, "val_long_long", long64_);
        field::attribute(op, "val_unsigned_char", unsigned_char_);
        field::attribute(op, "val_unsigned_short", unsigned_short_);
        field::attribute(op, "val_unsigned_int", unsigned_int_);
        field::attribute(op, "val_unsigned_long", unsigned_long_);
        field::attribute(op, "val_unsigned_long_long", unsigned_long64_);
        field::attribute(op, "val_bool", bool_);
        field::attribute(op, "val_cstr", cstr_, CSTR_LEN);
        field::attribute(op, "val_string", string_);
        field::attribute(op, "val_varchar", varchar_, 63);
        field::attribute(op, "val_date", date_);
        field::attribute(op, "val_time", time_);
        field::attribute(op, "val_binary", binary_);
    }
};

}

#endif // QUERY_TYPES_HPP
