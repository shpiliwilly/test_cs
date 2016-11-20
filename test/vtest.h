#pragma once

#include <iostream>
#include <vector>
#include <map>

namespace vtest {

    //////////////////////////////////////////////////////////////////

    struct TestCase {
        virtual bool RunTest() = 0;
        virtual const char* GetName() = 0;
    };

    //////////////////////////////////////////////////////////////////

    unsigned test_case_count = 0;
    std::map<const char*, std::vector<TestCase*> > test_suites;

    //////////////////////////////////////////////////////////////////

    void RunAllTests() {
        std::cout << "[=================]" << " Running " << test_case_count << " tests from " << test_suites.size() << " suites" << std::endl;

        unsigned fail_count = 0;
        for(auto& suite : test_suites) {
            auto& tests = suite.second;
            std::cout << std::endl << "[-----------------] " << tests.size() << " tests from " << suite.first << std::endl;

            for(auto& test : tests) {
                std::cout << "[  RUN            ] " << suite.first << "." << test->GetName() << std::endl;
                bool res = test->RunTest();
                if(res) {
                    std::cout << "[             OK  ]" << std::endl;
                } else {
                    std::cout << "[         FAILED  ]" << std::endl;
                    fail_count++;
                }
            }

            std::cout << "[-----------------]" << std::endl;
        }

        std::cout << std::endl << "[=================]" << std::endl;
        if(fail_count) {
            std::cout << "[     FAILED      ] " << fail_count << " tests failed"  << std::endl;
        } else {
            std::cout << "[     PASSED      ]" << std::endl;
        }
    }

}

//////////////////////////////////////////////////////////////////

#define TEST_CASE(test_suit_name, test_case_name)                           \
struct TestCase##test_case_name : public vtest::TestCase {                  \
    TestCase##test_case_name() {                                            \
        vtest::test_suites[#test_suit_name].push_back(this);                \
        vtest::test_case_count++;                                           \
    }                                                                       \
    static const char* m_name;                                              \
    virtual const char* GetName() { return m_name; }                        \
    virtual bool RunTest();                                                 \
} TestCase_##test_case_name##_instance;                                     \
const char* TestCase##test_case_name::m_name = #test_case_name;             \
bool TestCase##test_case_name::RunTest() 

//////////////////////////////////////////////////////////////////

#define ASSERT(condition)                                                   \
{                                                                           \
    if(!(condition)) {                                                      \
        std::cout << __FILE__ << "(" << __LINE__ << ")"                     \
            << ": assertion failed" << std::endl;                           \
        return false;                                                       \
    }                                                                       \
}

//////////////////////////////////////////////////////////////////

#define SUCCEED() return true

//////////////////////////////////////////////////////////////////

#define FAIL()                                                  \
    std::cout << __FILE__ << "(" << __LINE__  << ")"            \
        << ": FAIL() reached" << std::endl;                     \
    return false

