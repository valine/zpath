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


void ZPathViewController::onCreate() {
	ZViewController::onCreate();
	mScene = new BasicScene(getResourcePath());

    auto* tab1 = new ZScrollView(300, fillParent);
    tab1->setName("Controls");
    auto* tab2 = new ZScrollView(300, fillParent);
    tab2->setName("Other");

    auto tabView = new ZTabView(120, fillParent, {tab1, tab2}, this);
    tabView->setGravity(ZView::topRight);

    auto* label = new ZLabel("Controls", tab1);
    label->setXOffset(5);
    label->setMarginTop(5);

    // Button Example
    auto* gridButton = new ZButton("Toggle Grid", tab1);
    gridButton->setCornerRadius(vec4(gridButton->getHeight() / 2,
            gridButton->getHeight() / 2,0,0));
    gridButton->setMarginLeft(5);
    gridButton->setMarginTop(5);
    gridButton->setOnClick([this](ZView*) {
        if (mIsQuadView) {
            mIsQuadView = false;
            mTileView->setTileCount(1,1);
        } else {
            mIsQuadView = true;
            mTileView->setTileCount(mGridSizeX, mGridSizeY);
        }
    });

    auto* saveButton = new ZButton("Save Image", tab1);
    saveButton->setCornerRadius(vec4(0,0,
                                     saveButton->getHeight() / 2,
                                     saveButton->getHeight() / 2));
    saveButton->setMarginLeft(5);
    saveButton->setMarginTop(1);
    saveButton->setOnClick([this](ZView*){
        ZUtil::saveView(mTileView->getTiles().at(0));
    });

    // Slider example
    auto* slider2 = new ZSlider("Exposure", 0, 20, mScene->getExposure(), tab1);
    slider2->setOnSlide([this](ZView* view, float v, bool actionUp) {
        mScene->setExposure(v);
    });

    auto checkBox = new ZCheckbox("Enable Blur", tab1);
    checkBox->setMarginTop(5);
    checkBox->setOnCheck([this](ZView* sender, bool checked){
        mScene->getWorld()->blurBackground(checked);
    });

    tab1->getInnerView()->refreshMargins();
    mTileView = new ZTiledView(mScene, ZView::fillParent,  ZView::fillParent, 1, 1, getResourcePath());
    mIsQuadView = false;
    mTileView->setOffset(tab1->getWidth(), 0);
    mTileView->setGravity(ZView::topRight);
    addSubView(mTileView);

    mTileView->setZoom(3);

    auto* subViewController = new ZViewController(getResourcePath());
    tab2->addSubView(subViewController);

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
