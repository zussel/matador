#include "TemplateEngineTest.hpp"

#include "matador/utils/json.hpp"

#include "matador/http/template_engine.hpp"

TemplateEngineTest::TemplateEngineTest()
  : matador::unit_test("template_engine", "template engine test")
{
  add_test("vars", [this] { test_replace_var(); }, "test replace variables");
  add_test("foreach", [this] { test_foreach(); }, "test foreach loop");
  add_test("foreach_nested", [this] { test_foreach_nested(); }, "test foreach loop with nested foreach");
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
  std::string simple_foreach { "List [{% for item in list %}Color: {{ item.name }} (Shortcut: {{ item.shortcut}}){% endfor %}]" };

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

  UNIT_ASSERT_EQUAL("List [Color: green (Shortcut: GR)Color: red (Shortcut: RE)]", engine.str());
}

void TemplateEngineTest::test_foreach_nested()
{
  std::string nested_foreach { "<ul>{% for number in numbers %}<li><h2>{{ number.name }}</h2><ol>{% for i in number.items %}<li>{{ i }}</li>{% endfor %}</ol></li>{% endfor %}</ul>" };

  json data {
    { "numbers", { {
      {"name", "first"},
      {"items", { 1, 2, 3, 4, 5, 6, 7 }}
    }, {
      {"name", "second"},
      {"items", { 10, 20, 30, 40, 50, 60 , 70 }}
   } } }
  };

  http::template_engine engine;

  engine.render(nested_foreach, data);

  auto result = engine.str();

  std::cout << "result: " << result << "\n";
}
