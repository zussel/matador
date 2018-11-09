//
// Created by sascha on 11.01.18.
//

#ifndef MATADOR_BLOG_HPP
#define MATADOR_BLOG_HPP

#include <matador/utils/identifier.hpp>
#include <matador/utils/time.hpp>
#include <matador/utils/base_class.hpp>

#include <matador/object/has_many.hpp>

namespace blog_detail {

struct person
{
  matador::identifier<unsigned long> id;
  matador::varchar<255> name;
  matador::date birthday;

  person() = default;

  person(const std::string &n, matador::date bd)
    : name(n), birthday(std::move(bd))
  {}

  template<class Serializer>
  void serialize(Serializer &serializer)
  {
    serializer.serialize("id", id);
    serializer.serialize("name", name);
    serializer.serialize("birthday", birthday);
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

  template < class Serializer >
  void serialize(Serializer &serializer)
  {
    serializer.serialize(*matador::base_class<person>(this));
    serializer.serialize("post", posts, "author", "id", matador::cascade_type::NONE);
  }
};

struct category
{
  matador::identifier<unsigned long> id;
  matador::varchar<255> name;
  std::string description;
  matador::has_many<post> posts;

  category() = default;
  explicit category(const std::string &n) : category(n, "") {}
  category(std::string n, std::string desc) : name(n), description(std::move(desc)) {}

  template < class Serializer >
  void serialize(Serializer &serializer)
  {
    serializer.serialize("id", id);
    serializer.serialize("name", name);
    serializer.serialize("description", description);
    serializer.serialize("post_category", posts, "category_id", "post_id", matador::cascade_type::NONE);
  }
};

struct comment
{
  matador::identifier<unsigned long> id;
  matador::varchar<255> email;
  matador::belongs_to<post> blog_post;
  std::string content;
  matador::time created_at;

  comment() = default;
  comment(const std::string &eml, std::string msg)
    : email(eml), content(std::move(msg))
  {}

  template < class Serializer >
  void serialize(Serializer &serializer)
  {
    serializer.serialize("id", id);
    serializer.serialize("email", email);
    serializer.serialize("post", blog_post, matador::cascade_type::NONE);
    serializer.serialize("content", content);
    serializer.serialize("created_at", created_at);
  }
};

struct post
{
  matador::identifier<unsigned long> id;
  matador::varchar<255> title;
  matador::belongs_to<author> writer;
  matador::time created_at;
  matador::has_many<category> categories;
  matador::has_many<comment> comments;
  matador::has_many<matador::varchar<255>> tags;
  std::string content;

  post() = default;

  post(const std::string &ttle,
       const matador::object_ptr<author> &autr,
       std::string cntnt
  )
    : title(ttle), writer(autr), content(std::move(cntnt))
  {}

  post(const std::string &ttle,
       const matador::object_ptr<author> &autr,
       const matador::object_ptr<category> &cat,
       std::string cntnt
  )
    : title(ttle), writer(autr), content(std::move(cntnt))
  {
    categories.push_back(cat);
  }

  template < class Serializer >
  void serialize(Serializer &serializer)
  {
    serializer.serialize("id", id);
    serializer.serialize("title", title);
    serializer.serialize("author", writer, matador::cascade_type::NONE);
    serializer.serialize("created_at", created_at);
    serializer.serialize("post_category", categories, "category_id", "post_id", matador::cascade_type::INSERT);
    serializer.serialize("comment", comments, "post", "id", matador::cascade_type::ALL);
    serializer.serialize("post_tag", tags, "post_id", "tag", matador::cascade_type::ALL);
    serializer.serialize("content", content);
  }
};

#endif //MATADOR_BLOG_HPP
