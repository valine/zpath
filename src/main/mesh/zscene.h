#include <vector>
#include "mesh/zobject.h"
#include "mesh/zmesh.h"
using std::vector;

class ZScene {
	
public: 
	void addObject(ZObject* object);
private:
	vector<ZObject*> mObjects;
};