
#include "ztiledview.h"
#include <iostream>

ZTiledView::ZTiledView(float maxWidth, float maxHeight, int tilesX, int tilesY) 
: ZView(maxWidth, maxHeight) {

	mTileCountX = tilesX;
	mTileCountY = tilesY;

	float backgroundColor[4] = {0.2, 0.2, 0.2, 1};

	int margin = 1;
	int width = (getWidth() - (margin * tilesX)) / tilesX;
	int height = (getHeight() - (margin * tilesY)) / tilesY;

	for (int x = 0; x < tilesX; x++) {
		for (int y = 0; y < tilesY; y++) {
			ZView *tile = new ZView(width, height);
			tile->setOffset(x * (width + margin), y * (height + margin));
			tile->setMargin(margin, margin, margin, margin);
			tile->setBackgroundColor(backgroundColor);
			addSubView(tile);
		}
	}
}

void ZTiledView::setTileCount(int x, int y) {
	mTileCountX = x;
	mTileCountY = y;
}

void ZTiledView::computeBounds(int windowHeight, int maxWidth) {

	int i = 0;
	for (int x = 0; x < mTileCountX; x++) {
		for (int y = 0; y < mTileCountY; y++) {	
			int margin = 1;
			int width = (getWidth() - (margin * mTileCountX)) / mTileCountX;
			int height = (getHeight() - (margin * mTileCountY)) / mTileCountY;

			ZView *tile = getSubViews().at(i);

			tile->setMaxWidth(width);
			tile->setMaxHeight(height);

			tile->setOffset(x * (width + margin), y * (height + margin));
			i++;
		}

	}

	ZView::computeBounds(windowHeight, maxWidth);
}