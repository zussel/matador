#ifndef MATADOR_USER_HPP
#define MATADOR_USER_HPP

#include "matador/query/relations/collection.hpp"
#include "matador/query/relations/object_ptr.hpp"
#include "matador/query/mapping/access.hpp"
#include "matador/query/model/foreign_key_options.hpp"
#include "matador/query/model/primary_key_options.hpp"

#include <string>

namespace matador::test {
template<const query::primary_key_options &PkAttribute>
struct user_session_pk_generator;

template<const query::primary_key_options &PkAttribute>
struct user_pk_generator {
  unsigned int id{};
  std::string name;
  std::string password;
  query::object_ptr<user_session_pk_generator<PkAttribute>> session;

  user_pk_generator() = default;
  user_pk_generator(std::string name, std::string password)
  : name(std::move(name)), password(std::move(password)) {}
  user_pk_generator(const unsigned int id, std::string name, std::string password)
  : id(id), name(std::move(name)), password(std::move(password)) {}

  template<class Operator>
  void process(Operator &op) {
    namespace field = query::access;
    field::primary_key(op, "id", id, PkAttribute);
    field::attribute(op, "name", name, UniqueVarChar255);
    field::attribute(op, "password", password, UniqueVarChar255);
    field::has_one(op, "session", session, "user_id", CascadeAllFetchLazy);
  }
};

template<const query::primary_key_options &PkAttribute>
struct user_session_pk_generator {
  unsigned int id{};
  std::string session_token;
  query::object_ptr<user_pk_generator<PkAttribute>> user_;

  user_session_pk_generator() = default;
  user_session_pk_generator(std::string session_token, query::object_ptr<user_pk_generator<PkAttribute>> user)
  : session_token(std::move(session_token)), user_(std::move(user)) {}
  user_session_pk_generator(const unsigned int id, std::string session_token, query::object_ptr<user_pk_generator<PkAttribute>> user)
  : id(id), session_token(std::move(session_token)), user_(std::move(user)) {}

  template<class Operator>
  void process(Operator &op) {
    namespace field = query::access;
    field::primary_key(op, "id", id, PkAttribute);
    field::attribute(op, "session_token", session_token, VarChar255);
    field::belongs_to(op, "user_id", user_, CascadeAllFetchLazy);
  }
};

using user = user_pk_generator<query::Manual>;
using user_identity = user_pk_generator<query::Identity>;
using user_sequence = user_pk_generator<query::Sequence>;
using user_table = user_pk_generator<query::Table>;
using user_session = user_session_pk_generator<query::Manual>;
using user_session_identity = user_session_pk_generator<query::Identity>;
using user_session_sequence = user_session_pk_generator<query::Sequence>;
using user_session_table = user_session_pk_generator<query::Table>;
}

#endif //MATADOR_USER_HPP
