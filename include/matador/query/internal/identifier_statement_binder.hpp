#ifndef MATADOR_IDENTIFIER_STATEMENT_BINDER_HPP
#define MATADOR_IDENTIFIER_STATEMENT_BINDER_HPP

#include "matador/query/identifier.hpp"
#include "matador/query/identifier_serializer.hpp"

namespace matador::query {
class statement;

class identifier_statement_binder : public identifier_serializer {
public:
  explicit identifier_statement_binder(statement &stmt, size_t index = 0);

  void bind(const identifier &id);

  void serialize(int8_t &, const column_options &) override;
  void serialize(int16_t &, const column_options &) override;
  void serialize(int32_t &, const column_options &) override;
  void serialize(int64_t &, const column_options &) override;
  void serialize(uint8_t &, const column_options &) override;
  void serialize(uint16_t &, const column_options &) override;
  void serialize(uint32_t &, const column_options &) override;
  void serialize(uint64_t &, const column_options &) override;
  void serialize(std::string &, const column_options &) override;
  void serialize(utils::null_type_t &, const column_options &) override;

private:
  statement &stmt_;
  size_t index_{};
};

}
#endif //MATADOR_IDENTIFIER_STATEMENT_BINDER_HPP