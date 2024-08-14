#include "sqlite_query_compiler.hpp"

namespace matador::backends::sqlite {

sqlite_query_compiler::sqlite_query_compiler(const sql::dialect &d)
: query_compiler(d)
{}

}
