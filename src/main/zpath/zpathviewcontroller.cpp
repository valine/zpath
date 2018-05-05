#include "zpathviewcontroller.h"

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

    ZView* propertiesPanel = new ZView(300, ZView::fillParent);
    propertiesPanel->setOffset(0, 22);
    propertiesPanel->setBackgroundColor(panelColor);
    propertiesPanel->setGravity(ZView::topRight);
    getRootView()->addSubView(propertiesPanel);

    ZLabel* label = new ZLabel(1000, 21, "roboto/Roboto-Bold.ttf", getResourcePath());
    label->setOffset(10,0);
    label->setText("Properties");
    label->setTextColor(vec3(0.1,0.1,0.1));
    propertiesPanel->addSubView(label);

    mGridViewButton = new ZButton(200, 40, getResourcePath());
    mGridViewButton->setOffset(0,50);
    mGridViewButton->setMargin(10,10,10,10);
    mGridViewButton->setBackgroundColor(highlightColor);
    mGridViewButton->setText("Toggle Grid View");
    mGridViewButton->setOnClickListener(this);
    propertiesPanel->addSubView(mGridViewButton);

    mBackgroundBlurButton = new ZButton(200, 40, getResourcePath());
    mBackgroundBlurButton->setOffset(0,50);
    mBackgroundBlurButton->setMargin(10,10,10,10);
    mBackgroundBlurButton->setBackgroundColor(highlightColor);
    mBackgroundBlurButton->setText("Toggle Background Blur");
    mBackgroundBlurButton->setGravity(ZView::bottomLeft);
    mBackgroundBlurButton->setOnClickListener(this);
    propertiesPanel->addSubView(mBackgroundBlurButton);

    mDeleteButton = new ZButton(200, 40, getResourcePath());
    mDeleteButton->setOffset(0,100);
    mDeleteButton->setMargin(10,10,10,10);
    mDeleteButton->setBackgroundColor(highlightColor);
    mDeleteButton->setText("Delete Object");
    mDeleteButton->setGravity(ZView::bottomLeft);
    mDeleteButton->setOnClickListener(this);
    propertiesPanel->addSubView(mDeleteButton);

    mExposureLabel = new ZLabel(1000, 21, "roboto/Roboto-Bold.ttf", getResourcePath());
    mExposureLabel->setOffset(10,120);
    mExposureLabel->setText("Exposure 1.0");
    mExposureLabel->setTextColor(vec3(0.1,0.1,0.1));
    propertiesPanel->addSubView(mExposureLabel);

    mExposureSlider = new ZSlider(10000, 30, getResourcePath());
    mExposureSlider->setOffset(0,150);
    mExposureSlider->setMargin(10, 10, 10, 10);
    mExposureSlider->setListener(this);
    propertiesPanel->addSubView(mExposureSlider);
    mExposureSlider->setMaxValue(5);
    mExposureSlider->setMinValue(0);
    mExposureSlider->setValue(mScene->getExposure());

    mRoughnessLabel = new ZLabel(1000, 21, "roboto/Roboto-Bold.ttf", getResourcePath());
    mRoughnessLabel->setOffset(10,220);
    mRoughnessLabel->setText("Roughness 0.1");
    mRoughnessLabel->setTextColor(vec3(0.1,0.1,0.1));
    propertiesPanel->addSubView(mRoughnessLabel);

    mRoughnessSlider = new ZSlider(10000, 30, getResourcePath());
    mRoughnessSlider->setOffset(0,250);
    mRoughnessSlider->setMargin(10, 10, 10, 10);
    mRoughnessSlider->setListener(this);
    propertiesPanel->addSubView(mRoughnessSlider);

    mMetalLabel = new ZLabel(100, 21, "roboto/Roboto-Bold.ttf", getResourcePath());
    mMetalLabel->setOffset(10,320);
    mMetalLabel->setText("Metallic 0.00");
    mMetalLabel->setTextColor(vec3(0.1,0.1,0.1));
    propertiesPanel->addSubView(mMetalLabel);

    mMetalSlider = new ZSlider(10000, 30, getResourcePath());
    mMetalSlider->setOffset(0,350);
    mMetalSlider->setMargin(10, 10, 10, 10);
    mMetalSlider->setListener(this);
    propertiesPanel->addSubView(mMetalSlider);

    // Red control ----------------------
    mRedLabel = new ZLabel(1000, 21, "roboto/Roboto-Bold.ttf", getResourcePath());
    mRedLabel->setOffset(10,320 + (100 * 1));
    mRedLabel->setText("Red");
    mRedLabel->setTextColor(vec3(0.1,0.1,0.1));
    propertiesPanel->addSubView(mRedLabel);

    mRedSlider = new ZSlider(10000, 30, getResourcePath());
    mRedSlider->setOffset(0,350 + (100 * 1));
    mRedSlider->setMargin(10, 10, 10, 10);
    mRedSlider->setListener(this);
    propertiesPanel->addSubView(mRedSlider);

  // Green control ----------------------
    mGreenLabel = new ZLabel(1000, 21, "roboto/Roboto-Bold.ttf", getResourcePath());
    mGreenLabel->setOffset(10,320 + (100 * 2));
    mGreenLabel->setText("Green");
    mGreenLabel->setTextColor(vec3(0.1,0.1,0.1));
    propertiesPanel->addSubView(mGreenLabel);

    mGreenSlider = new ZSlider(10000, 30, getResourcePath());
    mGreenSlider->setOffset(0,350 + (100 * 2));
    mGreenSlider->setMargin(10, 10, 10, 10);
    mGreenSlider->setListener(this);
    propertiesPanel->addSubView(mGreenSlider);


    // Blue control ----------------------
    mBlueLabel = new ZLabel(1000, 21, "roboto/Roboto-Bold.ttf", getResourcePath());
    mBlueLabel->setOffset(10,320 + (100 * 3));
    mBlueLabel->setText("Blue");
    mBlueLabel->setTextColor(vec3(0.1,0.1,0.1));
    propertiesPanel->addSubView(mBlueLabel);

    mBlueSlider = new ZSlider(10000, 30, getResourcePath());
    mBlueSlider->setOffset(0,350 + (100 * 3));
    mBlueSlider->setMargin(10, 10, 10, 10);
    mBlueSlider->setListener(this);
    propertiesPanel->addSubView(mBlueSlider);

    // Alpha control ----------------------
    mAlphaLabel = new ZLabel(1000, 21, "roboto/Roboto-Bold.ttf", getResourcePath());
    mAlphaLabel->setOffset(10,320 + (100 * 4));
    mAlphaLabel->setText("Alpha");
    mAlphaLabel->setTextColor(vec3(0.1,0.1,0.1));
    propertiesPanel->addSubView(mAlphaLabel);

    mAlphaSlider = new ZSlider(10000, 30, getResourcePath());
    mAlphaSlider->setOffset(0,350 + (100 * 4));
    mAlphaSlider->setMargin(10, 10, 10, 10);
    mAlphaSlider->setListener(this);
    propertiesPanel->addSubView(mAlphaSlider);

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
    mTileView->setOffset(propertiesPanel->getWidth(), 22);
    mTileView->setGravity(ZView::topRight);
    getRootView()->addSubView(mTileView);


}

