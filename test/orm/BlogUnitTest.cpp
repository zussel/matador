#include "BlogUnitTest.hpp"
#include "../Blog.hpp"

#include <matador/orm/session.hpp>

#include <utility>

struct blog_service
{
  explicit blog_service(matador::session &ses)
    : session_(ses)
  {}

  matador::object_ptr<post> add(const std::string &title, std::string content,
                                const matador::object_ptr<author> &writer,
                                const matador::object_ptr<category> &cat)
  {
    auto first = session_.insert(new post(title, writer, std::move(content)));
    first.modify()->categories.push_back(cat);
    session_.flush();
    return first;
  }

  void remove(matador::object_ptr<post> p)
  {
    session_.remove(p);
    session_.flush();
  }

  void add_comment(const std::string &email, std::string msg, matador::object_ptr<post> &pst)
  {
    auto cmt = session_.insert(new comment(email, std::move(msg)));
    pst.modify()->comments.push_back(cmt);
    session_.flush();
  }
  matador::session &session_;
};

BlogUnitTest::BlogUnitTest(const std::string& prefix, std::string  dns)
  : unit_test(prefix + "_blog", prefix + " blog unit tests")
  , dns_(std::move(dns))
{
  add_test("single", [this] { test_blog_single_post(); }, "test single blog post");
  add_test("multiple", [this] { test_blog_multiple_post(); }, "test multiple blog post");
}

void BlogUnitTest::initialize() {
  persistence_ = std::make_unique<matador::persistence>(dns_);

  persistence_->attach_abstract<blog_detail::person>("person");
  persistence_->attach<author, blog_detail::person>("author");
  persistence_->attach<category>("category");
  persistence_->attach<comment>("comment");
  persistence_->attach<post>("post");
}

void BlogUnitTest::finalize() {
  persistence_->drop();
  persistence_.reset();
}

void BlogUnitTest::test_blog_single_post()
{
  UNIT_ASSERT_EQUAL(persistence_->store().size(), 7UL);
  persistence_->create();

  {
    matador::session s(*persistence_);

    blog_service blogger(s);

    matador::object_ptr<author> me;
    matador::object_ptr<category> main;

    me = s.insert(new author("sascha", matador::date(29, 4, 1972)));
    main = s.insert(new category("Main", "Main category"));

    s.flush();

    blogger.add("First post", "My first post content", me, main);

    using t_post_view = matador::object_view<post>;
    t_post_view posts(s.store());

    UNIT_ASSERT_EQUAL(posts.size(), 1UL);

    auto post1 = posts.front();

    auto hi = post1->categories.begin().holder_item();

    UNIT_ASSERT_EQUAL(hi.item_proxy()->reference_count(), 2UL);
    UNIT_ASSERT_EQUAL(posts.size(), 1UL);
    UNIT_ASSERT_EQUAL(post1.reference_count(), 2UL);
    UNIT_ASSERT_EQUAL(me.reference_count(), 1UL);
    UNIT_ASSERT_EQUAL(main.reference_count(), 1UL);
    UNIT_ASSERT_EQUAL(me->posts.size(), 1UL);
    UNIT_ASSERT_EQUAL(main->posts.size(), 1UL);

    blogger.add_comment("uli@mail.de", "cool stuff", post1);

    UNIT_ASSERT_EQUAL(post1->comments.size(), 1UL);
    UNIT_ASSERT_EQUAL(post1.reference_count(), 3UL);
  }

  persistence_->clear();

  {
    matador::session s(*persistence_);

    s.load();

    using t_author_view = matador::object_view<author>;
    t_author_view authors(s.store());

    auto me = authors.front();

    using t_category_view = matador::object_view<category>;
    t_category_view categories(s.store());

    auto main = categories.front();

    UNIT_ASSERT_EQUAL(me->posts.size(), 1UL);
    UNIT_ASSERT_EQUAL(main->posts.size(), 1UL);
    UNIT_ASSERT_EQUAL(main.reference_count(), 1UL);
    UNIT_ASSERT_EQUAL(me.reference_count(), 1UL);

    using t_post_view = matador::object_view<post>;
    t_post_view posts(s.store());

    UNIT_ASSERT_EQUAL(posts.size(), 1UL);

    auto post1 = posts.front();

    auto hi = post1->categories.begin().holder_item();

    UNIT_ASSERT_EQUAL(hi.item_proxy()->reference_count(), 2UL);
    UNIT_ASSERT_EQUAL(posts.size(), 1UL);
    UNIT_ASSERT_EQUAL(post1.reference_count(), 3UL);

//     delete post
    auto i = std::find_if(posts.begin(), posts.end(), [](const matador::object_ptr<post> &p) {
      return p->title == "First post";
    });

    blog_service blogger(s);

    if (i != posts.end()) {
      blogger.remove(*i);
    }

    UNIT_ASSERT_EQUAL(posts.size(), 0UL);

    UNIT_ASSERT_EQUAL(me->posts.size(), 0UL);
    UNIT_ASSERT_EQUAL(main->posts.size(), 0UL);
    UNIT_ASSERT_EQUAL(main.reference_count(), 0UL);
    UNIT_ASSERT_EQUAL(me.reference_count(), 0UL);
  }

  persistence_->drop();
}

