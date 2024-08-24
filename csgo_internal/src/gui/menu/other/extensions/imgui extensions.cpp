#include "../../../../precompiled.h"

namespace ImGui
{
    void ColorPicker(const char* name, Color* color, int change_controls)
    {
        float flColToImGui[4] =
        {
           static_cast<float>(color->r() / 255.f),
           static_cast<float>(color->g() / 255.f),
           static_cast<float>(color->b() / 255.f),
           static_cast<float>(color->a() / 255.f)
        };

        if (ImGui::ColorEdit4(name, flColToImGui, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_DisplayMask_))
            color->set(flColToImGui[0], flColToImGui[1], flColToImGui[2], flColToImGui[3]);

        const ImGuiCol_ colors[] =
        {
            ImGuiCol_CheckMark,
            ImGuiCol_SliderGrab,
            ImGuiCol_SliderGrabActive,
            ImGuiCol_HeaderActive,
        };

        const int size = sizeof(colors) / sizeof(colors[0]);

        if (change_controls == 1)
        {
            for (int i = 0; i < size; ++i)
            {
                ImGui::GetStyle().Colors[colors[i]] = ImVec4(flColToImGui[0], flColToImGui[1], flColToImGui[2], flColToImGui[3]);
            }
        }
        else if (change_controls == 2) {
            ImGui::GetStyle().Colors[ImGuiCol_MenuBarBg] = ImVec4(flColToImGui[0], flColToImGui[1], flColToImGui[2], flColToImGui[3]);
        }
    }

    void Separator(const char* label)
    {
        const auto textsize = ImGui::CalcTextSize(label);
        const auto draw_list = ImGui::GetWindowDrawList();
        const auto width = ImGui::GetContentRegionAvail().x;
        const auto cursor = ImGui::GetCursorScreenPos();
        const auto childsize = ImGui::GetWindowSize();
        const auto textpos = ImVec2(cursor.x + childsize.x / 2 - textsize.x / 2, cursor.y);
        const auto pos = ImVec2(cursor.x + 2, cursor.y + 7);
        auto maincolor = ImGui::GetColorU32(ImGuiCol_CheckMark);
        auto gradientcolor = ImGui::GetColorU32(ImGuiCol_MenuBarBg);

        if (g_Config.i(g_Items.m_Theme) < 1) {
            draw_list->AddRectFilledMultiColor(ImVec2(pos.x, pos.y), ImVec2(pos.x + (childsize.x / 2 - textsize.x / 2) - 8, pos.y + 2.f),
                maincolor, gradientcolor,
                gradientcolor, maincolor);//left

            draw_list->AddRectFilledMultiColor(ImVec2(pos.x + (childsize.x / 2 + textsize.x / 2) + 4, pos.y), ImVec2(pos.x + childsize.x, pos.y + 2.f),
                gradientcolor, maincolor,
                maincolor, gradientcolor);//right
        }
        else {
            auto alphacolor = ImColor(ImGui::GetStyle().Colors[ImGuiCol_CheckMark].x, ImGui::GetStyle().Colors[ImGuiCol_CheckMark].y, ImGui::GetStyle().Colors[ImGuiCol_CheckMark].z, 0.f);

            draw_list->AddRectFilledMultiColor(ImVec2(pos.x, pos.y), ImVec2(pos.x + (childsize.x / 2 - textsize.x / 2) - 8, pos.y + 2.f),
                alphacolor, maincolor,
                maincolor, alphacolor);//right

            draw_list->AddRectFilledMultiColor(ImVec2(pos.x + (childsize.x / 2 + textsize.x / 2) + 4, pos.y), ImVec2(pos.x + childsize.x, pos.y + 2.f),
                maincolor, alphacolor,
                alphacolor, maincolor);//left
        }

        draw_list->AddText(textpos, ImGui::GetColorU32(ImGui::GetStyleColorVec4(ImGuiCol_Text)), label);

        ImGui::NewLine();
        ImGui::Spacing();
    }

