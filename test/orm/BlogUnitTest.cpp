//
// Created by sascha on 22.12.17.
//

#include "BlogUnitTest.hpp"

#include <matador/utils/identifier.hpp>
#include <matador/utils/time.hpp>
#include <matador/utils/base_class.hpp>

#include <matador/object/object_store.hpp>

#include <matador/orm/persistence.hpp>
#include <matador/orm/session.hpp>

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
    serializer.serialize("post", posts, "post", "id");
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
  category(std::string n, std::string desc) : name(std::move(n)), description(std::move(desc)) {}

  template < class Serializer >
  void serialize(Serializer &serializer)
  {
    serializer.serialize("id", id);
    serializer.serialize("name", name);
    serializer.serialize("description", description);
    serializer.serialize("post_category", posts, "category_id", "post_id");
  }
};

struct comment
{
  matador::identifier<unsigned long> id;
  matador::varchar<255> email;
  matador::belongs_to<post> blog_post;
  std::string content;
  matador::time created_at;

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

  template < class Serializer >
  void serialize(Serializer &serializer)
  {
    serializer.serialize("id", id);
    serializer.serialize("title", title);
    serializer.serialize("author", writer, matador::cascade_type::NONE);
    serializer.serialize("created_at", created_at);
    serializer.serialize("post_category", categories, "category_id", "post_id");
    serializer.serialize("comment", comments, "comment", "id");
    serializer.serialize("post_tag", tags, "post_id", "tag");
    serializer.serialize("content", content);
  }
};

BlogUnitTest::BlogUnitTest(const std::string &prefix, const std::string &dns)
  : unit_test(prefix + "_blog", prefix + " blog unit tests")
  , dns_(dns)
{
  add_test("blog", std::bind(&BlogUnitTest::test_blog, this), "test blog");
}

void BlogUnitTest::test_blog()
{
  matador::persistence p(dns_);

  p.attach<blog_detail::person>("person", matador::object_store::abstract);
  p.attach<author, blog_detail::person>("author");
  p.attach<category>("category");
  p.attach<comment>("comment");
  p.attach<post>("post");

  p.create();

  matador::session s(p);

  matador::transaction tr = s.begin();
  try {
    auto me = s.insert(new author("sascha", matador::date(29, 4, 1972)));
    auto main = s.insert(new category("Main", "Main category"));

    auto first = s.insert(new post("First post", me, "My first post content"));

    s.push_back(first->categories, main);

    tr.commit();
  } catch(std::exception &ex) {
    tr.rollback();
  }

  p.drop();
}
