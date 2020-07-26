#include <memory>
#include <utility>

#include <ui/zchart.h>
#include <utils/zimageutil.h>
#include <ui/ztabview.h>
#include <ui/zradiobutton.h>
#include <ui/z2dslider.h>
#include "zpath/zpathviewcontroller.h"

int ZPathViewController::mGridSizeX = 3;
int ZPathViewController::mGridSizeY = 2;

ZPathViewController::ZPathViewController(char* argv[])
: ZViewController(argv) {
}

ZPathViewController::ZPathViewController(string path)
        : ZViewController(path) {
}

void ZPathViewController::onCreate() {
	ZViewController::onCreate();
	mScene = new BasicScene(getResourcePath());

    auto* tab1 = new ZScrollView(300, fillParent);
    auto* tab2 = new ZScrollView(300, fillParent);

    auto tabView = new ZTabView<ZView>(300, fillParent, "", {"Tab 1", "Tab2"});
    tabView->setGravity(ZView::topRight);
    tabView->addToTab(tab1, 0);
    tabView->addToTab(tab2, 1);
    addSubView(tabView);

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

    mChart = new ZChart(200, 200, tab1);
    auto* points = (float*) malloc(sizeof(float) * 6);
    for (int i = 0; i < 6; i++) {
        points[i] = i % 2;
    }
    mChart->updateLine(0, points, 6);


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

    mTileView = new ZTiledView(mScene, ZView::fillParent,  ZView::fillParent, 1, 1, getResourcePath());
    mIsQuadView = false;
    mTileView->setOffset(tab1->getWidth(), 0);
    mTileView->setGravity(ZView::topRight);
    addSubView(mTileView);


    ZViewController* subViewController = new ZViewController(getResourcePath());
    tab2->addSubView(subViewController);
    subViewController->setBackgroundColor(vec4(1,0,0,1));

    auto* slider2d = new Z2DSlider(200, 200, vec2(0,0), vec2(5), subViewController);
    slider2d->setIncrement(1);
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
