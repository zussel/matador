#include "TemplateEngineTest.hpp"

#include "matador/utils/json.hpp"

#include "matador/http/template_engine.hpp"

TemplateEngineTest::TemplateEngineTest()
  : matador::unit_test("template_engine", "template engine test")
{
  add_test("vars", [this] { test_replace_var(); }, "test replace variables");
  add_test("foreach", [this] { test_foreach(); }, "test foreach loop");
}

using namespace matador;

void TemplateEngineTest::test_replace_var()
{
  std::string no_replace { "no_replace" };
  std::string replace_one { "replace {{ name   }} one" };
  std::string replace_one_cascade { "replace {{ person.name   }} one" };

  json data {
    { "name", "george" },
    { "list", { 1,2,3,4,5 }},
    { "person", {
      { "name", "jane" },
      { "id", 7 }
    }}
  };

  http::template_engine engine;

  engine.render(no_replace, data);

  UNIT_ASSERT_EQUAL(no_replace, engine.str());

  engine.render(replace_one, data);

  UNIT_ASSERT_EQUAL("replace george one", engine.str());

  engine.render(replace_one_cascade, data);

  UNIT_ASSERT_EQUAL("replace jane one", engine.str());
}

void TemplateEngineTest::test_foreach()
{
  std::string simple_foreach { "List [{% for item in list %}Color: {{ item.name }} (Shortcut: {{ item.shortcut}}){% endfor &}]" };

  json data {
    { "list", { {
      {"name", "green"},
      {"shortcut", "GR"}
    }, {
      {"name", "red"},
      {"shortcut", "RE"}
    } } }
  };

  http::template_engine engine;

  engine.render(simple_foreach, data);

  auto result = engine.str();

  std::cout << "Result: " << result << "\n";
}
