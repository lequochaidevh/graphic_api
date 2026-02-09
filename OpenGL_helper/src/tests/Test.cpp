#include "Test.h"
#include "imgui/imgui.h"

namespace test {
TestMenu::TestMenu(Test*& currentTestPtr) : m_CurrentTest(currentTestPtr) {}

void TestMenu::OnImGuiRender() {
    for (auto& test : m_Tests) {
        if (ImGui::Button(test.first.c_str())) {  // click
            std::cout << "Onclick " << test.first.c_str() << std::endl;
            m_CurrentTest = test.second();
        }
    }
}
}  // namespace test
