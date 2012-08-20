#ifndef JSONIMPORTER_HPP
#define JSONIMPORTER_HPP

#include "json/generic_json_parser.hpp"

namespace oos {
  class session;
  class object;
}

class JsonImporter : public oos::generic_json_parser<JsonImporter>
{
public:
  explicit JsonImporter(oos::session &db);
  virtual ~JsonImporter();

  void parse(std::istream &in);

  void on_begin_object();
  void on_object_key(const std::string &key);
  void on_end_object();

  void on_begin_array();
  void on_end_array();

  void on_string(const std::string &value);
  void on_number(double value);
  void on_bool(bool value);
  void on_null();

private:
  enum parse_step_t {
    unknown = 0,
    artist,
    album,
    track
  };

private:
  oos::session &db_;

  parse_step_t step_;

  std::string key_;

  oos::object *obj_;
};


#endif /* JSONIMPORTER_HPP */
