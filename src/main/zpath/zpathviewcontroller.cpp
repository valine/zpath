#include <ui/zchart.h>
#include "zpath/zpathviewcontroller.h"

int ZPathViewController::mGridSizeX = 3;
int ZPathViewController::mGridSizeY = 2;


ZPathViewController::ZPathViewController(string resources) 
: ZViewController(resources) {
}

void ZPathViewController::addObject(ZObject* object) {

}

void ZPathViewController::onCreate() {
	ZViewController::onCreate();

	mScene = new BasicScene(getResourcePath());

	float panelColor[4] = {0.9, 0.9, 0.91, 1.0};
    float highlightColor[4] = {0.1, 0.2, 0.9, 1.0};

    ZScrollView* propertiesPanel = new ZScrollView(300, ZView::fillParent);
    propertiesPanel->setOffset(0, 22);
    propertiesPanel->setBackgroundColor(panelColor);
    propertiesPanel->setGravity(ZView::topRight);
    propertiesPanel->setInnerViewHeight(1200);
    getRootView()->addSubView(propertiesPanel);

    ZLabel* label = new ZLabel(1000, 21, "roboto/Roboto-Bold.ttf", getResourcePath());
    label->setOffset(10,0);
    label->setText("Properties");
    label->setTextColor(vec3(0.1,0.1,0.1));
    propertiesPanel->addSubView(label);

    mGridViewButton = new ZButton(140, 40, getResourcePath());
    mGridViewButton->setOffset(0,50);
    mGridViewButton->setMargin(10,10,10,10);
    mGridViewButton->setBackgroundColor(highlightColor);
    mGridViewButton->setText("Toggle Grid View");
    mGridViewButton->setOnClickListener(this);
    propertiesPanel->addSubView(mGridViewButton);

    mExposureSlider = new ZSlider(10000, 60, "Exposure", getResourcePath());
    mExposureSlider->setOffset(0,150);
    mExposureSlider->setMargin(10, 10, 10, 10);
    mExposureSlider->setListener(this);
    propertiesPanel->addSubView(mExposureSlider);
    mExposureSlider->setMaxValue(20);
    mExposureSlider->setMinValue(0);
    mExposureSlider->setTextColor(vec3(0,0,0));
    mExposureSlider->setValue(mScene->getExposure());

    mRoughnessSlider = new ZSlider(10000, 60, "Roughness", getResourcePath());
    mRoughnessSlider->setOffset(0,250);
    mRoughnessSlider->setMargin(10, 10, 10, 10);
    mRoughnessSlider->setTextColor(vec3(0));
    mRoughnessSlider->setListener(this);
    propertiesPanel->addSubView(mRoughnessSlider);

    mMetalSlider = new ZSlider(10000, 60, "Metallic", getResourcePath());
    mMetalSlider->setOffset(0,350);
    mMetalSlider->setMargin(10, 10, 10, 10);
    mMetalSlider->setListener(this);
    mMetalSlider->setTextColor(vec3(0));
    propertiesPanel->addSubView(mMetalSlider);

    mRedSlider = new ZSlider(10000, 60, "Red", getResourcePath());
    mRedSlider->setOffset(0,350 + (100 * 1));
    mRedSlider->setMargin(10, 10, 10, 10);
    mRedSlider->setListener(this);
    mRedSlider->setLineColor(vec4(1,0,0,1));
    mRedSlider->setTextColor(vec3(0));
    propertiesPanel->addSubView(mRedSlider);

    mGreenSlider = new ZSlider(10000, 60, "Green", getResourcePath());
    mGreenSlider->setOffset(0,350 + (100 * 2));
    mGreenSlider->setMargin(10, 10, 10, 10);
    mGreenSlider->setListener(this);
    mGreenSlider->setTextColor(vec3(0));
    mGreenSlider->setLineColor(vec4(0,1,0,1));
    propertiesPanel->addSubView(mGreenSlider);

    mBlueSlider = new ZSlider(10000, 60, "Blue", getResourcePath());
    mBlueSlider->setOffset(0,350 + (100 * 3));
    mBlueSlider->setMargin(10, 10, 10, 10);
    mBlueSlider->setListener(this);
    mBlueSlider->setLineColor(vec4(0,0,1,1));
    mBlueSlider->setTextColor(vec3(0));
    propertiesPanel->addSubView(mBlueSlider);

    mAlphaSlider = new ZSlider(10000, 60, "Alpha", getResourcePath());
    mAlphaSlider->setOffset(0,350 + (100 * 4));
    mAlphaSlider->setMargin(10, 10, 10, 10);
    mAlphaSlider->setListener(this);
    mAlphaSlider->setTextColor(vec3(0));
    propertiesPanel->addSubView(mAlphaSlider);

    mFocalLengthSlider = new ZSlider(10000, 60, "Focal Length", getResourcePath());
    mFocalLengthSlider->setOffset(0,350 + (100 * 5));
    mFocalLengthSlider->setMargin(10, 10, 10, 10);
    mFocalLengthSlider->setListener(this);
    mFocalLengthSlider->setMaxValue(179);
    mFocalLengthSlider->setMinValue(1);
    mFocalLengthSlider->setTextColor(vec3(0));
    propertiesPanel->addSubView(mFocalLengthSlider);

    mBackgroundBlurButton = new ZButton(130, 40, getResourcePath());
    mBackgroundBlurButton->setOffset(0, 300 + (100 * 6));
    mBackgroundBlurButton->setMargin(10,10,10,10);
    mBackgroundBlurButton->setBackgroundColor(highlightColor);
    mBackgroundBlurButton->setText("Background Blur");
    mBackgroundBlurButton->setOnClickListener(this);
    propertiesPanel->addSubView(mBackgroundBlurButton);
    
    mDeleteButton = new ZButton(130, 40, getResourcePath());
    mDeleteButton->setOffset(140, 300 + (100 * 6));
    mDeleteButton->setMargin(10,10,10,10);
    mDeleteButton->setBackgroundColor(highlightColor);
    mDeleteButton->setText("Delete Object");
    mDeleteButton->setOnClickListener(this);
    propertiesPanel->addSubView(mDeleteButton);

    mBillboardButton = new ZButton(130, 40, getResourcePath());
    mBillboardButton->setOffset(140, 300 + (100 * 7));
    mBillboardButton->setMargin(10,10,10,10);
    mBillboardButton->setBackgroundColor(highlightColor);
    mBillboardButton->setText("Billboard");
    mBillboardButton->setOnClickListener(this);
    propertiesPanel->addSubView(mBillboardButton);

    ZView* navBar = new ZView(ZView::fillParent, 24);
    navBar->setBackgroundColor(panelColor);
    navBar->setGravity(ZView::topRight);
    getRootView()->addSubView(navBar);

    string menuItemNames[4] = {"File", "Edit", "View", "Render"};
    for (int i = 0; i < 4; i++) {
         ZLabel* menuItem = new ZLabel(48, 21, "roboto/Roboto-Regular.ttf", getResourcePath());
         menuItem->setOffset(i * 50 + 10, 0);
         menuItem->setText(menuItemNames[i]);
         menuItem->setTextColor(vec3(0.1,0.1,0.1));
         menuItem->setGravity(ZView::topLeft);
         navBar->addSubView(menuItem);
    } 

    mTileView = new ZTiledView(mScene, 10000, 10000, 1, 1, getResourcePath());
    mIsQuadView = false;
    mTileView->setOffset(propertiesPanel->getWidth(), 24);
    mTileView->setGravity(ZView::topRight);
    getRootView()->addSubView(mTileView);

    mAnimationButton = new ZButton(150, 40, getResourcePath());
    mAnimationButton->setOffset(150,50);
    mAnimationButton->setMargin(10,10,10,10);
    mAnimationButton->setBackgroundColor(highlightColor);
    mAnimationButton->setText("Test Animation");
    mAnimationButton->setOnClickListener(this);
    propertiesPanel->addSubView(mAnimationButton);

//    ZView* mSplash = new ZView(538, 336);
//    mSplash->setBackgroundColor(vec4(0, 0.56, 0.56, 1));
//    mSplash->setBackgroundImage(new ZTexture(getResourcePath() +
//                                 "resources/textures/splash.png"));
//    mSplash->setOffset((getRootView()->getWindowWidth()) / 2.0, 0);
//    getRootView()->addSubView(mSplash);

    ZChart* chart = new ZChart(100, 100, getResourcePath());
    getRootView()->addSubView(chart);

}

