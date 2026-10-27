#include <gtkmm.h>
#include <iostream>

#include "window.h"
#include "wrappers/libusb_wrapper.h"

int main(int argc, char *argv[]) {
    auto app = Gtk::Application::create("iscluis.ryujinIII.gui");
    RyujinDevice device(std::make_shared<LibUsbWrapper>());
    if (!device.Initialize()) {
        std::cerr << "Device not found" << std::endl;
        return 1;
    }
    return app->make_window_and_run<Window>(argc, argv,device);
}