void BlogUnitTest::test_blog_multiple_post()
{
  UNIT_ASSERT_EQUAL(persistence_->store().size(), 7UL);
  persistence_->create();
  {
    matador::session s(*persistence_);

    blog_service blogger(s);

    matador::object_ptr<author> me;
    matador::object_ptr<category> main;

    me = s.insert(new author("sascha", matador::date(29, 4, 1972)));
    main = s.insert(new category("Main", "Main category"));

    s.flush();

    blogger.add("First post", "My first post content", me, main);
    blogger.add("Second post", "My second post content", me, main);
    blogger.add("Third post", "My third post content", me, main);
    blogger.add("Fourth post", "My fourth post content", me, main);

    UNIT_ASSERT_EQUAL(me.reference_count(), 4UL);
    UNIT_ASSERT_EQUAL(main.reference_count(), 4UL);
    UNIT_ASSERT_EQUAL(me->posts.size(), 4UL);
    UNIT_ASSERT_EQUAL(main->posts.size(), 4UL);

    using t_post_view = matador::object_view<post>;
    t_post_view posts(s.store());

    UNIT_ASSERT_EQUAL(posts.size(), 4UL);
  }

  persistence_->clear();

  {
    matador::session s(*persistence_);

    s.load();

    blog_service blogger(s);

    using t_author_view = matador::object_view<author>;
    t_author_view authors(s.store());

    UNIT_ASSERT_EQUAL(authors.size(), 1UL);

    auto me = authors.front();

    using t_category_view = matador::object_view<category>;
    t_category_view categories(s.store());

    UNIT_ASSERT_EQUAL(categories.size(), 1UL);

    auto main = categories.front();

    UNIT_ASSERT_EQUAL(me.reference_count(), 4UL);
    UNIT_ASSERT_EQUAL(main.reference_count(), 4UL);
    UNIT_ASSERT_EQUAL(me->posts.size(), 4UL);
    UNIT_ASSERT_EQUAL(main->posts.size(), 4UL);

    using t_post_view = matador::object_view<post>;
    t_post_view posts(s.store());

    UNIT_ASSERT_EQUAL(posts.size(), 4UL);

    // delete third post
    auto i = std::find_if(posts.begin(), posts.end(), [](const matador::object_ptr<post> &p) {
      return p->title == "Third post";
    });

    if (i != posts.end()) {
      blogger.remove(*i);
    }

    UNIT_ASSERT_EQUAL(posts.size(), 3UL);

    UNIT_ASSERT_EQUAL(me.reference_count(), 3UL);
    UNIT_ASSERT_EQUAL(main.reference_count(), 3UL);
    UNIT_ASSERT_EQUAL(me->posts.size(), 3UL);
    UNIT_ASSERT_EQUAL(main->posts.size(), 3UL);
  }
  persistence_->drop();
}