void ZPathViewController::onFileDrop(int count, const char** paths) {

 	for (int i = 0;  i < count;  i++) {

 		string path(paths[i]);   

        if (getFileExtension(path) == "hdr") {
            ZTexture* texture = new ZTexture(path);
            mScene->getWorld()->setEnvironmentTexture(texture);
             if (mIsQuadView) {
                mTileView->setTileCount(3,2);
            } else {
                mTileView->setTileCount(1,1);
            }
        } else {
		
    		ZObjLoader loader = ZObjLoader();

            vector<ZObject*> objects = loader.loadObjects(path);

            for (unsigned i = 0; i < objects.size(); ++i) {
                ZObject* object = objects.at(i);

                mScene->addObject(object);
                //ZMaterial* brainMaterial = new ZMaterial(vec4(0.17, 0.01, ((float) (i % 5) / 16) + 0.09, 1.0));
                ZMaterial* brainMaterial = new ZMaterial(vec3(0.1,0.5,1));
                brainMaterial->setRoughness(0.1);
                brainMaterial->setMetallic(0);
                object->setMaterial(brainMaterial);
            }
        }
    }
}

void ZPathViewController::onSliderValueChanged(ZSlider* sender, float value) {
    int selectionIndex = mScene->getActiveObjectIndex();

    if (sender == mExposureSlider) {
        ZScene* scene = mTileView->getScene();
        float newExposure = value;
        string str = to_string(newExposure);
        str.erase ( str.find_last_not_of('.') + 1, std::string::npos );
        mExposureLabel->setText("Exposure " + str);
        scene->setExposure(value);
    } 

    if (selectionIndex != -1) {
        if (sender == mRoughnessSlider) {
            ZScene* scene = mTileView->getScene();
            ZObject* object = scene->getObjects().at(selectionIndex);
            ZMaterial* material = object->getMaterial();

            string str = to_string(value);
            str.erase ( str.find_last_not_of('.') + 1, std::string::npos );
            mRoughnessLabel->setText("Roughness " + str);
            material->setRoughness(value);
        } 

        else if (sender == mMetalSlider) {
            ZScene* scene = mTileView->getScene();
            ZObject* object = scene->getObjects().at(selectionIndex);
            ZMaterial* material = object->getMaterial();

            string str = to_string(value);
            str.erase ( str.find_last_not_of('.') + 1, std::string::npos );
            mMetalLabel->setText("Metallic " + str);
            material->setMetallic(value);
        } 

        else if (sender == mRedSlider) {
            ZScene* scene = mTileView->getScene();
            ZObject* object = scene->getObjects().at(selectionIndex);
            ZMaterial* material = object->getMaterial();

            vec4 color = material->getColor();
            color.r = value;
            string str = to_string(value);
            str.erase ( str.find_last_not_of('.') + 1, std::string::npos );
            mRedLabel->setText("Red " + str);
            material->setColor(color);
        } 

        else if (sender == mGreenSlider) {
            ZScene* scene = mTileView->getScene();
            ZObject* object = scene->getObjects().at(selectionIndex);
            ZMaterial* material = object->getMaterial();

            vec4 color = material->getColor();
            color.g = value;
            string str = to_string(value);
            str.erase ( str.find_last_not_of('.') + 1, std::string::npos );
            mGreenLabel->setText("Green " + str);
            material->setColor(color);
        }

         else if (sender == mBlueSlider) {
            ZScene* scene = mTileView->getScene();
            ZObject* object = scene->getObjects().at(selectionIndex);
            ZMaterial* material = object->getMaterial();

            vec4 color = material->getColor();
            color.b = value;
            string str = to_string(value);
            str.erase ( str.find_last_not_of('.') + 1, std::string::npos );
            mBlueLabel->setText("Blue " + str);
            material->setColor(color);
        } 

        else if (sender == mAlphaSlider) {
            ZScene* scene = mTileView->getScene();
            ZObject* object = scene->getObjects().at(selectionIndex);
            ZMaterial* material = object->getMaterial();

            vec4 color = material->getColor();
            color.a = value;
            string str = to_string(value);
            str.erase ( str.find_last_not_of('.') + 1, std::string::npos );
            mAlphaLabel->setText("Alpha " + str);
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
	        mTileView->setTileCount(2,1);
	    }
	} 

    else if (sender == mBackgroundBlurButton) {
        ZScene* scene = mTileView->getScene();
        ZWorld* world = scene->getWorld();
        world->blurBackground(!world->isBackgroundBlurred());
    }

    else if (sender == mDeleteButton && selectionIndex != -1) {
        ZScene* scene = mTileView->getScene();
        scene->deleteSelectedObject();
    }
}

string ZPathViewController::getFileExtension(string path) {
    return path.substr(path.find_last_of(".") + 1) ;
}