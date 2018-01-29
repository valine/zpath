#include "zrenderer.h"



ZRenderer::ZRenderer(string resourcePath) {


}

void ZRenderer::draw() {
	std::cout << "drawing";
}

void ZRenderer::setScene(ZScene *scene) {
	mScene = scene;
}