//
// Created by sascha on 22.12.17.
//

#include "BlogUnitTest.hpp"
#include "../Blog.hpp"

#include <matador/orm/persistence.hpp>
#include <matador/orm/session.hpp>

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
