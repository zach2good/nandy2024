#include "app.h"

#include "common.h"

int main(int argc, char** argv)
{
    App app(Config::kScreenTitle, Config::kScreenWidth, Config::kScreenHeight);
    while (app.running())
    {
        if (app.nextFrame())
        {
            app.handleInput();
            app.tick();
            app.render();
        }
    }

    return 0;
}
