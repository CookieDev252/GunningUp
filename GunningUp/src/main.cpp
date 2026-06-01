#include "app.hpp"

int main()
{
    App app(500, 500, "Gunning UP");

    while (app.running) {
        if (WindowShouldClose() && !IsKeyPressed(KEY_ESCAPE)) {
            return 0;
        }
        app.update(GetFrameTime());

        app.draw();
    }
}