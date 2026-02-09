#pragma once

#include <memory>
#include <vector>
#include <functional>
#include <string>
#include <iostream>
namespace test {
class Test {
 public:
    Test() {}
    virtual ~Test() {}

    virtual void OnUpdate(float deltaTime) {}
    virtual void OnRender() {}
    virtual void OnImGuiRender() {}
};

class TestMenu : public Test {
 public:
    TestMenu(Test*& currentTestPtr);
    virtual ~TestMenu() {}

    void OnImGuiRender() override;

    template <typename FuncName>
    void RegisterTest(const std::string& label) {
        std::cout << "Register Test: " << label << std::endl;
        m_Tests.push_back(
            std::make_pair(label, []() { return new FuncName(); }));
    }

 private:
    Test*&                                                      m_CurrentTest;
    std::vector<std::pair<std::string, std::function<Test*()>>> m_Tests;
};

}  // namespace test
