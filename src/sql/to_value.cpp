#include "matador/sql/to_value.hpp"

namespace matador::sql {
void to_value( bool& value, const char* str ) {
    if ( std::strcmp( str, "t" ) == 0 ) {
        value = true;
    }
}

}