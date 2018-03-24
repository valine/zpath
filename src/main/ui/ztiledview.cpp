
#include "ztiledview.h"
#include <iostream>

ZTiledView::ZTiledView(float maxWidth, float maxHeight, int tilesX, int tilesY, string resourcePath) 
: ZView(maxWidth, maxHeight) {
   	mScene = new BasicScene(resourcePath);
   	mResourcePath = resourcePath;

	setTileCount(tilesX, tilesY);
}

void ZTiledView::setTileCount(int tilesX, int tilesY) {


    clearSubViews();

	mTileCountX = tilesX;
	mTileCountY = tilesY;

	int width = (getWidth() - (mTileMargin * tilesX)) / tilesX;
	int height = (getHeight() - (mTileMargin * tilesY)) / tilesY;
   

	float backgroundColor[4] = {0.002428, 0.021219, 0.063010, 1.0};
	for (int x = 0; x < tilesX; x++) {
		for (int y = 0; y < tilesY; y++) {
			ZRenderer *renderer = new ZRenderer(mResourcePath);
			renderer->setScene(mScene);

			Z3DView *tile = new Z3DView(width, height, renderer);
			tile->setOffset(x * (width + mTileMargin), y * (height + mTileMargin));
			tile->setMargin(mTileMargin, mTileMargin, mTileMargin, mTileMargin);
			tile->setBackgroundColor(backgroundColor);
			addSubView(tile);
		}
	}

	computeBounds(getWindowHeight(), getWindowWidth());
}

void ZTiledView::setTileMargin(int margin) {
	mTileMargin = margin;
}

void ZTiledView::computeBounds(int windowHeight, int maxWidth) {
	int i = 0;
	for (int x = 0; x < mTileCountX; x++) {
		for (int y = 0; y < mTileCountY; y++) {	

			int width = (getWidth() - (mTileMargin * mTileCountX)) / mTileCountX;
			int height = (getHeight() - (mTileMargin * mTileCountY)) / mTileCountY;

			ZView *tile = getSubViews().at(i);

			tile->setMaxWidth(width);
			tile->setMaxHeight(height);

			tile->setOffset(x * (width + mTileMargin), y * (height + mTileMargin));

			mTiles.push_back(tile);
			i++;
		}
	}

	ZView::computeBounds(windowHeight, maxWidth);
}

void ZTiledView::onMouseEvent(int button, int action, int mods, int x, int y) {
	ZView::onMouseEvent(button, action, mods, x, y);

	// if (action == GLFW_PRESS) {
	// 	float color[4] = {0.1,0.1,0.1,1.0};
	// 	setBackgroundColor(color);
	// 	mInitialTileWeightX = mTileWeightX;
	// }

	// if (action == GLFW_RELEASE) {
	// 	float color[4] = {0,0,0,0};
	// 	setBackgroundColor(color);
	// }
}

void ZTiledView::onKeyPress(int key, int scancode, int action, int mods) {
	ZView::onKeyPress(key, scancode, action, mods);

}

ZScene* ZTiledView::getScene() {
	return mScene;
}
	
void ZTiledView::onCursorPosChange(double x, double y)  {
	ZView::onCursorPosChange(x, y);

	if (mouseIsDown()) {
		int deltaX =  x - getMouseDownX();
		mTileWeightX = mInitialTileWeightX + deltaX;
	}
}

