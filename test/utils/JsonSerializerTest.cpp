#include "JsonSerializerTest.hpp"

#include "matador/utils/json_serializer.hpp"

#include "../dto.hpp"

using namespace matador;

JsonSerializerTest::JsonSerializerTest()
  : unit_test("json_serializer", "json serializer test")
{
  add_test("to_string", [this] { test_to_string(); }, "test serialize object to json string");
}

void JsonSerializerTest::test_to_string()
{
  dto d;
  d.id = "pk11";
  d.name = "saturn";
  d.flag = true;
  d.height = 23;
  d.doubles.assign({13.5, 123.9, 0.732});
  d.bits.assign({true, false, false, true, false});
  d.dimension.height = 200;
  d.dimension.width = 1200;
  d.dimension.length = 750;
  d.values = { 1,2,3,4,5,6,7 };

  d.dimensions.assign({{200, 122, 345}, { 800, 900, 450 }});

  bounding_box bb {100, 200, 300 };
  json_serializer js(json_format::pretty);

  std::string result = js.to_string(d);

  std::cout << "\nresult: \n" << result << "\n";
}
