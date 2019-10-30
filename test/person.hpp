#ifndef PERSON_HPP
#define PERSON_HPP

#include "matador/utils/date.hpp"
#include "matador/utils/identifier.hpp"

class person
{
private:
    matador::identifier<unsigned long> id_;
    std::string name_;
    matador::date birthdate_;
    unsigned int height_ = 0;

public:
    person() = default;
    person(unsigned long id, std::string name, matador::date birthdate, unsigned int height)
        : id_(id)
        , name_(std::move(name))
        , birthdate_(std::move(birthdate))
        , height_(height)
    {}

    person(const std::string &name, const matador::date &birthdate, unsigned int height)
            : person(0, name, birthdate, height)
    {}

    virtual ~person() = default;

    template < class T >
    void serialize(T &serializer)
    {
        serializer.serialize("id", id_);
        serializer.serialize("name", name_, 255);
        serializer.serialize("birthdate", birthdate_);
        serializer.serialize("height", height_);
    }

    void id(unsigned long i) { id_.value(i); }
    unsigned long id() const { return id_.value(); }

    std::string name() const { return name_; }
    void name(const std::string &name) { name_ = name; }

    matador::date birthdate() const { return birthdate_; }
    void birthdate(const matador::date &birthdate) { birthdate_ = birthdate; }

    unsigned int height() const { return height_; }
    void height(unsigned int height) { height_ = height; }
};

#endif /* PERSON_HPP */