#include "ui/zimageview.h"

ZImageView::ZImageView(float maxWidth, float maxHeight, string resourcePath, string image) : 
ZView(maxWidth, maxHeight) {

	//mTexture = new ZTexture(resourcePath + image);
}

void ZImageView::draw() {
	ZView::draw();

}

void ZImageView::onMouseEvent(int button, int action, int mods, int x, int y) {
	return ZView::onMouseEvent(button, action, mods, x, y);
}

void ZImageView::onKeyPress(int key, int scancode, int action, int mods) {
	ZView::onKeyPress(key, scancode, action, mods);

}
	
void ZImageView::onCursorPosChange(double x, double y)  {
	// ZView::onCursorPosChange(x, y);
}