void ZPathViewController::onFileDrop(int count, const char** paths) {

 	for (int i = 0;  i < count;  i++) {
 		string path(paths[i]);   

        if (getFileExtension(path) == "hdr") {
            ZTexture* texture = new ZTexture(path);
            mScene->getWorld()->setEnvironmentTexture(texture);
        } else {
	
    		ZObjLoader loader = ZObjLoader();
            vector<ZObject*> objects = loader.loadObjects(path);
            for (auto object : objects) {
                mScene->addObject(object);
                //ZMaterial* brainMaterial = new ZMaterial(vec3(0.1,0.5,1));
                //brainMaterial->setRoughness(0.1);
                //brainMaterial->setMetallic(0);
               // object->setMaterial(brainMaterial);
            }
        }
    }
}

void ZPathViewController::onSliderValueChanged(ZSlider* sender, float value) {
  int selectionIndex = mScene->getActiveObjectIndex();

    if (sender == mExposureSlider) {
        mScene->setExposure(value);
    } 

    else if (sender == mFocalLengthSlider) {
        mTileView->setFocalLength(180 - value);
        string str = to_string(value);
        str.erase ( str.find_last_not_of('.') + 1, std::string::npos );
        mFocalLengthLabel->setText("Focal Length " + str);
    }

    if (sender == mRoughnessSlider) {
        if (selectionIndex != -1) {
            ZObject* object = mScene->getObjects().at(selectionIndex);
            ZMaterial* material = object->getMaterial();
            material->setRoughness(value);
        }
    } 

    else if (sender == mMetalSlider) {
        if (selectionIndex != -1) {
            ZObject* object = mScene->getObjects().at(selectionIndex);
            ZMaterial* material = object->getMaterial();
            material->setMetallic(value);
        }
    } 

    else if (sender == mRedSlider) {
        if (selectionIndex != -1) {
            ZObject* object = mScene->getObjects().at(selectionIndex);
            ZMaterial* material = object->getMaterial();
            vec4 color = material->getColor();
            color.r = value;
            material->setColor(color);
        } else {
            ZWorld* world = mScene->getWorld();
            vec3 color = world->getColor();
            color.r = value;
            world->setColor(color);
        }
    } 

    else if (sender == mGreenSlider) {
        if (selectionIndex != -1) {
            ZObject* object = mScene->getObjects().at(selectionIndex);
            ZMaterial* material = object->getMaterial();
            vec4 color = material->getColor();
            color.g = value;
            material->setColor(color);
        } else {
            ZWorld* world = mScene->getWorld();
            vec3 color = world->getColor();
            color.g = value;
            world->setColor(color);
        }
    }

     else if (sender == mBlueSlider) {
        if (selectionIndex != -1) {
            ZObject* object = mScene->getObjects().at(selectionIndex);
            ZMaterial* material = object->getMaterial();
            vec4 color = material->getColor();
            color.b = value;
            material->setColor(color);
        } else {
            ZWorld* world = mScene->getWorld();
            vec3 color = world->getColor();
            color.b = value;
            world->setColor(color);
        }
    } 

    else if (sender == mAlphaSlider) {
        if (selectionIndex != -1) {
            ZObject* object = mScene->getObjects().at(selectionIndex);
            ZMaterial* material = object->getMaterial();
            vec4 color = material->getColor();
            color.a = value;
            material->setColor(color);
        }
    }
}

