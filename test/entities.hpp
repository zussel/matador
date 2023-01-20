#ifndef ENTITIES_HPP
#define ENTITIES_HPP

#include "matador/utils/base_class.hpp"
#include "matador/object/object_ptr.hpp"
#include "matador/object/has_many.hpp"

#include "matador/utils/time.hpp"
#include "matador/utils/date.hpp"

#include <ostream>
#include <utility>

#include "datatypes.hpp"
#include "person.hpp"

template < class T >
class ObjectItem : public datatypes
{
public:
    typedef matador::object_ptr<T> value_ptr;

    ObjectItem() = default;
    ObjectItem(const std::string &n, int i)
            : datatypes(n, i)
    {}

    template < class SERIALIZER >
    void serialize(SERIALIZER &serializer)
    {
        serializer.serialize(*matador::base_class<datatypes>(this));
        serializer.on_has_one("ref", ref_, matador::cascade_type::NONE);
        serializer.on_has_one("ptr", ptr_, matador::cascade_type::ALL);
    }

    void ref(const value_ptr &r)
    {
        ref_ = r;
    }
    void ptr(const value_ptr &p) { ptr_ = p; }

    value_ptr ref() const { return ref_; }
    value_ptr ptr() const { return ptr_; }

    template < class I >
    friend std::ostream& operator <<(std::ostream &os, const ObjectItem<I> &i)
    {
        os << "ObjectItem [" << i.get_string() << "] (" << i.get_int() << ")";
        return os;
    }

private:
    matador::object_ptr<T> ref_;
    matador::object_ptr<T> ptr_;
};

class ObjectItemList
{
public:
    typedef matador::has_many<ObjectItem<datatypes>> object_item_list_t;
    typedef typename object_item_list_t::iterator iterator;
    typedef typename object_item_list_t::const_iterator const_iterator;

    unsigned long id{};
    std::string name;
    object_item_list_t items;

    ObjectItemList() = default;
    explicit ObjectItemList(std::string n) : name(std::move(n)) {}

    template < class S >
    void serialize(S &s)
    {
        s.on_primary_key("id", id);
        s.on_attribute("name", name);
        s.on_has_many("object_item_list", items, "list_id", "object_item_id", matador::cascade_type::NONE);
    }

    iterator begin() { return items.begin(); }
    iterator end() { return items.end(); }

    const_iterator begin() const { return items.begin(); }
    const_iterator end() const { return items.end(); }

};

class book
{
private:
    unsigned long id_{};
    std::string title_;
    std::string isbn_;
    std::string author_;

public:
    book() = default;
    book(std::string title, std::string isbn, std::string author)
            : title_(std::move(title))
            , isbn_(std::move(isbn))
            , author_(std::move(author))
    {}
    ~book() = default;

    template < class SERIALIZER >
    void serialize(SERIALIZER &serializer)
    {
        serializer.on_primary_key("id", id_);
        serializer.on_attribute("title", title_);
        serializer.on_attribute("isbn", isbn_);
        serializer.on_attribute("author", author_);
    }

    unsigned long id() const { return id_; }
    void id(unsigned long i)  { id_ = i; }
    std::string title() const { return title_; }
    std::string isbn() const { return isbn_; }
    std::string author() const { return author_; }
};

class book_list
{
public:
    typedef matador::has_many<book> book_list_t;
    typedef book_list_t::size_type size_type;
    typedef book_list_t::iterator iterator;
    typedef book_list_t::const_iterator const_iterator;

    book_list() = default;
    ~book_list() = default;

    template < class SERIALIZER >
    void serialize(SERIALIZER &serializer)
    {
        serializer.on_primary_key("id", id_);
        serializer.on_has_many("books", book_list_, "book_list_id", "book_id", matador::cascade_type::NONE);
    }

    void add(const matador::object_ptr<book> &b)
    {
        book_list_.push_back(b);
    }

    iterator begin() { return book_list_.begin(); }
    const_iterator begin() const { return book_list_.begin(); }

    iterator end() { return book_list_.end(); }
    const_iterator end() const { return book_list_.end(); }

    iterator erase(const iterator &i) { return book_list_.erase(i); }

    size_type size() const { return book_list_.size(); }
    bool empty() const { return book_list_.empty(); }

private:
    unsigned long id_{};
    book_list_t book_list_;
};

struct department;

class employee : public person
{
public:
    matador::object_ptr<department> department_;

public:
    employee() = default;
    explicit employee(const std::string &name) : person(name, matador::date(17, 12, 1983), 183) {}
    employee(const std::string &name, const matador::object_ptr<department> &dep)
            : person(name, matador::date(17, 12, 1983), 183)
            , department_(dep)
    {}

    template < class SERIALIZER >
    void serialize(SERIALIZER &serializer)
    {
        serializer.serialize(*matador::base_class<person>(this));
        serializer.on_belongs_to("department"    , department_, matador::cascade_type::NONE);
        // name of table, object     , cascade
    }

    matador::object_ptr<department> dep() const { return department_; }
    void dep(const matador::object_ptr<department> &d)
    {
        department_ = d;
    }
};

