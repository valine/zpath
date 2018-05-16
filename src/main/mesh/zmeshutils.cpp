#include "mesh/zmeshutils.h"

ZMeshUtils::ZMeshUtils() {

}

vec3 ZMeshUtils::calculateBoundingBoxCenter(ZMesh* mesh) {
	if (mesh != nullptr) {
		vector<float> vertices = mesh->getVertices();
		if (vertices.size() > 3) {
			vec3 sum = vec3(0);

			for (unsigned i = 0; i < vertices.size() / 3; i+=1) {
				sum.x += vertices.at((i * 3) + 0);
				sum.y += vertices.at((i * 3) + 1);
				sum.z += vertices.at((i * 3) + 2);
			}

			float centerX = (sum.x) / (vertices.size() / 3);
			float centerY = (sum.y) / (vertices.size() / 3);
			float centerZ = (sum.z) / (vertices.size() / 3);
			return vec3(centerX, centerY, centerZ);
		} else {
			return vec3(vertices.at(0), vertices.at(1), vertices.at(2));
		}
	}
	return vec3(0);
}