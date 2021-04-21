#include <iostream>
#include <beagle/engine.h>
#include <eagle/platform/desktop/desktop_application.h>
#include <template/template_game.h>

int main() {

    eagle::DesktopApplication application(1280, 720, new beagle::Engine(new TemplateGame()));

    try {
        application.run();
    } catch (std::exception& e){
        std::cout << "Exception caught: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
