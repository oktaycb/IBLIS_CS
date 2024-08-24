#pragma once
#include <string>
#include <sstream>
#include <vector>
#include <math.h>
#include <d3d9.h>

#pragma comment (lib, "d3d9.lib")

#define IMGUI_DEFINE_MATH_OPERATORS
#include "../../imgui/imgui_internal.h"
#include "../../imgui/imgui.h"

namespace BlurData {

    inline IDirect3DDevice9* device;
}

extern void DrawBackgroundBlur(ImDrawList* drawList, int alpha = 255);