#include "JsonSerializerTest.hpp"

#include "matador/utils/json_mapper.hpp"

#include "../dto.hpp"

using namespace matador;

JsonSerializerTest::JsonSerializerTest()
  : unit_test("json_serializer", "json serializer test")
{
  add_test("time_to_string_object", [this] { test_date_to_string_object(); }, "test serialize data to json string");
  add_test("to_string_object", [this] { test_to_string_object(); }, "test serialize object to json string");
  add_test("to_string_array", [this] { test_to_string_array(); }, "test serialize array to json string");
}

std::string expected_dto_result = R"({
  "id": "pk11",
  "name": "saturn",
  "birthday": "2001-07-13",
  "created": "1998-09-25T11:55:46",
  "flag": true,
  "height": 23,
  "doubles": [13.500000,123.900000,0.732000],
  "bits": [true,false,false,true,false],
  "names": ["blue","green","red","yellow"],
)"
#ifdef _WIN32
R"(  "values": [1,2,3,4,5,6,7],
)"
#else
R"(  "values": [7,6,5,4,3,2,1],
)"
#endif
R"(  "dimension": {
    "length": 750,
    "width": 1200,
    "height": 200
  },
  "dimensions": [{
    "length": 200,
    "width": 122,
    "height": 345
  },{
    "length": 800,
    "width": 900,
    "height": 450
  }]
})";

std::string expected_bb_result = R"({
  "length": 100,
  "width": 200,
  "height": 300
}
)";

std::string expected_derived_result = R"({
  "id": 8,
  "name": "james"
}
)";

struct date_object
{
  matador::time t;

  template < class S >
  void serialize(S &serializer)
  {
    serializer.serialize("time", t);
  }
};

void JsonSerializerTest::test_date_to_string_object()
{
  date_object dao;
  dao.t.set(1998, 9, 25, 11, 55, 46, 123);

  json_mapper mapper;

  std::string result = mapper.to_string(dao);

  UNIT_ASSERT_EQUAL(R"({"time": "1998-09-25T11:55:46"})", result);
}

void JsonSerializerTest::test_to_string_object()
{
  dto d;
  d.id = "pk11";
  d.name = "saturn";
  d.flag = true;
  d.height = 23;
  d.birthday.set(13, 7, 2001);
  d.created.set(1998, 9, 25, 11, 55, 46, 123);
  d.doubles.assign({13.5, 123.9, 0.732});
  d.bits.assign({true, false, false, true, false});
  d.names = { "green", "red", "yellow", "blue" };
  d.dimension.height = 200;
  d.dimension.width = 1200;
  d.dimension.length = 750;
  d.values = { 1,2,3,4,5,6,7 };

  d.dimensions.assign({{200, 122, 345}, { 800, 900, 450 }});

  json_mapper mapper;

  std::string result = mapper.to_string(d, json_format::pretty);

  UNIT_ASSERT_EQUAL(expected_dto_result + "\n", result);

  bounding_box bb {100, 200, 300 };

  result = mapper.to_string(bb, json_format::pretty);

  UNIT_ASSERT_EQUAL(expected_bb_result, result);

  derived de;
  de.id = 8;
  de.name = "james";

  result = mapper.to_string(de, json_format::pretty);

  UNIT_ASSERT_EQUAL(expected_derived_result, result);
}

void JsonSerializerTest::test_to_string_array()
{
  dto d;
  d.id = "pk11";
  d.name = "saturn";
  d.flag = true;
  d.height = 23;
  d.birthday.set(13, 7, 2001);
  d.created.set(1998, 9, 25, 11, 55, 46, 123);
  d.doubles.assign({13.5, 123.9, 0.732});
  d.bits.assign({true, false, false, true, false});
  d.names = { "green", "red", "yellow", "blue" };
  d.dimension.height = 200;
  d.dimension.width = 1200;
  d.dimension.length = 750;
  d.values = { 1,2,3,4,5,6,7 };

  d.dimensions.assign({{200, 122, 345}, { 800, 900, 450 }});

  std::vector<dto> dtos { d, d };

  json_mapper mapper;

  std::string result = mapper.to_string(dtos, json_format::pretty);

  auto expected_dto_array_result = "[" + expected_dto_result + "," + expected_dto_result + "]\n";
  UNIT_ASSERT_EQUAL(expected_dto_array_result, result);

}
