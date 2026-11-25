#include "SchemaFixture.hpp"

#include "connection.hpp"

namespace matador::test {
SchemaFixture::SchemaFixture()
: pool(connection::dns, 4) {}
}
