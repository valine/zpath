#include <memory>
#include <utility>

#include <ui/zchart.h>
#include <utils/zimageutil.h>
#include <ui/ztabview.h>
#include <ui/zradiobutton.h>
#include "zpath/zpathviewcontroller.h"

int ZPathViewController::mGridSizeX = 3;
int ZPathViewController::mGridSizeY = 2;

ZPathViewController::ZPathViewController(char* argv[])
: ZViewController(argv) {
}

void ZPathViewController::onCreate() {
	ZViewController::onCreate();
	mScene = new BasicScene(getResourcePath());

    auto* tab1 = new ZScrollView(300, ZView::fillParent);
    auto* tab2 = new ZScrollView(300, ZView::fillParent);

    auto tabView = new ZTabView<ZView>(300, ZView::fillParent, "", {"Tab 1", "Tab2"});
    tabView->setGravity(ZView::topRight);
    tabView->addToTab(tab1, 0);
    tabView->addToTab(tab2, 1);
    getRootView()->addSubView(tabView);

    auto* label = new ZLabel("Panel", tab1);

    // Button Example
    auto* gridButton = new ZButton("Toggle Grid View", tab1);
    gridButton->setOnClick([this](ZView*) {
        if (mIsQuadView) {
            mIsQuadView = false;
            mTileView->setTileCount(1,1);
        } else {
            mIsQuadView = true;
            mTileView->setTileCount(mGridSizeX, mGridSizeY);
        }
    });

    auto* saveButton = new ZButton("SaveImage", tab1);
    saveButton->setOnClick([this](ZView*){
        ZUtil::saveView(mTileView->getTiles().at(0));
    });

    // Checkbox example
    auto* checkbox = new ZCheckbox("Checkbox", getResourcePath(), tab1);
    checkbox->setOnClick([this](ZView*, bool checked){
        if (checked) {
            mIsQuadView = false;
            mTileView->setTileCount(1,1);
        } else {
            mIsQuadView = true;
            mTileView->setTileCount(mGridSizeX, mGridSizeY);
        }
    });

    auto* label2 = new ZLabel("Second label", tab1);

    mChart = new ZChart(200, 200, tab1);
    auto* points = (float*) malloc(sizeof(float) * 6);
    for (int i = 0; i < 6; i++) {
        points[i] = i % 2;
    }
    mChart->addLine(points, 6);

    auto* radioButton = new ZRadioButton("Radio", {"One", "Two", "Three"}, tab1);
    radioButton->setOnClick([this](ZView* sender, int index) {
        cout << index << endl;
    });

    // Slider example
    auto* slider2 = new ZSlider("Exposure", 0, 20, mScene->getExposure(), tab1);
    slider2->setOnSlide([this](ZView* view, float v, bool actionUp){
        if (actionUp) {
            mScene->setExposure(v);
            auto *newpoints = (float *) malloc(sizeof(float) * 6);
            for (int i = 0; i < 6; i++) {
                newpoints[i] = i % (int) (v + 1);
            }
            mChart->updateLine(0, newpoints, 6);
        }
    });


    auto* radioButton2 = new ZRadioButton("Radio", {"One", "Two", "Three", "Four"}, tab2);
    radioButton2->setOnClick([=](ZView* sender, int index) {
        cout << index << endl;
    });

    mTileView = new ZTiledView(mScene, ZView::fillParent,  ZView::fillParent, 1, 1, getResourcePath());
    mIsQuadView = false;
    mTileView->setOffset(tab1->getWidth(), 0);
    mTileView->setGravity(ZView::topRight);
    getRootView()->addSubView(mTileView);
}

void ZPathViewController::onFileDrop(int count, const char** paths) {
    for (int i = 0; i < count; i++) {
        string path(paths[i]);
        if (getFileExtension(path) == "hdr") {
            auto *texture = new ZTexture(path);
            mScene->getWorld()->setEnvironmentTexture(texture);
        } else {

            ZObjLoader loader = ZObjLoader();
            vector<ZObject *> objects = loader.loadObjects(path);
            for (auto object : objects) {
                mScene->addObject(object);
            }
        }
    }
}

string ZPathViewController::getFileExtension(string path) {
    return path.substr(path.find_last_of(".") + 1) ;
}
