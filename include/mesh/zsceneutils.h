#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "mesh/zscene.h"

using namespace glm;

class ZSceneUtils {

public:
	static vec2 projectPoint(vec3 point, ZScene* scene);
private:
};