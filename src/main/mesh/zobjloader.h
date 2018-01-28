#include <string>
#include "mesh/zmesh.h"
#include <vector>

using std::vector;
using std::string;

class ZObjLoader {
	
public:
	ZObjLoader(string resourcePath);

	ZMesh loadMesh(string fileName);
	
private:



};