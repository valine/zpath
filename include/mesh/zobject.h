#ifndef ZOBJECT_H
#define ZOBJECT_H
#include <vector>
#include "mesh/zmesh.h"
#include "mesh/zmaterial.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace std;
using namespace glm;
class ZObject {
public:
	ZObject();
	void setMesh(ZMesh* mesh);
	ZMesh* getMesh();

	void setMaterial(ZMaterial* material);
	ZMaterial* getMaterial();
	void setSelected(bool selected);
	bool isSelected();
	vec3 getOrigin();
	void setOrigin(vec3 origin);

	void setTranslation(vec3);
	vec3 getTranslation();

	void setRotation(vec3);
	vec3 getRotation();

	void setRotationAngle(float);
	float getRotationAngle();

	void setScale(vec3);
	vec3 getScale();

	bool isBillboard();
	void setBillboard(bool billboard);

private: 
	ZMesh* mMesh;
	ZMaterial* mMaterial;
	bool mSelected = false;
	vec3 mOrigin = vec3(0);

	vec3 mTranslation = vec3(0);
	vec3 mRotation = vec3(1);
	vec3 mScale = vec3(1);

	float mRotationAngle = 0;

	bool mIsBillboard = false;
};
#endif