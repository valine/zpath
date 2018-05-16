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
private: 
	ZMesh* mMesh;
	ZMaterial* mMaterial;
	bool mSelected = false;
	vec3 mOrigin = vec3(0);
};
#endif