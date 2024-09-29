#include <catch2/catch_test_macros.hpp>

#include "matador/sql/connection_info.hpp"
#include "matador/sql/backend_provider.hpp"

#include "matador/utils/os.hpp"

using namespace matador::sql;

TEST_CASE("Load backend", "[backend provider]") {
  using namespace matador::utils;
  auto path = os::getenv("MATADOR_BACKENDS_PATH");
  REQUIRE(path.empty());

  os::setenv("MATADOR_BACKENDS_PATH", matador::os::get_current_dir().c_str(), os::override_env_value::OverrideValue);

  path = os::getenv("MATADOR_BACKENDS_PATH");
  REQUIRE(!path.empty());

  if (path.back() != matador::os::DIR_SEPARATOR) {
    path.push_back(matador::os::DIR_SEPARATOR);
  }

  REQUIRE(!path.empty());

  connection_info ci{"noop"};
  const auto &d = backend_provider::instance().connection_dialect("noop");
  auto *connection = backend_provider::instance().create_connection("noop", ci);
  REQUIRE(connection != nullptr);
  backend_provider::instance().destroy_connection("noop", connection);

  os::unsetenv("MATADOR_BACKENDS_PATH");
}