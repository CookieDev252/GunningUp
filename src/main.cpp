#include "app.hpp"

int main()
{
    App app(500, 500, "Gunning UP");

    while (app.running) {
        app.update(1.f/60.f);

        app.draw();
    }
}