#ifndef MATADOR_BLOG_HPP
#define MATADOR_BLOG_HPP

#include <matador/utils/time.hpp>
#include <matador/utils/base_class.hpp>

#include <matador/object/has_many.hpp>

namespace blog_detail {

struct person
{
  unsigned long id{};
  std::string name;
  matador::date birthday;

  person() = default;

  person(std::string n, matador::date bd)
    : name(std::move(n)), birthday(std::move(bd))
  {}

  template < class Operator >
  void process(Operator &op)
  {
    matador::access::primary_key(op, "id", id);
    matador::access::attribute(op, "name", name, 255);
    matador::access::attribute(op, "birthday", birthday);
  }
};
}
struct post;

struct author : public blog_detail::person
{
  // many to one
  matador::has_many<post> posts;

  author() = default;
  author(const std::string &n, const matador::date &birthday)
    : person(n, birthday)
  {}

  template < class Operator >
  void process(Operator &op)
  {
    matador::access::process(op, *matador::base_class<person>(this));
    matador::access::has_many_(op, "post", posts, "author", "id", matador::cascade_type::NONE);
  }
};

struct category
{
  unsigned long id{};
  std::string name;
  std::string description;
  matador::has_many<post> posts;

  category() = default;
  explicit category(const std::string &n) : category(n, "") {}
  category(std::string n, std::string desc)
    : name(std::move(n)), description(std::move(desc))
  {}

  template < class Operator >
  void process(Operator &op)
  {
    matador::access::primary_key(op, "id", id);
    matador::access::attribute(op, "name", name, 255);
    matador::access::attribute(op, "description", description);
    matador::access::has_many_(op, "post_category", posts, "category_id", "post_id", matador::cascade_type::NONE);
  }
};

struct comment
{
  unsigned long id{};
  std::string email;
  matador::object_ptr<post> blog_post;
  std::string content;
  matador::time created_at;

  comment() = default;
  comment(std::string eml, std::string msg)
    : email(std::move(eml)), content(std::move(msg))
  {}

  template < class Operator >
  void process(Operator &op)
  {
    matador::access::primary_key(op, "id", id);
    matador::access::attribute(op, "email", email, 255);
    matador::access::belongs_to(op, "post", blog_post, matador::cascade_type::NONE);
    matador::access::attribute(op, "content", content);
    matador::access::attribute(op, "created_at", created_at);
  }
};

struct post
{
  unsigned long id{};
  std::string title;
  matador::object_ptr<author> writer;
  matador::time created_at;
  matador::has_many<category> categories;
  matador::has_many<comment> comments;
//  matador::has_many<matador::varchar<255>> tags;
  matador::has_many<std::string> tags;
  // Todo: Implement new behavior for varchar:
//  matador::has_many<std::string, 255> tags;
  std::string content;

  post() = default;

  post(std::string ttle,
       const matador::object_ptr<author> &autr,
       std::string cntnt
  )
    : title(std::move(ttle)), writer(autr), content(std::move(cntnt))
  {}

  post(std::string ttle,
       const matador::object_ptr<author> &autr,
       const matador::object_ptr<category> &cat,
       std::string cntnt
  )
    : title(std::move(ttle)), writer(autr), content(std::move(cntnt))
  {
    categories.push_back(cat);
  }

  template < class Operator >
  void process(Operator &op)
  {
    matador::access::primary_key(op, "id", id);
    matador::access::attribute(op, "title", title, 255);
    matador::access::belongs_to(op, "author", writer, matador::cascade_type::NONE);
    matador::access::attribute(op, "created_at", created_at);
    matador::access::has_many_(op, "post_category", categories, "category_id", "post_id", matador::cascade_type::INSERT);
    matador::access::has_many_(op, "comment", comments, "post", "id", matador::cascade_type::ALL);
    matador::access::has_many_(op, "post_tag", tags, "post_id", "tag", matador::cascade_type::ALL);
    matador::access::attribute(op, "content", content);
  }
};

#endif //MATADOR_BLOG_HPP
