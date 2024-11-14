//-------------------------

#include "Core.hpp"

//-------------------------

int main() {

    Core core;

    core.init({ 640, 480 });

    while(core.windowIsOpen()) {

        core.update();
        core.draw();
    }

    return 0;
}

//-------------------------
