#ifndef ZPATHVIEWCONTROLLER_H
#define ZPATHVIEWCONTROLLER_H

#include "ui/zviewcontroller.h"

#include "mesh/zscene.h"
#include "mesh/zpointlight.h"
#include "ui/ztexture.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "mesh/zobject.h"
#include "ui/zchart.h"
#include "ui/zscrollview.h"

using namespace glm;
using namespace std;

class ZPathViewController : public ZViewController {

public:
 	ZPathViewController(char* argv[]);
 	void onCreate();
    void onFileDrop(int count, const char** paths);
private:
	bool mIsQuadView = true;

	ZTiledView *mTileView = nullptr;
	ZScene* mScene{};
    ZChart* mChart;

	static int mGridSizeX;
	static int mGridSizeY;

	string getFileExtension(string path);

};

#endif