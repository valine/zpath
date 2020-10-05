#include "ui/ztiledview.h"
#include <iostream>
#include <utils/zsettingsstore.h>

ZTiledView::ZTiledView(ZScene* scene, float maxWidth, float maxHeight, int tilesX, int tilesY, string resourcePath) 
: ZView(maxWidth, maxHeight) {
   	mScene = scene;
   	mResourcePath = resourcePath;
	setTileCount(tilesX, tilesY);
    setBackgroundColor(ZSettingsStore::getInstance().getBackgroundColor());
}

void ZTiledView::setTileCount(int tilesX, int tilesY) {

	mTileCountX = tilesX;
	mTileCountY = tilesY;
	int width = (getWidth() - (mTileMargin * tilesX)) / tilesX;
	int height = (getHeight() - (mTileMargin * tilesY)) / tilesY;
   
	float backgroundColor[4] = {0.002428, 0.021219, 0.063010, 1.0};
	int i = 0;
	for (int x = 0; x < tilesX; x++) {
		for (int y = 0; y < tilesY; y++) {
			ZRenderer *renderer = new ZRenderer(mResourcePath);
			renderer->setScene(mScene);
			
			Z3DView *tile;
			if (i < (int) mTiles.size()) {
				tile = mTiles.at(i);
			} else {
				tile = new Z3DView(width, height, renderer);
				mTiles.push_back(tile);
				tile->setWindowWidth(getWindowWidth());
				tile->setWindowHeight(getWindowHeight());
				addSubView(tile);
			}

			tile->setOffset(x * (width + mTileMargin), y * (height + mTileMargin));
			tile->setMargin(mTileMargin, mTileMargin, mTileMargin, mTileMargin);
			tile->setBackgroundColor(backgroundColor);
			tile->setVisibility(true);
			tile->getRenderer()->getCamera()->setWidth(width);
			tile->getRenderer()->getCamera()->setHeight(height);
			tile->getRenderer()->recreateBuffers();
			i++;

			if (x == 1) {
				tile->setTilt(-90);
			}
		}
	}

	for (int j = i; j < (int) mTiles.size(); j++) {
		mTiles.at(j)->setVisibility(false);
		mTiles.at(j)->setMaxWidth(0);
	}

    computeBounds();
	invalidate();
}

void ZTiledView::setPerspective(bool perspective) {
	for (vector<Z3DView*>::iterator it = mTiles.begin() ; it != mTiles.end(); ++it) {
    	(*it)->getRenderer()->getCamera()->setUsePerspective(perspective);
    }
}

void ZTiledView::setTileMargin(int margin) {
	mTileMargin = margin;
	setTileCount(mTileCountX, mTileCountY);
}

void ZTiledView::setNearClipping(float nearClipping) {
	for (vector<Z3DView*>::iterator it = mTiles.begin() ; it != mTiles.end(); ++it) {
    	(*it)->getRenderer()->getCamera()->setNearClipping(nearClipping);
    }
}

void ZTiledView::zoomBy(float zoom) {
	for (vector<Z3DView*>::iterator it = mTiles.begin() ; it != mTiles.end(); ++it) {
    	(*it)->getRenderer()->getCamera()->translateBy(vec3(0,0,zoom));
    }

}

void ZTiledView::setWireMode(bool wireMode) {
	for (vector<Z3DView*>::iterator it = mTiles.begin() ; it != mTiles.end(); ++it) {
    	(*it)->getRenderer()->setWireMode(wireMode);
    }
}

void ZTiledView::computeBounds() {
	int i = 0;
	for (int x = 0; x < mTileCountX; x++) {
		for (int y = 0; y < mTileCountY; y++) {	

			int width = (getWidth() - (mTileMargin * mTileCountX)) / mTileCountX;
			int height = (getHeight() - (mTileMargin * mTileCountY)) / mTileCountY;

			ZView *tile = getSubViews().at(i);

			tile->setMaxWidth(width);
			tile->setMaxHeight(height);

			tile->setOffset(x * (width + mTileMargin), y * (height + mTileMargin));
			i++;
		}
	}

    ZView::computeBounds();
}

void ZTiledView::onMouseEvent(int button, int action, int mods, int x, int y) {
	ZView::onMouseEvent(button, action, mods, x, y);
}

void ZTiledView::onKeyPress(int key, int scancode, int action, int mods) {
	ZView::onKeyPress(key, scancode, action, mods);
}

void ZTiledView::setScene(ZScene* scene) {
	mScene = scene;
}

ZScene* ZTiledView::getScene() {
	return mScene;
}

void ZTiledView::setFocalLength(float focalLength) {
	for (vector<Z3DView*>::iterator it = mTiles.begin() ; it != mTiles.end(); ++it) {
    	(*it)->getRenderer()->getCamera()->setFocalLength(focalLength);
    }
}
	
void ZTiledView::onCursorPosChange(double x, double y)  {
	ZView::onCursorPosChange(x, y);

	if (mouseIsDown()) {
		int deltaX =  x - getMouseDownX();
		mTileWeightX = mInitialTileWeightX + deltaX;
	}
}

vector<Z3DView *> ZTiledView::getTiles() {
    return mTiles;
}

void ZTiledView::setAASamples(int samples) {
    for (vector<Z3DView*>::iterator it = mTiles.begin() ; it != mTiles.end(); ++it) {
        (*it)->getRenderer()->setAASamples(samples);
    }
}