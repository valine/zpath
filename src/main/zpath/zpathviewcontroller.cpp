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

    mTileView = new ZTiledView(mScene, 10000, 10000, 2, 1, getResourcePath());
    mTileView->setOffset(propertiesPanel->getWidth(), 22);
    mTileView->setGravity(ZView::topRight);
    getRootView()->addSubView(mTileView);
}

void ZPathViewController::onFileDrop(int count, const char** paths) {

 	for (int i = 0;  i < count;  i++) {

 		string path(paths[i]);   
		
		ZObjLoader loader = ZObjLoader();
        ZObject* object = loader.loadObject(path);
		mScene->addObject(object);

		ZMaterial* brainMaterial = new ZMaterial(vec3(0.093402, 0.211725, 0.420024));
		brainMaterial->setRoughness(0.1);
		object->setMaterial(brainMaterial);
    }
}

void ZPathViewController::onClick(ZButton* sender) {
	ZViewController::onClick(sender);
	if (sender == mGridViewButton) {
	    if (mIsQuadView) {
	        mIsQuadView = false;
	        mTileView->setTileCount(1,1);
	    } else {
	        mIsQuadView = true;
	        mTileView->setTileCount(3,2);
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
	    ZObject* object = scene->getObjects().at(1);
	    ZMaterial* material = object->getMaterial();

	    float currentValue = material->getRoughness();
	    float newValue = currentValue +  0.03;
	    string str = to_string(newValue);
	    str.erase ( str.find_last_not_of('0') + 1, std::string::npos );
	    mRoughnessLabel->setText("Roughness " + str);


	    material->setRoughness(newValue);
	} else if (sender == mRoughnessDecrementButton) {
	    ZScene* scene = mTileView->getScene();
	    ZObject* object = scene->getObjects().at(1);
	    ZMaterial* material = object->getMaterial();

	    float currentValue = material->getRoughness();
	    float newValue = currentValue -  0.03;
	    string str = to_string(newValue);
	    str.erase ( str.find_last_not_of('0') + 1, std::string::npos );
	    mRoughnessLabel->setText("Roughness " + str);


	    material->setRoughness(newValue);
	} else if (sender == mMetalIncrementButton) {
	    ZScene* scene = mTileView->getScene();
	    ZObject* object = scene->getObjects().at(1);
	    ZMaterial* material = object->getMaterial();

	    float currentValue = material->getMetallic();
	    float newValue = currentValue + 0.05;
	    string str = to_string(newValue);
	    str.erase ( str.find_last_not_of('0') + 1, std::string::npos );
	    mMetalLabel->setText("Metallic " + str);

	    material->setMetallic(newValue);
	} else if (sender == mMetalDecrementButton) {
	          ZScene* scene = mTileView->getScene();
	    ZObject* object = scene->getObjects().at(1);
	    ZMaterial* material = object->getMaterial();

	    float currentValue = material->getMetallic();
	    float newValue = currentValue -  0.05;
	    string str = to_string(newValue);
	    str.erase ( str.find_last_not_of('0') + 1, std::string::npos );
	    mMetalLabel->setText("Metallic " + str);

	    material->setMetallic(newValue);
	}
}