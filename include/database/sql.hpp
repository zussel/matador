#ifndef SQL_HPP
#define SQL_HPP

#include <string>
#include <map>
#include <list>
#include <vector>
#include <memory>
#include <stdexcept>

namespace oos {

class sql
{
public:
  enum data_type_t {
    type_char = 0,
    type_short,
    type_int,
    type_long,
    type_unsigned_char,
    type_unsigned_short,
    type_unsigned_int,
    type_unsigned_long,
    type_float,
    type_double,
    type_bool,
    type_char_pointer,
    type_varchar,
    type_text,
    type_blob,
    type_unknown
  };

  struct field
  {
    field(const char *n, data_type_t t, unsigned int i, bool h)
      : name(n), type(t), index(i), is_host(h)
    {}
    std::string name;
    data_type_t type;
    unsigned int index;
    bool is_host;
  };

  typedef std::shared_ptr<field> field_ptr;

private:
  struct token {
    virtual ~token() {}
    
    virtual std::string get(bool) const = 0;
  };
  
  struct string_token : public token {
    string_token() {}
    string_token(const std::string &s) : str(s) {}

    virtual std::string get(bool) const {
      return str;
    }

    std::string str;
  };
  
  struct result_field_token : public token {
    result_field_token(field_ptr f) : fld(f) {}

    virtual std::string get(bool prepared) const {
      if (prepared) {
        return fld->name;
      } else {
        return fld->name;
      }
    }

    field_ptr fld;
  };

  struct host_field_token : public string_token {
    host_field_token(field_ptr f) : fld(f) {}
    host_field_token(field_ptr f, const std::string &s)
      : string_token(s), fld(f)
    {}

    virtual std::string get(bool prepared) const {
      if (prepared) {
        return std::string("?");
      } else {
        return str;
      }
    }

    field_ptr fld;
  };

public:
  typedef std::vector<field_ptr> field_vector_t;
  typedef field_vector_t::size_type size_type;
  typedef field_vector_t::iterator iterator;
  typedef field_vector_t::const_iterator const_iterator;

  typedef std::map<std::string, field_ptr> field_map_t;
  
  typedef std::list<token*> token_list_t;
  
public:
  ~sql();
  
  void append(const std::string &str);
  void append(const char *id, data_type_t type);
  void append(const char *id, data_type_t type, const std::string &val);

  std::string prepare() const;
  std::string direct() const;

  void reset();

  iterator result_begin();
  iterator result_end();
  const_iterator result_begin() const;
  const_iterator result_end() const;
  size_type result_size() const;

  iterator host_begin();
  iterator host_end();
  const_iterator host_begin() const;
  const_iterator host_end() const;
  size_type host_size() const;

  static unsigned int type_size(data_type_t type);

private:
  std::string generate(bool prepared) const;

private:
  field_vector_t host_field_vector_;
  field_map_t host_field_map_;
  field_vector_t result_field_vector_;
  field_map_t result_field_map_;
  
  token_list_t token_list_;
};

}

#endif /* SQL_HPP */
