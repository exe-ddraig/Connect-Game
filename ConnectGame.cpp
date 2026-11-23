#define SDL_MAIN_HANDLED

#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")

#include "game.h"


int main()
{
    try
    {
        Game g;
        g.run();
        return 0;
    }
    catch (const exception& e)
    {
        cerr << e.what() << "\n";
        return 1;
    }
}
