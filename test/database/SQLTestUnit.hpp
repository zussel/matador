//
// Created by sascha on 9/7/15.
//

#ifndef OOS_SQLTEST_HPP
#define OOS_SQLTEST_HPP


#include "unit/unit_test.hpp"

namespace oos {
    class session;
}

class SQLTestUnit : public oos::unit_test
{
public:
    SQLTestUnit(const std::string &name, const std::string &msg, const std::string &db = "memory", const oos::time &timeval = oos::time(2015, 3, 15, 13, 56, 23, 123));
    virtual ~SQLTestUnit();

    virtual void initialize();
    virtual void finalize();

    void test_create();

protected:
    std::string db() const;

private:
    std::string db_;
};


#endif //OOS_SQLTEST_HPP