    static float CalcMaxPopupHeightFromItemCount(int items_count)
    {
        ImGuiContext& g = *GImGui;
        if (items_count <= 0)
            return FLT_MAX;
        return (g.FontSize + g.Style.ItemSpacing.y) * items_count - g.Style.ItemSpacing.y + (g.Style.WindowPadding.y * 2);
    }

    bool MultiCombobox(const char* name, std::vector<bool>& items, const std::string_view* arr, int count)
    {
        ImGui::SetCursorPosX(7);

        ImGuiContext& g = *GImGui;
        ImGuiWindow* pWindow = g.CurrentWindow;

        if (pWindow->SkipItems)
            return false;

        const ImGuiStyle& style = g.Style;
        const ImVec2 vecLabelSize = ImGui::CalcTextSize(name, nullptr, true);
        const float flActiveWidth = ImGui::CalcItemWidth() - (vecLabelSize.x > 0.0f ? style.ItemInnerSpacing.x + ImGui::GetFrameHeight() : 0.0f);

        std::vector<std::string_view> vecActiveItems = { };

        // collect active items
        for (int i = 0; i < count; i++)
        {
            if (items[i])
                vecActiveItems.push_back(arr[i]);
        }

        std::string buffer = { };
        for (std::size_t i = 0U; i < vecActiveItems.size(); i++)
        {
            buffer.append(vecActiveItems[i]);

            if (i < vecActiveItems.size() - 1U)
                buffer.append(", ");
        }

        if (buffer.empty())
            buffer.assign("None");
        else
        {
            const char* szWrapPosition = g.Font->CalcWordWrapPositionA(ImGui::GetCurrentWindow()->FontWindowScale, &buffer[0], buffer.data() + buffer.length(), flActiveWidth - style.FramePadding.x * 2.0f);
            const std::size_t nWrapLength = szWrapPosition - &buffer[0];

            if (nWrapLength > 0U && nWrapLength < buffer.length())
            {
                buffer.resize(nWrapLength);
                buffer.append("...");
            }
        }

        bool value_changed = false;
        if (ImGui::BeginCombo(name, buffer.c_str()))
        {
            for (int i = 0; i < count; i++)
            {
                if (ImGui::Selectable(arr[i].data(), items[i], ImGuiSelectableFlags_DontClosePopups))
                {
                    items[i] = !items[i];
                    value_changed = true;
                }
            }

            ImGui::EndCombo();
        }

        return value_changed;
    }

    // todo@ : these funcs were marked obsolete by ocornut. adding them here as a workaround. will delete when ocornut makes something new about it..
    inline bool FocusableItemRegisterWithoutAssertion(ImGuiWindow* window, ImGuiID id) {/* IM_ASSERT(0);*/ IM_UNUSED(window); IM_UNUSED(id); return false; } // -> pass ImGuiItemAddFlags_Inputable flag to ItemAdd()

