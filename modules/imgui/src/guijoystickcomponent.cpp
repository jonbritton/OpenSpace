/*****************************************************************************************
 *                                                                                       *
 * OpenSpace                                                                             *
 *                                                                                       *
 * Copyright (c) 2014-2024                                                               *
 *                                                                                       *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this  *
 * software and associated documentation files (the "Software"), to deal in the Software *
 * without restriction, including without limitation the rights to use, copy, modify,    *
 * merge, publish, distribute, sublicense, and/or sell copies of the Software, and to    *
 * permit persons to whom the Software is furnished to do so, subject to the following   *
 * conditions:                                                                           *
 *                                                                                       *
 * The above copyright notice and this permission notice shall be included in all copies *
 * or substantial portions of the Software.                                              *
 *                                                                                       *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,   *
 * INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A         *
 * PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT    *
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF  *
 * CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE  *
 * OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                                         *
 ****************************************************************************************/

#include <modules/imgui/include/guijoystickcomponent.h>

#include <modules/imgui/include/imgui_include.h>
#include <openspace/engine/globals.h>
#include <openspace/interaction/joystickinputstate.h>

namespace {
    const ImVec2 Size = ImVec2(350, 500);
} // namespace

namespace openspace::gui {

GuiJoystickComponent::GuiJoystickComponent()
    : GuiComponent("joystick_information", "Joystick Information")
{}

void GuiJoystickComponent::render() {
    using namespace interaction;

    ImGui::SetNextWindowCollapsed(_isCollapsed);

    bool v = _isEnabled;
    ImGui::SetNextWindowSize(Size, ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowBgAlpha(0.5f);
    ImGui::Begin("Joystick Information", &v);
    _isEnabled = v;
    _isCollapsed = ImGui::IsWindowCollapsed();

    for (size_t i = 0; i < global::joystickInputStates->size(); i++) {
        const JoystickInputState& state = global::joystickInputStates->at(i);
        if (!state.isConnected) {
            continue;
        }

        ImGui::Text("%s [%i]", state.name.c_str(), static_cast<int>(i));
        ImGui::Text("%s", "Axes");
        for (int j = 0; j < state.nAxes; j++) {
            float f = state.axes[j];
            std::string id = std::to_string(j) + "##" + state.name + "Axis";
            ImGui::SliderFloat(
                id.c_str(),
                &f,
                -1.f,
                1.f
            );
        }
        ImGui::Text("%s", "Buttons");
        for (int j = 0; j < state.nButtons; j++) {
            std::string id = std::to_string(j) + "##" + state.name + "Button";
            ImGui::RadioButton(
                id.c_str(),
                state.buttons[j] == JoystickAction::Press ||
                state.buttons[j] == JoystickAction::Repeat
            );
        }

        ImGui::Separator();
    }

    ImGui::Separator();
    ImGui::Separator();

    ImGui::Text("%s", "Summed contributions");
    ImGui::Text("%s", "Axes");
    for (int i = 0; i < global::joystickInputStates->numAxes(); i++) {
        float f = global::joystickInputStates->axis("", i);
        std::string id = std::to_string(i) + "##" + "TotalAxis";
        ImGui::SliderFloat(
            id.c_str(),
            &f,
            -1.f,
            1.f
        );
    }
    ImGui::Text("%s", "Buttons");
    for (int i = 0; i < global::joystickInputStates->numButtons(); i++) {
        std::string id = std::to_string(i) + "##" + "TotalButton";
        ImGui::RadioButton(
            id.c_str(),
            global::joystickInputStates->button("", i, JoystickAction::Press) ||
                global::joystickInputStates->button("", i, JoystickAction::Repeat)
        );
    }

    ImGui::End();
}

} // namespace openspace::gui
