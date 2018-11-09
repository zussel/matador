#include <utility>

//
// Created by sascha on 22.12.17.
//

#include "BlogUnitTest.hpp"
#include "../Blog.hpp"

#include <matador/orm/persistence.hpp>
#include <matador/orm/session.hpp>

struct blog_service
{
  explicit blog_service(matador::session &ses)
    : session_(ses)
  {}

  matador::object_ptr<post> add(const std::string &title, std::string content,
                                const matador::object_ptr<author> &writer,
                                const matador::object_ptr<category> &cat)
  {
    matador::transaction tr = session_.begin();
    try {
      auto first = session_.insert(new post(title, writer, std::move(content)));

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
      session_.remove(p);

      tr.commit();

      return true;
    } catch (std::exception &ex) {
      tr.rollback();
      return false;
    }
  }

  bool add_comment(const std::string &email, std::string msg, const matador::object_ptr<post> &pst)
  {
    auto tr = session_.begin();
    try {
      auto cmt = session_.insert(new comment(email, std::move(msg)));

      pst->comments.push_back(cmt);

      tr.commit();

      return true;
    } catch(std::exception &ex) {
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
  add_test("blog_single", std::bind(&BlogUnitTest::test_blog_single_post, this), "test single blog post");
  add_test("blog_multiple", std::bind(&BlogUnitTest::test_blog_multiple_post, this), "test multiple blog post");
}

void BlogUnitTest::test_blog_single_post()
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

    blog_service blogger(s);

    matador::transaction tr = s.begin();

    matador::object_ptr<author> me;
    matador::object_ptr<category> main;
    try {
      me = s.insert(new author("sascha", matador::date(29, 4, 1972)));
      main = s.insert(new category("Main", "Main category"));

      blogger.add("First post", "My first post content", me, main);

      tr.commit();
    } catch (std::exception &ex) {
      tr.rollback();
      UNIT_FAIL("failed on commit blog post");
    }

    using t_post_view = matador::object_view<post>;
    t_post_view posts(s.store());

    UNIT_ASSERT_EQUAL(posts.size(), 1UL, "size must be three");

    auto post1 = posts.front();

    auto hi = post1->categories.begin().holder_item();

    UNIT_ASSERT_EQUAL(hi.item_proxy()->reference_count(), 2UL, "ref count must be two");
    UNIT_ASSERT_EQUAL(posts.size(), 1UL, "size must be three");
    UNIT_ASSERT_EQUAL(post1.reference_count(), 2UL, "ref count must be two");
    UNIT_ASSERT_EQUAL(me.reference_count(), 1UL, "ref count must be one");
    UNIT_ASSERT_EQUAL(main.reference_count(), 1UL, "ref count must be one");
    UNIT_ASSERT_EQUAL(me->posts.size(), 1UL, "size must be one");
    UNIT_ASSERT_EQUAL(main->posts.size(), 1UL, "size must be one");

    blogger.add_comment("uli@mail.de", "cool stuff", post1);

    UNIT_ASSERT_EQUAL(post1->comments.size(), 1UL, "size must be one");
  }

  p.clear();

  {
    matador::session s(p);

    s.load();

    using t_author_view = matador::object_view<author>;
    t_author_view authors(s.store());

    auto me = authors.front();

    using t_category_view = matador::object_view<category>;
    t_category_view categories(s.store());

    auto main = categories.front();

    UNIT_ASSERT_EQUAL(me->posts.size(), 1UL, "size must be one");
    UNIT_ASSERT_EQUAL(main->posts.size(), 1UL, "size must be one");
    UNIT_ASSERT_EQUAL(main.reference_count(), 1UL, "ref count must be one");
    UNIT_ASSERT_EQUAL(me.reference_count(), 1UL, "ref count must be one");

    using t_post_view = matador::object_view<post>;
    t_post_view posts(s.store());

    UNIT_ASSERT_EQUAL(posts.size(), 1UL, "size must be three");

    auto post1 = posts.front();

    auto hi = post1->categories.begin().holder_item();

    UNIT_ASSERT_EQUAL(hi.item_proxy()->reference_count(), 2UL, "ref count must be two");
    UNIT_ASSERT_EQUAL(posts.size(), 1UL, "size must be three");
    UNIT_ASSERT_EQUAL(post1.reference_count(), 2UL, "ref count must be two");

//     delete post
    auto i = std::find_if(posts.begin(), posts.end(), [](const matador::object_ptr<post> &p) {
      return p->title == "First post";
    });

    blog_service blogger(s);

    if (i != posts.end()) {
      blogger.remove(*i);
    }

    UNIT_ASSERT_EQUAL(posts.size(), 1UL, "size must be one");

    UNIT_ASSERT_EQUAL(me->posts.size(), 0UL, "size must be zero");
    UNIT_ASSERT_EQUAL(main->posts.size(), 1UL, "size must be zero");
    UNIT_ASSERT_EQUAL(main.reference_count(), 1UL, "ref count must be zero");
    UNIT_ASSERT_EQUAL(me.reference_count(), 1UL, "ref count must be zero");
  }

  p.drop();
}

void BlogUnitTest::test_blog_multiple_post()
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

    blog_service blogger(s);

    matador::object_ptr<author> me;
    matador::object_ptr<category> main;

    matador::transaction tr = s.begin();
    try {
      me = s.insert(new author("sascha", matador::date(29, 4, 1972)));
      main = s.insert(new category("Main", "Main category"));

      blogger.add("First post", "My first post content", me, main);
      blogger.add("Second post", "My second post content", me, main);
      blogger.add("Third post", "My third post content", me, main);
      blogger.add("Fourth post", "My fourth post content", me, main);

      tr.commit();
    } catch (std::exception &ex) {
      tr.rollback();
    }

    UNIT_ASSERT_EQUAL(me.reference_count(), 4UL, "ref count must be four");
    UNIT_ASSERT_EQUAL(main.reference_count(), 4UL, "ref count must be four");
    UNIT_ASSERT_EQUAL(me->posts.size(), 4UL, "size must be four");
    UNIT_ASSERT_EQUAL(main->posts.size(), 4UL, "size must be four");

    using t_post_view = matador::object_view<post>;
    t_post_view posts(s.store());

    UNIT_ASSERT_EQUAL(posts.size(), 4UL, "size must be three");
  }

  p.clear();

  {
    matador::session s(p);

    s.load();

    blog_service blogger(s);

    using t_author_view = matador::object_view<author>;
    t_author_view authors(s.store());

    UNIT_ASSERT_EQUAL(authors.size(), 1UL, "size must be one");

    auto me = authors.front();

    using t_category_view = matador::object_view<category>;
    t_category_view categories(s.store());

    UNIT_ASSERT_EQUAL(categories.size(), 1UL, "size must be one");

    auto main = categories.front();

    UNIT_ASSERT_EQUAL(me.reference_count(), 4UL, "ref count must be four");
    UNIT_ASSERT_EQUAL(main.reference_count(), 4UL, "ref count must be four");
    UNIT_ASSERT_EQUAL(me->posts.size(), 4UL, "size must be four");
    UNIT_ASSERT_EQUAL(main->posts.size(), 4UL, "size must be four");

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

    UNIT_ASSERT_EQUAL(me.reference_count(), 3UL, "ref count must be three");
    UNIT_ASSERT_EQUAL(main.reference_count(), 3UL, "ref count must be three");
    UNIT_ASSERT_EQUAL(me->posts.size(), 3UL, "size must be three");
    UNIT_ASSERT_EQUAL(main->posts.size(), 3UL, "size must be three");
  }
  p.drop();
}
