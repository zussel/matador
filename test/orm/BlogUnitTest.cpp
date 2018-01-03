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

struct post_service
{
  explicit post_service(matador::session &ses)
    : session_(ses)
  {}

  matador::object_ptr<post> add(std::string title, std::string content,
                                const matador::object_ptr<author> &writer, const matador::object_ptr<category> &cat)
  {
    matador::transaction tr = session_.begin();
    try {
      auto first = session_.insert(new post(title, writer, content));

      session_.push_back(first->categories, cat);

      tr.commit();

      return first;
    } catch (std::exception &ex) {
      tr.rollback();
      return matador::object_ptr<post>();
    }
  }

  bool remove(matador::object_ptr<post> p)
  {
    matador::transaction tr = session_.begin();
    try {
      session_.clear(p->categories);
      session_.clear(p->comments);
      session_.clear(p->tags);
      session_.remove(p);

      tr.commit();

      return true;
    } catch (std::exception &ex) {
      tr.rollback();
      return false;
    }
  }

  matador::session &session_;
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

  UNIT_ASSERT_EQUAL(p.store().size(), 7UL, "there must be seven nodes in store");
  p.create();
  {
    matador::session s(p);

    post_service blogger(s);

    matador::transaction tr = s.begin();
    try {
      auto me = s.insert(new author("sascha", matador::date(29, 4, 1972)));
      auto main = s.insert(new category("Main", "Main category"));

      blogger.add("First post", "My first post content", me, main);
      blogger.add("Second post", "My second post content", me, main);
      blogger.add("Third post", "My third post content", me, main);
      blogger.add("Fourth post", "My fourth post content", me, main);

      tr.commit();
    } catch (std::exception &ex) {
      tr.rollback();
    }

    using t_post_view = matador::object_view<post>;
    t_post_view posts(s.store());

    UNIT_ASSERT_EQUAL(posts.size(), 4UL, "size must be three");
  }

  p.clear();

  {
    matador::session s(p);

    s.load();

    post_service blogger(s);

    using t_post_view = matador::object_view<post>;
    t_post_view posts(s.store());

    UNIT_ASSERT_EQUAL(posts.size(), 4UL, "size must be three");

    // delete third post
    auto i = std::find_if(posts.begin(), posts.end(), [](const matador::object_ptr<post> &p) {
      return p->title == "Third post";
    });

    if (i != posts.end()) {
      blogger.remove(*i);
    }

    UNIT_ASSERT_EQUAL(posts.size(), 3UL, "size must be three");
  }
  p.drop();
}