    const char* keys[] = {
    "[-]",
    "[M1]",
    "[M2]",
    "[CN]",
    "[M3]",
    "[M4]",
    "[M5]",
    "[-]",
    "[BAC]",
    "[TAB]",
    "[-]",
    "[-]",
    "[CLR]",
    "[RET]",
    "[-]",
    "[-]",
    "[SHI]",
    "[CTL]",
    "[MEN]",
    "[PAU]",
    "[CAP]",
    "[KAN]",
    "[-]",
    "[JUN]",
    "[FIN]",
    "[KAN]",
    "[-]",
    "[ESC]",
    "[CON]",
    "[NCO]",
    "[ACC]",
    "[MAD]",
    "[SPA]",
    "[PGU]",
    "[PGD]",
    "[END]",
    "[HOM]",
    "[LEF]",
    "[UP]",
    "[RIG]",
    "[DOW]",
    "[SEL]",
    "[PRI]",
    "[EXE]",
    "[PRI]",
    "[INS]",
    "[DEL]",
    "[HEL]",
    "[0]",
    "[1]",
    "[2]",
    "[3]",
    "[4]",
    "[5]",
    "[6]",
    "[7]",
    "[8]",
    "[9]",
    "[-]",
    "[-]",
    "[-]",
    "[-]",
    "[-]",
    "[-]",
    "[-]",
    "[A]",
    "[B]",
    "[C]",
    "[D]",
    "[E]",
    "[F]",
    "[G]",
    "[H]",
    "[I]",
    "[J]",
    "[K]",
    "[L]",
    "[M]",
    "[N]",
    "[O]",
    "[P]",
    "[Q]",
    "[R]",
    "[S]",
    "[T]",
    "[U]",
    "[V]",
    "[W]",
    "[X]",
    "[Y]",
    "[Z]",
    "[WIN]",
    "[WIN]",
    "[APP]",
    "[-]",
    "[SLE]",
    "[NUM]",
    "[NUM]",
    "[NUM]",
    "[NUM]",
    "[NUM]",
    "[NUM]",
    "[NUM]",
    "[NUM]",
    "[NUM]",
    "[NUM]",
    "[MUL]",
    "[ADD]",
    "[SEP]",
    "[MIN]",
    "[DEC]",
    "[DIV]",
    "[F1]",
    "[F2]",
    "[F3]",
    "[F4]",
    "[F5]",
    "[F6]",
    "[F7]",
    "[F8]",
    "[F9]",
    "[F10]",
    "[F11]",
    "[F12]",
    "[F13]",
    "[F14]",
    "[F15]",
    "[F16]",
    "[F17]",
    "[F18]",
    "[F19]",
    "[F20]",
    "[F21]",
    "[F22]",
    "[F23]",
    "[F24]",
    "[-]",
    "[-]",
    "[-]",
    "[-]",
    "[-]",
    "[-]",
    "[-]",
    "[-]",
    "[NUM]",
    "[SCR]",
    "[EQU]",
    "[MAS]",
    "[TOY]",
    "[OYA]",
    "[OYA]",
    "[-]",
    "[-]",
    "[-]",
    "[-]",
    "[-]",
    "[-]",
    "[-]",
    "[-]",
    "[-]",
    "[SHI]",
    "[SHI]",
    "[CTR]",
    "[CTR]",
    "[ALT]",
    "[ALT]"
    };

