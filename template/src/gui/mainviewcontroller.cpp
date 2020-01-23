#include "zmainviewcontroller.h"
#include "utils/zsettingsstore.h"

MainViewController::MainViewController(char **argv) : ZViewController(argv) {}

void MainViewController::onCreate() {
    ZViewController::onCreate();
    auto* root = new ZView(ZView::fillParent, ZView::fillParent, getRootView());
    root->setBackgroundColor(ZSettingsStore::getInstance().getBackgroundColor());

    auto* button = new ZButton("Button", root);
    button->setOnClick([=](ZView*){
       cout << "hello" << endl;
    });
}
