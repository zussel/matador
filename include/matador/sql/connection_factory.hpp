#ifndef DATABASE_FACTORY_HPP
#define DATABASE_FACTORY_HPP

#include "matador/sql/export.hpp"

#include "matador/utils/singleton.hpp"
#include "matador/utils/factory.hpp"
#include "matador/utils/library.hpp"

#include "matador/sql/connection.hpp"

#include <string>

namespace matador {

/// @cond MATADOR_DEV
class OOS_SQL_API connection_factory : public matador::singleton<connection_factory>
{
private:
  typedef factory<std::string, connection_impl> factory_t;
  
private:
  friend class singleton<connection_factory>;

  connection_factory();

public:
  ~connection_factory() override = default;

  connection_impl* create(const std::string &name);
  bool destroy(const std::string &name, connection_impl* impl);
  
private:

  class connection_producer : public factory_t::producer_base
  {
  public:
    connection_producer() = default;
    ~connection_producer() override = default;
    factory_t::value_type* create() const override;
    virtual void destroy(factory_t::value_type* val) const;
  };

  class dynamic_connection_producer : public connection_producer
  {
  public:
    explicit dynamic_connection_producer(const std::string &name);
    ~dynamic_connection_producer() override;
    factory_t::value_type* create() const override;
    void destroy(factory_t::value_type* val) const override;

  private:
    typedef connection_impl*(*create_func)();
    typedef void (*destroy_func)(connection_impl*);

  private:
    create_func create_;
    destroy_func destroy_;

    library loader_;
  };

private:
  factory_t factory_;
};
/// @endcond
}

#endif /* DATABASE_FACTORY_HPP */
