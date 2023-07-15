#include <ui/zapplication.h>
#include <zmainviewcontroller.h>

int main(int argc, char* argv[]) {
    /**
     * Main application loop
     */
    ZApplication(new MainViewController(argv), "Sample Project", false);
}
