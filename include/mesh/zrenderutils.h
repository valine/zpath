#ifndef ZRENDERUTILS_H
#define ZRENDERUTILS_H
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/matrix_decompose.hpp>

#include "mesh/zobject.h"
#include "mesh/zcamera.h"

class ZRenderUtils {

public:
	static void saveRender(int fbo, int width, int height);
	static mat4 getModelMatrix(ZObject*, ZCamera*);
	static mat4 getModelMatrix(ZObject*, ZCamera*, vec3 translation);
	static mat4 getViewMatrix(ZCamera*);
	static vec3 extractScale(mat4);
	static vec3 extractTranslation(mat4);

private:

};

#endif