#ifndef UI_UI_HPP
#define UI_UI_HPP

#include "../core/graphics.hpp"
#include "../core/window.hpp"
#include "cursor.hpp"

void RenderUI() {
    Canvas* canvas = Graphics::GetScreenCanvas();
    WindowManager::RenderAll(*canvas);
    Cursor::Render(*canvas);
    Graphics::FlushScreenCanvas();
}

#endif

