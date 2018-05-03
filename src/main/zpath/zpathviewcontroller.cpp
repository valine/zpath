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

    ZLabel* label = new ZLabel(100, 21, "roboto/Roboto-Bold.ttf", getResourcePath());
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

    mExposureLabel = new ZLabel(100, 21, "roboto/Roboto-Bold.ttf", getResourcePath());
    mExposureLabel->setOffset(10,120);
    mExposureLabel->setText("Exposure 1.0");
    mExposureLabel->setTextColor(vec3(0.1,0.1,0.1));
    propertiesPanel->addSubView(mExposureLabel);

    mIncrementButton = new ZButton(40, 40, getResourcePath());
    mIncrementButton->setOffset(0,150);
    mIncrementButton->setMargin(10,10,10,10);
    mIncrementButton->setBackgroundColor(highlightColor);
    mIncrementButton->setText("+");
    mIncrementButton->setOnClickListener(this);
    propertiesPanel->addSubView(mIncrementButton);

    mDecrementButton = new ZButton(40, 40, getResourcePath());
    mDecrementButton->setOffset(50,150);
    mDecrementButton->setMargin(10,10,10,10);
    mDecrementButton->setBackgroundColor(highlightColor);
    mDecrementButton->setText("-");
    mDecrementButton->setOnClickListener(this);
    propertiesPanel->addSubView(mDecrementButton);

    mRoughnessLabel = new ZLabel(200, 21, "roboto/Roboto-Bold.ttf", getResourcePath());
    mRoughnessLabel->setOffset(10,220);
    mRoughnessLabel->setText("Roughness 0.1");
    mRoughnessLabel->setTextColor(vec3(0.1,0.1,0.1));
    propertiesPanel->addSubView(mRoughnessLabel);

    mRoughnessIncrementButton = new ZButton(40, 40, getResourcePath());
    mRoughnessIncrementButton->setOffset(0,250);
    mRoughnessIncrementButton->setMargin(10,10,10,10);
    mRoughnessIncrementButton->setBackgroundColor(highlightColor);
    mRoughnessIncrementButton->setText("+");
    mRoughnessIncrementButton->setOnClickListener(this);
    propertiesPanel->addSubView(mRoughnessIncrementButton);


    mRoughnessDecrementButton = new ZButton(40, 40, getResourcePath());
    mRoughnessDecrementButton->setOffset(50,250);
    mRoughnessDecrementButton->setMargin(10,10,10,10);
    mRoughnessDecrementButton->setBackgroundColor(highlightColor);
    mRoughnessDecrementButton->setText("-");
    mRoughnessDecrementButton->setOnClickListener(this);
    propertiesPanel->addSubView(mRoughnessDecrementButton);

    mMetalLabel = new ZLabel(100, 21, "roboto/Roboto-Bold.ttf", getResourcePath());
    mMetalLabel->setOffset(10,320);
    mMetalLabel->setText("Metallic 0.00");
    mMetalLabel->setTextColor(vec3(0.1,0.1,0.1));
    propertiesPanel->addSubView(mMetalLabel);

    mMetalIncrementButton = new ZButton(40, 40, getResourcePath());
    mMetalIncrementButton->setOffset(0,350);
    mMetalIncrementButton->setMargin(10,10,10,10);
    mMetalIncrementButton->setBackgroundColor(highlightColor);
    mMetalIncrementButton->setText("+");
    mMetalIncrementButton->setOnClickListener(this);
    propertiesPanel->addSubView(mMetalIncrementButton);

    mMetalDecrementButton = new ZButton(40, 40, getResourcePath());
    mMetalDecrementButton->setOffset(50,350);
    mMetalDecrementButton->setMargin(10,10,10,10);
    mMetalDecrementButton->setBackgroundColor(highlightColor);
    mMetalDecrementButton->setText("-");
    mMetalDecrementButton->setOnClickListener(this);
    propertiesPanel->addSubView(mMetalDecrementButton);

    // Red control ----------------------
    mRedLabel = new ZLabel(100, 21, "roboto/Roboto-Bold.ttf", getResourcePath());
    mRedLabel->setOffset(10,320 + (100 * 1));
    mRedLabel->setText("Red");
    mRedLabel->setTextColor(vec3(0.1,0.1,0.1));
    propertiesPanel->addSubView(mRedLabel);

    mRedIncrementButton = new ZButton(40, 40, getResourcePath());
    mRedIncrementButton->setOffset(0,350 + (100 * 1));
    mRedIncrementButton->setMargin(10,10,10,10);
    mRedIncrementButton->setBackgroundColor(highlightColor);
    mRedIncrementButton->setText("+");
    mRedIncrementButton->setOnClickListener(this);
    propertiesPanel->addSubView(mRedIncrementButton);

    mRedDecrementButton = new ZButton(40, 40, getResourcePath());
    mRedDecrementButton->setOffset(50,350 + (100 * 1));
    mRedDecrementButton->setMargin(10,10,10,10);
    mRedDecrementButton->setBackgroundColor(highlightColor);
    mRedDecrementButton->setText("-");
    mRedDecrementButton->setOnClickListener(this);
    propertiesPanel->addSubView(mRedDecrementButton);

    // Blue control ----------------------
    mBlueLabel = new ZLabel(100, 21, "roboto/Roboto-Bold.ttf", getResourcePath());
    mBlueLabel->setOffset(10,320 + (100 * 2));
    mBlueLabel->setText("Blue");
    mBlueLabel->setTextColor(vec3(0.1,0.1,0.1));
    propertiesPanel->addSubView(mBlueLabel);

    mBlueIncrementButton = new ZButton(40, 40, getResourcePath());
    mBlueIncrementButton->setOffset(0,350 + (100 * 2));
    mBlueIncrementButton->setMargin(10,10,10,10);
    mBlueIncrementButton->setBackgroundColor(highlightColor);
    mBlueIncrementButton->setText("+");
    mBlueIncrementButton->setOnClickListener(this);
    propertiesPanel->addSubView(mBlueIncrementButton);

    mBlueDecrementButton = new ZButton(40, 40, getResourcePath());
    mBlueDecrementButton->setOffset(50,350 + (100 * 2));
    mBlueDecrementButton->setMargin(10,10,10,10);
    mBlueDecrementButton->setBackgroundColor(highlightColor);
    mBlueDecrementButton->setText("-");
    mBlueDecrementButton->setOnClickListener(this);
    propertiesPanel->addSubView(mBlueDecrementButton);

    // Green control ----------------------
    mGreenLabel = new ZLabel(100, 21, "roboto/Roboto-Bold.ttf", getResourcePath());
    mGreenLabel->setOffset(10,320 + (100 * 3));
    mGreenLabel->setText("Green");
    mGreenLabel->setTextColor(vec3(0.1,0.1,0.1));
    propertiesPanel->addSubView(mGreenLabel);

    mGreenIncrementButton = new ZButton(40, 40, getResourcePath());
    mGreenIncrementButton->setOffset(0,350 + (100 * 3));
    mGreenIncrementButton->setMargin(10,10,10,10);
    mGreenIncrementButton->setBackgroundColor(highlightColor);
    mGreenIncrementButton->setText("+");
    mGreenIncrementButton->setOnClickListener(this);
    propertiesPanel->addSubView(mGreenIncrementButton);

    mGreenDecrementButton = new ZButton(40, 40, getResourcePath());
    mGreenDecrementButton->setOffset(50,350 + (100 * 3));
    mGreenDecrementButton->setMargin(10,10,10,10);
    mGreenDecrementButton->setBackgroundColor(highlightColor);
    mGreenDecrementButton->setText("-");
    mGreenDecrementButton->setOnClickListener(this);
    propertiesPanel->addSubView(mGreenDecrementButton);

    // Alpha control ----------------------
    mAlphaLabel = new ZLabel(100, 21, "roboto/Roboto-Bold.ttf", getResourcePath());
    mAlphaLabel->setOffset(10,320 + (100 * 4));
    mAlphaLabel->setText("Alpha");
    mAlphaLabel->setTextColor(vec3(0.1,0.1,0.1));
    propertiesPanel->addSubView(mAlphaLabel);

    mAlphaIncrementButton = new ZButton(40, 40, getResourcePath());
    mAlphaIncrementButton->setOffset(0,350 + (100 * 4));
    mAlphaIncrementButton->setMargin(10,10,10,10);
    mAlphaIncrementButton->setBackgroundColor(highlightColor);
    mAlphaIncrementButton->setText("+");
    mAlphaIncrementButton->setOnClickListener(this);
    propertiesPanel->addSubView(mAlphaIncrementButton);

    mAlphaDecrementButton = new ZButton(40, 40, getResourcePath());
    mAlphaDecrementButton->setOffset(50,350 + (100 * 4));
    mAlphaDecrementButton->setMargin(10,10,10,10);
    mAlphaDecrementButton->setBackgroundColor(highlightColor);
    mAlphaDecrementButton->setText("-");
    mAlphaDecrementButton->setOnClickListener(this);
    propertiesPanel->addSubView(mAlphaDecrementButton);

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
           // ZObject* object = loader.loadObject(path);
    		//mScene->addObject(object)
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
	} else if (sender == mIncrementButton) {
	    ZScene* scene = mTileView->getScene();
	    float currentExposure = scene->getExposure();
	   
	    float newExposure = currentExposure + 0.1;
	    string str = to_string(newExposure);
	    str.erase ( str.find_last_not_of('0') + 1, std::string::npos );
	    mExposureLabel->setText("Exposure " + str);

	    scene->setExposure(currentExposure + 0.1f);
	} else if (sender == mDecrementButton) {
	    ZScene* scene = mTileView->getScene();
	    float currentExposure = scene->getExposure();
	    float newExposure = currentExposure - 0.1;

	    string str = to_string(newExposure);
	    str.erase ( str.find_last_not_of('0') + 1, std::string::npos );
	    mExposureLabel->setText("Exposure " + str);

	    scene->setExposure(newExposure);
	} else if (sender == mRoughnessIncrementButton) {
	    ZScene* scene = mTileView->getScene();
	    ZObject* object = scene->getObjects().at(selectionIndex);
	    ZMaterial* material = object->getMaterial();

	    float currentValue = material->getRoughness();
	    float newValue = currentValue +  0.03;
	    string str = to_string(newValue);
	    str.erase ( str.find_last_not_of('0') + 1, std::string::npos );
	    mRoughnessLabel->setText("Roughness " + str);


	    material->setRoughness(newValue);
	} else if (sender == mRoughnessDecrementButton) {
	    ZScene* scene = mTileView->getScene();
	    ZObject* object = scene->getObjects().at(selectionIndex);
	    ZMaterial* material = object->getMaterial();

	    float currentValue = material->getRoughness();
	    float newValue = currentValue -  0.03;
	    string str = to_string(newValue);
	    str.erase ( str.find_last_not_of('0') + 1, std::string::npos );
	    mRoughnessLabel->setText("Roughness " + str);


	    material->setRoughness(newValue);
	} 

    else if (sender == mMetalIncrementButton) {
	    ZScene* scene = mTileView->getScene();
	    ZObject* object = scene->getObjects().at(selectionIndex);
	    ZMaterial* material = object->getMaterial();

	    float currentValue = material->getMetallic();
	    float newValue = currentValue + 0.05;
	    string str = to_string(newValue);
	    str.erase ( str.find_last_not_of('0') + 1, std::string::npos );
	    mMetalLabel->setText("Metallic " + str);

	    material->setMetallic(newValue);
	} 

    else if (sender == mMetalDecrementButton) {
	          ZScene* scene = mTileView->getScene();
	    ZObject* object = scene->getObjects().at(selectionIndex);
	    ZMaterial* material = object->getMaterial();

	    float currentValue = material->getMetallic();
	    float newValue = currentValue -  0.05;
	    string str = to_string(newValue);
	    str.erase ( str.find_last_not_of('0') + 1, std::string::npos );
	    mMetalLabel->setText("Metallic " + str);

	    material->setMetallic(newValue);
    } 

     else if (sender == mRedIncrementButton) {
        ZScene* scene = mTileView->getScene();
        ZObject* object = scene->getObjects().at(selectionIndex);
        ZMaterial* material = object->getMaterial();

        vec4 color = material->getColor();
        float currentValue = color.r;
        float newValue = currentValue + 0.05;
        color.r = newValue;
        string str = to_string(newValue);
        str.erase ( str.find_last_not_of('0') + 1, std::string::npos );
        mRedLabel->setText("Red " + str);
        material->setColor(color);
    } 

    else if (sender == mRedDecrementButton) {
        ZScene* scene = mTileView->getScene();
        ZObject* object = scene->getObjects().at(selectionIndex);
        ZMaterial* material = object->getMaterial();

        vec4 color = material->getColor();
        float currentValue = color.r;
        float newValue = currentValue - 0.05;
        color.r = newValue;
        string str = to_string(newValue);
        str.erase ( str.find_last_not_of('0') + 1, std::string::npos );
        mRedLabel->setText("Red " + str);
        material->setColor(color);
    } 

    else if (sender == mBlueIncrementButton) {
        ZScene* scene = mTileView->getScene();
        ZObject* object = scene->getObjects().at(selectionIndex);
        ZMaterial* material = object->getMaterial();

        vec4 color = material->getColor();
        float currentValue = color.b;
        float newValue = currentValue + 0.05;
        color.b = newValue;
        string str = to_string(newValue);
        str.erase ( str.find_last_not_of('0') + 1, std::string::npos );
        mBlueLabel->setText("Blue " + str);
        material->setColor(color);
    } 

    else if (sender == mBlueDecrementButton) {
        ZScene* scene = mTileView->getScene();
        ZObject* object = scene->getObjects().at(selectionIndex);
        ZMaterial* material = object->getMaterial();

        vec4 color = material->getColor();
        float currentValue = color.b;
        float newValue = currentValue - 0.05;
        color.b = newValue;
        string str = to_string(newValue);
        str.erase ( str.find_last_not_of('0') + 1, std::string::npos );
        mBlueLabel->setText("Blue " + str);
        material->setColor(color);
    } 

    else if (sender == mGreenIncrementButton) {
        ZScene* scene = mTileView->getScene();
        ZObject* object = scene->getObjects().at(selectionIndex);
        ZMaterial* material = object->getMaterial();

        vec4 color = material->getColor();
        float currentValue = color.g;
        float newValue = currentValue + 0.05;
        color.g = newValue;
        string str = to_string(newValue);
        str.erase ( str.find_last_not_of('0') + 1, std::string::npos );
        mGreenLabel->setText("Green " + str);
        material->setColor(color);
    } 

    else if (sender == mGreenDecrementButton) {
        ZScene* scene = mTileView->getScene();
        ZObject* object = scene->getObjects().at(selectionIndex);
        ZMaterial* material = object->getMaterial();

        vec4 color = material->getColor();
        float currentValue = color.g;
        float newValue = currentValue - 0.05;
        color.g = newValue;
        string str = to_string(newValue);
        str.erase ( str.find_last_not_of('0') + 1, std::string::npos );
        mGreenLabel->setText("Green " + str);
        material->setColor(color);
    } 

    else if (sender == mAlphaIncrementButton) {
        ZScene* scene = mTileView->getScene();
        ZObject* object = scene->getObjects().at(selectionIndex);
        ZMaterial* material = object->getMaterial();

        vec4 color = material->getColor();
        float currentValue = color.a;
        float newValue = currentValue + 0.05;
        color.a = newValue;
        string str = to_string(newValue);
        str.erase ( str.find_last_not_of('0') + 1, std::string::npos );
        mAlphaLabel->setText("Alpha " + str);
        material->setColor(color);
    } 
    
    else if (sender == mAlphaDecrementButton) {
        ZScene* scene = mTileView->getScene();
        ZObject* object = scene->getObjects().at(selectionIndex);
        ZMaterial* material = object->getMaterial();

        vec4 color = material->getColor();
        float currentValue = color.a;
        float newValue = currentValue - 0.05;
        color.a = newValue;
        string str = to_string(newValue);
        str.erase ( str.find_last_not_of('0') + 1, std::string::npos );
        mAlphaLabel->setText("Alpha " + str);
        material->setColor(color);
    } 


    else if (sender == mBackgroundBlurButton) {
        ZScene* scene = mTileView->getScene();
        ZWorld* world = scene->getWorld();
        world->blurBackground(!world->isBackgroundBlurred());
    }
}

string ZPathViewController::getFileExtension(string path) {
    return path.substr(path.find_last_of(".") + 1) ;
}