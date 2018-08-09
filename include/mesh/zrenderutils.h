#ifndef ZRENDERUTILS_H
#define ZRENDERUTILS_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "mesh/zobject.h"
#include "mesh/zcamera.h"

class ZRenderUtils {

public:
	static void saveRender(int fbo, int width, int height);
	static mat4 getModelMatrix(ZObject*, ZCamera*);
	static mat4 getViewMatrix(ZCamera*);

private:

};

#endif