void ZPathViewController::onClick(ZButton* sender) {

    int selectionIndex = mScene->getActiveObjectIndex();

	ZViewController::onClick(sender);
	if (sender == mGridViewButton) {
	    if (mIsQuadView) {
	        mIsQuadView = false;
	        mTileView->setTileCount(1,1);
	    } else {
	        mIsQuadView = true;
	        mTileView->setTileCount(mGridSizeX,mGridSizeY);
	    }
	} 

    else if (sender == mBackgroundBlurButton) {
        ZScene* scene = mTileView->getScene();
        ZWorld* world = scene->getWorld();
        world->blurBackground(!world->isBackgroundBlurred());
    }

    else if (sender == mAnimationButton && selectionIndex != -1) {
        ZScene* scene = mTileView->getScene();
        ZObject* object = scene->getObjects().at(selectionIndex);
        object->startAnimation(new ZAnimator(vec4(1,0,0,180), ZAnimator::rotation));
    }

    else if (sender == mDeleteButton && selectionIndex != -1) {
        ZScene* scene = mTileView->getScene();
        scene->deleteSelectedObject();
    }

    else if (sender == mBillboardButton && selectionIndex != -1) {
        ZScene* scene = mTileView->getScene();
        ZObject* object = scene->getObjects().at(selectionIndex);
        object->setBillboard(!object->isBillboard());
    }
}

string ZPathViewController::getFileExtension(string path) {
    return path.substr(path.find_last_of(".") + 1) ;
}