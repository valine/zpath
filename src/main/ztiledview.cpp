
#include "ztiledview.h"
#include <iostream>

ZTiledView::ZTiledView(float maxWidth, float maxHeight, int tilesX, int tilesY) 
: ZView(maxWidth, maxHeight) {

	mTileCountX = tilesX;
	mTileCountY = tilesY;

	float backgroundColor[4] = {0.1, 0.1, 0.1, 1};
;
	int width = (getWidth() - (mTileMargin * tilesX)) / tilesX;
	int height = (getHeight() - (mTileMargin * tilesY)) / tilesY;

	for (int x = 0; x < tilesX; x++) {
		for (int y = 0; y < tilesY; y++) {
			ZView *tile = new ZView(width, height);
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
			i++;
		}

	}

	ZView::computeBounds(windowHeight, maxWidth);
}