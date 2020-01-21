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

	float panelColor[4] = {0.7, 0.7, 0.71, 1.0};

    auto* propertiesPanel = new ZScrollView(300, ZView::fillParent);
    propertiesPanel->setOffset(0, 22);
    propertiesPanel->setBackgroundColor(panelColor);
    propertiesPanel->setGravity(ZView::topRight);
    propertiesPanel->setInnerViewHeight(1200);
    getRootView()->addSubView(propertiesPanel);

    auto* label = new ZLabel(1000, 21, "roboto/Roboto-Bold.ttf", getResourcePath());
    label->setOffset(10,0);
    label->setText("Properties");
    label->setTextColor(vec3(0.1,0.1,0.1));
    propertiesPanel->addSubView(label);

    // Button Example
    auto* gridButton = new ZButton("Toggle Grid View", propertiesPanel);
    gridButton->setOnClick([this](ZView*) {
        if (mIsQuadView) {
            mIsQuadView = false;
            mTileView->setTileCount(1,1);
        } else {
            mIsQuadView = true;
            mTileView->setTileCount(mGridSizeX, mGridSizeY);
        }
    });

    auto* saveButton = new ZButton("SaveImage", propertiesPanel);
    saveButton->setOnClick([this](ZView*){
        ZUtil::saveView(mTileView->getTiles().at(0));
    });

    // Slider example
    auto* exposureSlider = new ZSlider("Exposure", 0, 20, mScene->getExposure(), propertiesPanel);
    exposureSlider->setOnSlide([this](ZView*, float v){
        mScene->setExposure(v);
    });

    // Checkbox example
    auto* checkbox = new ZCheckbox("Checkbox", getResourcePath(), propertiesPanel);
    checkbox->setOnClick([this](ZView*, bool checked){
        if (checked) {
            mIsQuadView = false;
            mTileView->setTileCount(1,1);
        } else {
            mIsQuadView = true;
            mTileView->setTileCount(mGridSizeX, mGridSizeY);
        }
    });

    ZRadioButton* gridSetting = new ZRadioButton(
            10000, 45, getResourcePath(), {"2x1", "2x2", "1x1"});
    gridSetting->setMargin(10,0,10,10);
    gridSetting->setText("View Grid");
    gridSetting->setOnClickListener(this);
    propertiesPanel->addSubView(gridSetting);

    mTileView = new ZTiledView(mScene, 10000, 10000, 1, 1, getResourcePath());
    mIsQuadView = false;
    mTileView->setOffset(propertiesPanel->getWidth(), 24);
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
