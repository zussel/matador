#include "TemplateEngineTest.hpp"

#include "matador/utils/json.hpp"
#include "matador/utils/file.hpp"
#include "matador/utils/os.hpp"

#include "matador/http/template_engine.hpp"

TemplateEngineTest::TemplateEngineTest()
  : matador::unit_test("template_engine", "template engine test")
{
  add_test("vars", [this] { test_replace_var(); }, "test replace variables");
  add_test("foreach", [this] { test_foreach(); }, "test foreach loop");
  add_test("foreach_nested", [this] { test_foreach_nested(); }, "test foreach loop with nested foreach");
  add_test("foreach_empty", [this] { test_foreach_empty(); }, "test foreach loop with empty foreach data");
  add_test("if_else", [this] { test_if_else(); }, "test if else");
  add_test("include", [this] { test_include(); }, "test include");
  add_test("filter", [this] { test_filter(); }, "test filter");
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
                        "{% else %}<p>No details</p>{% endif %}"};

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

  std::string if_compare_else { "<h1>Header</h1>"
                        "{% if person.name == \"George\" %}<p>Details of {{ person.name }}</p>"
                        "{% else %}<p>No details</p>{% endif %}" };

  result = http::template_engine::render(if_compare_else, data);

  UNIT_ASSERT_EQUAL(expected_result, result);

  data = {
    { "persons", { {
      {"name", "George"},
      {"id", 2 }
    }, {
      {"name", "Jane"},
      {"id",  1 }
    }, {
      {"name", "Otto"},
      {"id", 3 }
    } } }
  };

  std::string if_elif { "<h1>Header</h1>{% for person in persons %}"
                        "{% if person.id == 2 %}<p>1 Details of {{ person.name }}</p>"
                        "{% elif person.id < 2 %}<p>2 Details of {{ person.name }}</p>"
                        "{% elif person.id >= 3 %}<p>3 Details of {{ person.name }}</p>"
                        "{% else %}<p>No details</p>{% endif %}{% endfor %}"};

  std::string expected_result_elif = { "<h1>Header</h1>"
                                  "<p>1 Details of George</p>"
                                  "<p>2 Details of Jane</p>"
                                  "<p>3 Details of Otto</p>"
  };

  result = http::template_engine::render(if_elif, data);

  UNIT_ASSERT_EQUAL(expected_result_elif, result);

  std::string if_json_equal_json { "<h1>Header</h1>"
                                   "{% if person.name == current.name %}<p>Details of {{ person.name }}</p>"
                                   "{% else %}<p>No details</p>{% endif %}" };

  data = {
    { "person", {
      { "id", 4711 },
      { "name", "George" }
    }},
    { "current", {
      { "name", "George"}
    }}
  };

  result = http::template_engine::render(if_json_equal_json, data);

  UNIT_ASSERT_EQUAL(expected_result, result);

  data["current"]["name"] = "Jane";

  result = http::template_engine::render(if_json_equal_json, data);

  UNIT_ASSERT_EQUAL(expected_result_no_details, result);
}

void TemplateEngineTest::test_include()
{
  const std::string filename { "header.html" };

  file f(filename, "w");

  UNIT_ASSERT_TRUE(matador::os::exists(filename));

  const std::string content { "<title>{{ title }}</title>" };
  ::fwrite(content.c_str(), sizeof(char), content.size(), f.stream());
  f.close();

  std::string incl { R"(
<html>
<head>
  {% include "header.html" %}
</head>
<body>
<h1>Hello {{ user.name }}</h1>
</body>
</html>)" };

  std::string expected_result { R"(
<html>
<head>
  <title>My first Page</title>
</head>
<body>
<h1>Hello George</h1>
</body>
</html>)" };

  json data {
    { "user", {
      { "id", 4711 },
      { "name", "George" }
    }},
    { "title", "My first Page"}
  };

  auto result = http::template_engine::render(incl, data);

  UNIT_ASSERT_EQUAL(expected_result, result);

  matador::os::remove(filename);
}

void TemplateEngineTest::test_filter()
{
  std::string replace_with_filter { "replace {{ name|escape   }} one" };

  json data {
    { "name", "george" }
  };

  auto result = http::template_engine::render(replace_with_filter, data);

  UNIT_ASSERT_EQUAL("replace george one", result);

  data = {
    { "formula", "1 < x && 3 > x" }
  };

  std::string replace_with_escape_filter { "replace {{ formula | escape   }} one" };

  result = http::template_engine::render(replace_with_escape_filter, data);

  UNIT_ASSERT_EQUAL("replace 1 &lt; x &amp;&amp; 3 &gt; x one", result);
}
