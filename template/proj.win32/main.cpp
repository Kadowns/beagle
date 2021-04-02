#include <iostream>
#include <beagle/engine.h>
#include <eagle/platform/desktop/desktop_application.h>


int main() {

    eagle::DesktopApplication application(1280, 720, new beagle::Engine());

    try {
        application.run();
    } catch (std::exception& e){
        std::cout << "Exception caught: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
