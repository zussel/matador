#include <catch2/catch_test_macros.hpp>

#include "matador/query/expression/binary_column_expression.hpp"
#include "matador/query/expression/placeholder_expression.hpp"
#include "matador/query/expression/table_column_expression.hpp"
#include "matador/query/expression/value_expression.hpp"
#include "matador/query/expression/expression_visitor.hpp"
#include "matador/query/expression/expression_operators.hpp"
#include "matador/query/column.hpp"
#include "matador/query/placeholder.hpp"

#include <string>
#include <type_traits>
#include <utility>
#include <vector>

using namespace matador::query;

namespace {

template <typename...>
using void_t = void;

template <typename T, typename = void>
struct has_ptr_method : std::false_type {};

template <typename T>
struct has_ptr_method<T, void_t<decltype(std::declval<const T&>().ptr())>> : std::true_type {};

inline const column_expression_ptr& as_ptr_ref(const column_expression_ptr& e) {
    return e;
}

template <typename Expr>
std::enable_if_t<has_ptr_method<Expr>::value, const column_expression_ptr&>
as_ptr_ref(const Expr& e) {
    return e.ptr();
}

template <typename Expr>
const abstract_column_expression& as_expr_ref(const Expr& e) {
    return *as_ptr_ref(e);
}

struct dispatch_probe_visitor final : expression_visitor {
  void visit(const binary_column_expression& /*node*/) override { hits.emplace_back("binary"); }
  void visit(const table_column_expression& /*node*/) override { hits.emplace_back("column"); }
  void visit(const value_expression& /*node*/) override { hits.emplace_back("value"); }
  void visit(const placeholder_expression& /*node*/) override { hits.emplace_back("placeholder"); }

  std::vector<std::string> hits;
};

struct preorder_dump_visitor final : expression_visitor {
  void visit(const binary_column_expression& node) override {
    out.emplace_back("bin");
    node.left_column()->accept(*this);
    out.emplace_back(op_to_string(node.operand()));
    node.right_column()->accept(*this);
  }

  void visit(const table_column_expression& node) override {
    out.emplace_back(std::string("col:") + node.col().column_name());
  }

  void visit(const value_expression& node) override {
    // We only assert this test for int literals below, so handle the int path explicitly.
    // If utils::database_type evolves, this still stays a good smoke test.
    std::string s = "val:";
    bool appended = false;
    std::visit([&](const auto& v) {
      using V = std::decay_t<decltype(v)>;
      if constexpr (std::is_same_v<V, int32_t> || std::is_same_v<V, int>) {
        s += std::to_string(static_cast<long long>(v));
        appended = true;
      }
    }, node.value());
    if (!appended) {
      s += "<non-int>";
    }
    out.emplace_back(std::move(s));
  }

  void visit(const placeholder_expression& /*node*/) override {
    out.emplace_back("ph");
  }

  static std::string op_to_string(binary_expression_operator op) {
    switch (op) {
      case binary_expression_operator::Plus: return "+";
      case binary_expression_operator::Minus: return "-";
      case binary_expression_operator::Multiply: return "*";
      case binary_expression_operator::Divide: return "/";
      case binary_expression_operator::Modulo: return "%";
    }
    return "?";
  }

  std::vector<std::string> out;
};

} // namespace

TEST_CASE("ColumnExpression: accept dispatches to the correct visitor overload", "[query][expression][visitor]") {
  dispatch_probe_visitor v;

  {
    auto n = std::make_unique<placeholder_expression>();
    n->accept(v);
    REQUIRE(v.hits.back() == "placeholder");
  }

  {
    auto n = std::make_unique<value_expression>(int32_t{7});
    n->accept(v);
    REQUIRE(v.hits.back() == "value");
  }

  {
    auto n = std::make_unique<table_column_expression>(column::make_plain("id"));
    n->accept(v);
    REQUIRE(v.hits.back() == "column");
  }

  {
    auto n = std::make_unique<binary_column_expression>(
      std::make_unique<table_column_expression>(column::make_plain("x")),
      binary_expression_operator::Plus,
      std::make_unique<value_expression>(int32_t{1})
    );
    n->accept(v);
    REQUIRE(v.hits.back() == "binary");
  }

  REQUIRE(v.hits.size() == 4);
}

TEST_CASE("ColumnExpression: accessors expose left/right and operand", "[query][expression]") {
  auto left = std::make_unique<table_column_expression>(column::make_plain("x"));
  auto right = std::make_unique<value_expression>(int32_t{123});

  auto expr = binary_column_expression(std::move(left), binary_expression_operator::Minus, std::move(right));

  REQUIRE(expr.operand() == binary_expression_operator::Minus);
  REQUIRE(expr.left_column() != nullptr);
  REQUIRE(expr.right_column() != nullptr);

  dispatch_probe_visitor v;
  expr.left_column()->accept(v);
  expr.right_column()->accept(v);

  REQUIRE(v.hits.size() == 2);
  REQUIRE(v.hits[0] == "column");
  REQUIRE(v.hits[1] == "value");
}

TEST_CASE("ColumnExpression: build AST for column +/- value and column +/- placeholder", "[query][expression][operators]") {
  using namespace matador::utils;

  SECTION("column + int") {
    auto e = "next_id"_col + 1;
    const auto& root = dynamic_cast<const binary_column_expression&>(as_expr_ref(e));

    REQUIRE(root.operand() == binary_expression_operator::Plus);
    REQUIRE(dynamic_cast<const table_column_expression*>(root.left_column().get()) != nullptr);
    REQUIRE(dynamic_cast<const value_expression*>(root.right_column().get()) != nullptr);
  }

  SECTION("column - placeholder") {
    auto e = ("x"_col - _);
    const auto& root = dynamic_cast<const binary_column_expression&>(as_expr_ref(e));

    REQUIRE(root.operand() == binary_expression_operator::Minus);
    REQUIRE(dynamic_cast<const table_column_expression*>(root.left_column().get()) != nullptr);
    REQUIRE(dynamic_cast<const placeholder_expression*>(root.right_column().get()) != nullptr);
  }

  SECTION("placeholder + column") {
    auto e = (_ + "x"_col);
    const auto& root = dynamic_cast<const binary_column_expression&>(as_expr_ref(e));

    REQUIRE(root.operand() == binary_expression_operator::Plus);
    REQUIRE(dynamic_cast<const placeholder_expression*>(root.left_column().get()) != nullptr);
    REQUIRE(dynamic_cast<const table_column_expression*>(root.right_column().get()) != nullptr);
  }
}

TEST_CASE("ColumnExpression: can recursively traverse a nested arithmetic expression", "[query][expression][visitor]") {
  using namespace matador::utils;

  // (x + 1) * (y - ?)
  auto e = ("x"_col + 1) * ("y"_col - _);

  const auto& root = dynamic_cast<const binary_column_expression&>(as_expr_ref(e));
  REQUIRE(root.operand() == binary_expression_operator::Multiply);

  preorder_dump_visitor v;
  root.accept(v);

  // Expected preorder-ish dump:
  // bin, (left subtree), *, (right subtree)
  //
  // left: bin col:x + val:1
  // right: bin col:y - ph
  const std::vector<std::string> expected = {
    "bin",
      "bin", "col:x", "+", "val:1",
      "*",
      "bin", "col:y", "-", "ph"
  };

  REQUIRE(v.out == expected);
}
