#pragma once

namespace ImGui
{
	void ColorPicker(const char* name, Color* color, int change_controls = 0);
	void Separator(const char* label);
	bool MultiCombobox(const char* name, std::vector<bool>& items, const std::string_view* arr, int count);
	bool KeyBind(const char* label, int* current_key, int* key_mode);
	bool Combo(const char* label, int* current_item, bool(*items_getter)(void*, int, const char**), void* data, int items_count, int popup_max_height_in_items);
	bool Combo(const char* label, int* current_item, const char* const items[], int items_count, int height_in_items);
	bool Combo(const char* label, int* current_item, const char* items_separated_by_zeros, int height_in_items);
}