struct department
{
    unsigned long id{};
    std::string name;
    matador::has_many<employee> employees;

    department() = default;
    explicit department(std::string n)
            : name(std::move(n))
    {}

    ~department() = default;

    template < class SERIALIZER >
    void serialize(SERIALIZER &serializer)
    {
        serializer.on_primary_key("id", id);
        serializer.on_attribute("name", name, 255);
        serializer.on_has_many("employee"    , employees, "department", "id", matador::cascade_type::NONE);
        //                    name of table, container,  name of member
        //                                   to serialize
    }
};

class course;

class student : public person
{
public:
    student() = default;
    explicit student(const std::string &name, const matador::date &bdate = matador::date(), unsigned h = 170)
            : person(name, bdate, h) {}

    template < class SERIALIZER >
    void serialize(SERIALIZER &serializer)
    {
        serializer.serialize(*matador::base_class<person>(this));
        serializer.on_has_many("student_course", courses, "student_id", "course_id", matador::cascade_type::NONE);
    }

    matador::has_many<course> courses;
};

class course
{
public:

    course() = default;
    explicit course(std::string t) : title(std::move(t)) {}

    template < class SERIALIZER >
    void serialize(SERIALIZER &serializer)
    {
        serializer.on_primary_key("id", id);
        serializer.on_attribute("title", title, 1023);
        serializer.on_has_many("student_course", students, "student_id", "course_id", matador::cascade_type::ALL);
    }

    unsigned long id{};
    std::string title;
    matador::has_many<student> students;
};

struct address;

struct citizen : public person
{
    citizen() = default;
    explicit citizen(const std::string &name, const matador::date &bdate = matador::date(), unsigned h = 170) : person(name, bdate, h) {}

    matador::object_ptr<address> address_;

    template < class SERIALIZER >
    void serialize(SERIALIZER &serializer)
    {
        serializer.serialize(*matador::base_class<person>(this));
        serializer.on_has_one("address", address_, matador::cascade_type::ALL);
    }
};

struct address
{
    unsigned long id{};
    std::string street;
    std::string city;
    matador::object_ptr<citizen> citizen_;

    address() = default;
    address(std::string str, std::string c)
            : street(std::move(str)), city(std::move(c))
    {}

    template < class SERIALIZER >
    void serialize(SERIALIZER &serializer)
    {
        serializer.on_primary_key("id", id);
        serializer.on_attribute("street", street, 255);
        serializer.on_attribute("city", city, 255);
        serializer.on_belongs_to("citizen", citizen_, matador::cascade_type::NONE);
    }
};

class album;

class track
{
public:
    typedef matador::object_ptr<album> album_ptr;

private:
    unsigned long id_{};
    std::string title_;
    matador::object_ptr<album> album_;
    int index_;

public:
    track() : index_(0) {}
    explicit track(std::string title)
            : title_(std::move(title))
            , index_(0)
    {}

    ~track() = default;

    template < class SERIALIZER >
    void serialize(SERIALIZER &serializer)
    {
        serializer.on_primary_key("id", id_);
        serializer.on_attribute("title", title_);
        serializer.on_has_one("album", album_, matador::cascade_type::ALL);
        serializer.on_attribute("track_index", index_);
    }

    unsigned long id() { return id_; }

    std::string title() const { return title_; }
    void title(const std::string &t) { title_ = t; }

    album_ptr alb() const { return album_; }
    void alb(const album_ptr &a) { album_ = a; }

    int index() const { return index_; }
    void index(int i) { index_ = i; }
};

class album
{
public:
    typedef matador::object_ptr <track> track_ptr;
    typedef matador::has_many<track> track_vector_t;
    typedef track_vector_t::size_type size_type;
    typedef track_vector_t::iterator iterator;
    typedef track_vector_t::const_iterator const_iterator;

private:
    unsigned long id_{};
    std::string name_;
    track_vector_t tracks_;

public:
    album() = default;
    explicit album(std::string name) : name_(std::move(name)) {}

    ~album() = default;

    template < class SERIALIZER >
    void serialize(SERIALIZER &serializer)
    {
        serializer.on_primary_key("id", id_);
        serializer.on_attribute("name", name_);
        serializer.on_has_many("tracks", tracks_, matador::cascade_type::ALL);
    }

    unsigned long id() { return id_; }

    std::string name() const { return name_; }
    void name(const std::string &name) { name_ = name; }

    void add(const track_ptr &t)
    {
        tracks_.push_back(t);
    }

    iterator insert(const iterator& pos, const track_ptr &b)
    {
        return tracks_.insert(pos, b);
    }

    iterator begin() { return tracks_.begin(); }
    const_iterator begin() const { return tracks_.begin(); }

    iterator end() { return tracks_.end(); }
    const_iterator end() const { return tracks_.end(); }

    iterator erase(const iterator& i) { return tracks_.erase(i); }
    iterator erase(const iterator& a, const iterator& b) { return tracks_.erase(a, b); }

    size_type size() const { return tracks_.size(); }
    bool empty() const { return tracks_.empty(); }
};

