//
// Created by sascha on 2/25/16.
//

#ifndef OOS_BASIC_DIALECT_HPP
#define OOS_BASIC_DIALECT_HPP

#include <unordered_map>
#include <string>

namespace oos {

class dialect
{
public:
  enum t_token
  {
    CREATE_TABLE = 0,
    DROP,
    DELETE,
    INSERT,
    VALUES,
    VALUE,
    UPDATE,
    SELECT,
    COLUMNS,
    COLUMN,
    FROM,
    WHERE,
    AND,
    OR,
    ORDER_BY,
    GROUP_BY,
    ASC,
    DESC,
    LIMIT,
    OFFSET,
    DISTINCT,
    CLAUSE,
    SET,
    NOT_NULL,
    PRIMARY_KEY
  };

public:
  virtual ~dialect() {}

  std::string token(t_token tok) const { return tokens.at(tok); }

private:
  typedef std::unordered_map<t_token, std::string, std::hash<int>> t_token_map;
  const t_token_map tokens {
    {dialect::CREATE_TABLE, "CREATE TABLE"},
    {dialect::DROP, "DROP TABLE"},
    {dialect::DELETE, "DELETE FROM"},
    {dialect::INSERT, "INSERT INTO"},
    {dialect::VALUES, "VALUES"},
    {dialect::UPDATE, "UPDATE"},
    {dialect::SELECT, "SELECT"},
    {dialect::COLUMNS, "COLUMNS"},
    {dialect::COLUMN, "COLUMN"},
    {dialect::FROM, "FROM"},
    {dialect::WHERE, "WHERE"},
    {dialect::AND, "AND"},
    {dialect::OR, "OR"},
    {dialect::ORDER_BY, "ORDER BY"},
    {dialect::GROUP_BY, "GROUP BY"},
    {dialect::ASC, "ASC"},
    {dialect::DESC, "DESC"},
    {dialect::LIMIT, "LIMIT"},
    {dialect::OFFSET, "OFFSET"},
    {dialect::DISTINCT, "DISTINCT"},
    {dialect::SET, "SET"},
    {dialect::NOT_NULL, "NOT NULL"},
    {dialect::PRIMARY_KEY, "PRIMARY KEY"}
  };
};

}
#endif //OOS_BASIC_DIALECT_HPP
