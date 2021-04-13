#include "TemplateEngineTest.hpp"

#include "matador/utils/json.hpp"

#include "matador/http/template_engine.hpp"

TemplateEngineTest::TemplateEngineTest()
  : matador::unit_test("template_engine", "template engine test")
{
  add_test("vars", [this] { test_replace_var(); }, "test replace variables");
  add_test("foreach", [this] { test_foreach(); }, "test foreach loop");
  add_test("foreach_nested", [this] { test_foreach_nested(); }, "test foreach loop with nested foreach");
  add_test("foreach_empty", [this] { test_foreach_empty(); }, "test foreach loop with empty foreach data");
  add_test("if_else", [this] { test_if_else(); }, "test if else");
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

  auto result = http::template_engine::render(no_replace, data);

  UNIT_ASSERT_EQUAL(no_replace, result);

  result = http::template_engine::render(replace_one, data);

  UNIT_ASSERT_EQUAL("replace george one", result);

  result = http::template_engine::render(replace_one_cascade, data);

  UNIT_ASSERT_EQUAL("replace jane one", result);
}

void TemplateEngineTest::test_foreach()
{
  std::string simple_foreach { "List [{% for item in list %}Color: {{ item.name }} (Shortcut: {{ item.shortcut}}){% endfor %}]" };
  std::string expected_result = "List [Color: green (Shortcut: GR)Color: red (Shortcut: RE)]";
  json data {
    { "list", { {
      {"name", "green"},
      {"shortcut", "GR"}
    }, {
      {"name", "red"},
      {"shortcut", "RE"}
    } } }
  };

  auto result = http::template_engine::render(simple_foreach, data);

  UNIT_ASSERT_EQUAL(expected_result, result);
}

void TemplateEngineTest::test_foreach_nested()
{
  std::string nested_foreach { "<ul>{% for number in numbers %}<li><h2>{{ number.name }}</h2><ol>{% for i in number.items %}<li>{{ i }}</li>{% endfor %}</ol></li>{% endfor %}</ul>" };
  std::string expected_result = "<ul><li><h2>first</h2><ol><li>1</li><li>2</li><li>3</li><li>4</li><li>5</li><li>6</li><li>7</li></ol></li><li><h2>second</h2><ol><li>10</li><li>20</li><li>30</li><li>40</li><li>50</li><li>60</li><li>70</li></ol></li></ul>";

  json data {
    { "numbers", { {
      {"name", "first"},
      {"items", { 1, 2, 3, 4, 5, 6, 7 }}
    }, {
      {"name", "second"},
      {"items", { 10, 20, 30, 40, 50, 60 , 70 }}
   } } }
  };

  auto result = http::template_engine::render(nested_foreach, data);

  UNIT_ASSERT_EQUAL(expected_result, result);
}

void TemplateEngineTest::test_foreach_empty()
{
  std::string empty_foreach {"List [{% for item in list %}Color: {{ item.name }} (Shortcut: {{ item.shortcut}}){% empty %}Liste leer{% endfor %}]" };
  std::string expected_result = "List [Liste leer]";
  json data = json::object();
  data["list"] = json::array();

  auto result = http::template_engine::render(empty_foreach, data);

  UNIT_ASSERT_EQUAL(expected_result, result);
}

void TemplateEngineTest::test_if_else()
{
  std::string if_else { "<h1>Header</h1>"
                        "{% if person %}<p>Details of {{ person.name }}</p>"
                        "{% else %}<p>No details</p>"};

  std::string expected_result = { "<h1>Header</h1>"
                                  "<p>Details of George</p>" };

  std::string expected_result_no_details = { "<h1>Header</h1>"
                                             "<p>No details</p>" };

  json data {
    { "person", {
      { "id", 4711 },
      { "name", "George" }
    }}
  };

  auto result = http::template_engine::render(if_else, data);

  UNIT_ASSERT_EQUAL(expected_result, result);

  result = http::template_engine::render(if_else, json::object());

  UNIT_ASSERT_EQUAL(expected_result_no_details, result);
}
