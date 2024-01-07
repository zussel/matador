#ifndef MATADOR_CONNECTION_INFO_HPP
#define MATADOR_CONNECTION_INFO_HPP

#include "matador/utils/access.hpp"

#include <string>

namespace matador {

/**
 * This class contains all information
 * about a database connection consisting
 * of
 * - database type
 * - username
 * - password
 * - hostname
 * - port
 * - database name
 * - driver
 */
struct connection_info
{
  std::string type;         /**< Type of the database i.e. sqlite or mysql. */
  std::string user;         /**< Username to login. */
  std::string password;     /**< Password for login. */
  std::string hostname;     /**< Hostname of the database. */
  unsigned short port{};    /**< Port of the database server. */
  std::string database;     /**< Name of the database to use */
  std::string driver;       /**< Driver to use. This is used by th mssql/odbc backend. */

  template < class Operator >
  void process(Operator &op)
  {
    namespace field = matador::access;
    field::attribute(op, "type", type);
    field::attribute(op, "user", user);
    field::attribute(op, "password", password);
    field::attribute(op, "hostname", hostname);
    field::attribute(op, "port", port);
    field::attribute(op, "database", database);
    field::attribute(op, "driver", driver);
  }

  /**
   * This parses database uri into a connection_info object.
   *
   * @param info The database uri
   * @param default_port The default port to use
   * @param default_driver The default driver to use
   * @return A connection_info object
   */
  static connection_info parse(const std::string &info, unsigned short default_port = 0, const std::string &default_driver = "");

  /**
   * Convert a connection_info object
   * into a database uri.
   *
   * @param ci The connection_info object to convert
   * @return The corresponding database uri
   */
  static std::string to_string(const connection_info &ci);
};

}
#endif //MATADOR_CONNECTION_INFO_HPP