class playlist
{
public:
    typedef matador::object_ptr<track> track_ref;
    typedef matador::has_many<track> track_list_t;
    typedef track_list_t::size_type size_type;
    typedef track_list_t::iterator iterator;
    typedef track_list_t::const_iterator const_iterator;

private:
    unsigned long id_{};
    std::string name_;
    track_list_t tracks_;
    track_list_t backup_tracks_;

public:
    playlist() = default;
    explicit playlist(std::string name) : name_(std::move(name)) {}

    ~playlist() = default;

    template < class SERIALIZER >
    void serialize(SERIALIZER &serializer)
    {
        serializer.on_primary_key("id", id_);
        serializer.on_attribute("name", name_);
        serializer.on_has_many("playlist_tracks", tracks_, matador::cascade_type::ALL);
        serializer.on_has_many("backup_tracks", backup_tracks_, matador::cascade_type::ALL);
    }

    std::string name() const { return name_; }
    void name(const std::string &name) { name_ = name; }

    void add(const track_ref &b)
    {
        tracks_.push_back(b);
    }

    iterator begin() { return tracks_.begin(); }
    const_iterator begin() const { return tracks_.begin(); }

    iterator end() { return tracks_.end(); }
    const_iterator end() const { return tracks_.end(); }

    iterator erase(const iterator &i) { return tracks_.erase(i); }

    size_type size() const { return tracks_.size(); }
    bool empty() const { return tracks_.empty(); }
};

class child
{
public:
    child() = default;
    explicit child(std::string n) : name(std::move(n)) {}
    ~child() = default;

    template < class S >
    void serialize(S &serializer)
    {
        serializer.on_primary_key("id", id);
        serializer.on_attribute("name", name);
    }

    unsigned long id{};
    std::string name;
};

class master
{
public:
    unsigned long id{};
    std::string name;
    matador::object_ptr<child> children;

public:
    master() = default;
    explicit master(std::string n) : name(std::move(n)) {}
    master(std::string n, const matador::object_ptr<child> &c) : name(std::move(n)), children(c) {}
    ~master() = default;

    template < class S >
    void serialize(S &serializer)
    {
        serializer.on_primary_key("id", id);
        serializer.on_attribute("name", name);
        serializer.on_has_one("child", children, matador::cascade_type::ALL);
    }
};

class children_vector
{
public:
    typedef matador::has_many<child> children_vector_t;

    children_vector() = default;
    explicit children_vector(std::string n) : name(std::move(n)) {}
    ~children_vector() = default;

    template < class S >
    void serialize(S &serializer)
    {
        serializer.on_primary_key("id", id);
        serializer.on_attribute("name", name);
        serializer.on_has_many("children", children, "vector_id", "child_id", matador::cascade_type::ALL);
    }

    unsigned long id{};
    std::string name;
    children_vector_t children;
};

class children_list
{
public:
    typedef matador::has_many<child, std::list> children_list_t;

    children_list() = default;
    explicit children_list(std::string n) : name(std::move(n)) {}
    ~children_list() = default;

    template < class S >
    void serialize(S &serializer)
    {
        serializer.on_primary_key("id", id);
        serializer.on_attribute("name", name);
        serializer.on_has_many("children", children, "list_id", "child_id", matador::cascade_type::ALL);
    }

    unsigned long id{};
    std::string name;
    children_list_t children;
};

template < class T, template <class ...> class C >
class many_builtins
{
public:
    typedef matador::has_many<T, C> element_list_t;

public:
    unsigned long id{};
    element_list_t elements;

    template < class S >
    void serialize(S &s)
    {
        s.on_primary_key("id", id);
        s.on_has_many("elements", elements, "list_id", "value", matador::cascade_type::ALL);
    }
};

using many_ints = many_builtins<int, std::list>;
using many_strings = many_builtins<std::string, std::list>;

using many_vector_ints = many_builtins<int, std::vector>;
using many_vector_strings = many_builtins<std::string, std::vector>;

class load
{
public:
    unsigned long id{};
    std::string name;

    load() = default;
    explicit load(std::string loadname) : name(std::move(loadname)) {}

    template < class S >
    void serialize(S &s)
    {
        s.on_primary_key("id", id);
        s.on_attribute("name", name, 255);
    }

};

class location
{
public:
    unsigned long id{};
    std::string name;

    location() = default;
    explicit location(std::string locname) : name(std::move(locname)) {}

    template < class S >
    void serialize(S &s)
    {
        s.on_primary_key("id", id);
        s.on_attribute("name", name, 255);
    }
};

class order
{
public:
    unsigned long id{};
    std::string name;
    matador::has_many<location> sources;
    matador::has_many<location> destinations;

    template < class S >
    void serialize(S &s)
    {
        s.on_primary_key("id", id);
        s.on_attribute("name", name, 255);
        s.on_has_many("sources", sources, matador::cascade_type::ALL);
        s.on_has_many("destinations", destinations, matador::cascade_type::ALL);
    }

};

#endif /* ENTITIES_HPP */