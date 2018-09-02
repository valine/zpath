#ifndef ZANIMATOR_H
#define ZANIMATOR_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
using namespace glm;
using namespace std;
class ZAnimator {


public:

	enum Type {
	    translation,
	    rotation,
	    scale
	};

	ZAnimator(vec4, ZAnimator::Type);
	void setDesiredValue(vec4 value);
	void setType(ZAnimator::Type type);
	ZAnimator::Type getType();
	vec4 getDesiredValue();

private:

	vec4 mDesiredValue;
	ZAnimator::Type mType;
};

#endif