    bool KeyBind(const char* label, int* current_key, int* key_mode)
    {
        ImGuiWindow* window = GetCurrentWindow();
        if (window->SkipItems)
            return false;

        ImGuiContext& g = *GImGui;
        const ImGuiStyle& style = g.Style;
        const ImGuiID id = window->GetID(label);
        ImGuiIO* io = &GetIO();

        SameLine(200, style.ItemInnerSpacing.x);

        char buf_display[32] = "[...]";

        if (*current_key != 0 && g.ActiveId != id)
            strcpy_s(buf_display, keys[*current_key]);

        else if (g.ActiveId == id)
            strcpy_s(buf_display, "[...]");
        else if (*key_mode >= 0)
            strcpy_s(buf_display, "[NOT SET]");

        ImVec2 label_size = CalcTextSize(buf_display);
        const ImRect total_bb(window->DC.CursorPos - ImVec2(0, 13), window->DC.CursorPos - ImVec2(0, 13) + ImVec2(label_size.x + 49, 0));
        const ImRect text_bb(total_bb.Min, total_bb.Max);
        const ImRect select_bb(text_bb.Min, total_bb.Max + ImVec2(0, label_size.y));

        ItemSize(text_bb, 0);
        if (!ItemAdd(select_bb, id))
            return false;

        const bool hovered = IsItemHovered();
        const bool smh = ImGui::ItemAdd(select_bb, g.ActiveId == id, 0, ImGuiItemFlags_Inputable);
        const bool edit_requested = FocusableItemRegisterWithoutAssertion(window, g.ActiveId == id);
        const bool style_requested = hovered && io->MouseClicked[1];

        const bool user_clicked = hovered && io->MouseClicked[0];

        if (edit_requested || user_clicked) {
            if (g.ActiveId != id) {
                memset(io->MouseDown, 0, sizeof(io->MouseDown));
                memset(io->KeysDown, 0, sizeof(io->KeysDown));
                *current_key = 0;
            }

            ImGui::SetActiveID(id, window);
            ImGui::FocusWindow(window);
        }
        else if (io->MouseClicked[0]) // empty space
        {
            // Release focus when we click outside
            if (g.ActiveId == id)
                ImGui::ClearActiveID();
        }

        bool value_changed = false;
        int key = *current_key;

        if (g.ActiveId == id) {
            for (auto i = 0; i < 5; i++) {
                if (io->MouseDown[i]) {
                    switch (i) {
                    case 0:
                        key = VK_LBUTTON;
                        break;
                    case 1:
                        key = VK_RBUTTON;
                        break;
                    case 2:
                        key = VK_MBUTTON;
                        break;
                    case 3:
                        key = VK_XBUTTON1;
                        break;
                    case 4:
                        key = VK_XBUTTON2;
                        break;
                    }
                    value_changed = true;
                    ImGui::ClearActiveID();
                }
            }
            if (!value_changed) {
                for (auto i = VK_BACK; i <= VK_RMENU; i++) {
                    if (io->KeysDown[i] && io->KeysDown[VK_ESCAPE] == false)
                    {
                        key = i;
                        value_changed = true;
                        ImGui::ClearActiveID();
                    }
                }
            }

            if (io->KeysDown[VK_ESCAPE]) {
                *current_key = 0;
                ImGui::ClearActiveID();
            }
            else {
                *current_key = key;
            }
        }
        else {
            if (key_mode) {
                bool popup_open = IsPopupOpen(id, ImGuiPopupFlags_None);

                if (style_requested && !popup_open)
                    OpenPopupEx(id);

                if (popup_open) {
                    SetNextWindowSize(ImVec2(76, CalcMaxPopupHeightFromItemCount(4)));

                    char name[16];
                    ImFormatString(name, IM_ARRAYSIZE(name), "##Combo_%02d", g.BeginPopupStack.Size); // Recycle windows based on depth

                    // Peak into expected window size so we can position it
                    if (ImGuiWindow* popup_window = FindWindowByName(name))

                        if (popup_window->WasActive) {
                            ImVec2 size_expected = ImGui::CalcWindowNextAutoFitSize(popup_window);
                            ImRect r_outer = ImGui::GetPopupAllowedExtentRect(popup_window);
                            ImVec2 pos = ImGui::FindBestWindowPosForPopupEx(text_bb.GetBL(), size_expected, &popup_window->AutoPosLastDirection, r_outer, text_bb, ImGuiPopupPositionPolicy_ComboBox);
                            ImGui::SetNextWindowPos(pos);
                        }

                    // Horizontally align ourselves with the framed text
                    ImGuiWindowFlags window_flags = ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_Popup | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings;
                    PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(style.FramePadding.x, style.WindowPadding.y));
                    bool ret = Begin(name, NULL, window_flags);
                    PopStyleVar();

                    if (Selectable("Always on", *key_mode == 0))
                        *key_mode = 0;

                    if (Selectable("Toggle", *key_mode == 1))
                        *key_mode = 1;

                    if (Selectable("Hold enable", *key_mode == 2))
                        *key_mode = 2;

                    if (Selectable("Hold disable", *key_mode == 3))
                        *key_mode = 3;

                    EndPopup();
                }
            }
        }

        PushFont(CRender::Fonts::m_MenuFontSmall);
        window->DrawList->AddText(text_bb.Min, g.ActiveId == id ? ImColor(255 / 255.f, 255 / 255.f, 255 / 255.f, g.Style.Alpha) : ImColor(0.35f, 0.35f, 0.35f, g.Style.Alpha), buf_display);

        //note@ : those were to test the clickable areas between keys.
        //window->DrawList->AddLine(text_bb.Min, text_bb.Max, ImColor(255 / 255.f, 255 / 255.f, 255 / 255.f, g.Style.Alpha), 1.f);
        //window->DrawList->AddLine(some.Min, some.Max, ImColor(255 / 255.f, 255 / 255.f, 255 / 255.f, g.Style.Alpha), 1.f);

        PopFont();

        return value_changed;
    }
}