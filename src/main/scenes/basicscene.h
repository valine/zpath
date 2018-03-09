#ifndef BASICSCENE_H
#define BASICSCENE_H

#include "mesh/zscene.h"
#include "mesh/zpointlight.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace glm;
using namespace std;

class BasicScene : public ZScene {

public:

	BasicScene(string resourcePath);

private:


};

#endif