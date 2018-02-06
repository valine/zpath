
#include "ztiledview.h"
#include <iostream>

ZTiledView::ZTiledView(float maxWidth, float maxHeight, int tilesX, int tilesY, string resourcePath) 
: ZView(maxWidth, maxHeight) {

	mTileCountX = tilesX;
	mTileCountY = tilesY;

	float backgroundColor[4] = {0.1, 0.1, 0.1, 1};

	int width = (getWidth() - (mTileMargin * tilesX)) / tilesX;
	int height = (getHeight() - (mTileMargin * tilesY)) / tilesY;
  
   
   	BasicScene *scene = new BasicScene(resourcePath);


	for (int x = 0; x < tilesX; x++) {
		for (int y = 0; y < tilesY; y++) {

			ZRenderer *renderer = new ZRenderer(resourcePath);
			renderer->setScene(scene);

			Z3DView *tile = new Z3DView(width, height, renderer);
			tile->setOffset(x * (width + mTileMargin), y * (height + mTileMargin));
			tile->setMargin(mTileMargin, mTileMargin, mTileMargin, mTileMargin);
			tile->setBackgroundColor(backgroundColor);
			addSubView(tile);
		}
	}
}

void ZTiledView::setTileCount(int x, int y) {
	mTileCountX = x;
	mTileCountY = y;
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

	if (action == GLFW_PRESS) {
		float color[4] = {0.1,0.1,0.1,1.0};
		setBackgroundColor(color);
		mInitialTileWeightX = mTileWeightX;
	}

	if (action == GLFW_RELEASE) {
		float color[4] = {0,0,0,0};
		setBackgroundColor(color);
	}
}

void ZTiledView::onKeyPress(int key, int scancode, int action, int mods) {
	ZView::onKeyPress(key, scancode, action, mods);

}
	
void ZTiledView::onCursorPosChange(double x, double y)  {
	ZView::onCursorPosChange(x, y);

	if (mouseIsDown()) {
		int deltaX =  x - getMouseDownX();
		int deltaY = y - getMouseDownY();
		mTileWeightX = mInitialTileWeightX + deltaX;
	}
}

