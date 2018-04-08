#include "ui/zviewcontroller.h"

#include "mesh/zscene.h"
#include "mesh/zpointlight.h"
#include "ui/ztexture.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "mesh/zobject.h"

using namespace glm;
using namespace std;

class ZPathViewController : public ZViewController {

public:
 	ZPathViewController(string resources);
 	void onCreate();
 	void onClick(ZButton* sender);
 	void addObject(ZObject* object);
 	void onFileDrop(int count, const char** paths);

private:
	bool mIsQuadView = true;

	ZTiledView *mTileView = nullptr;

	ZButton *mGridViewButton;
	ZButton *mIncrementButton;
	ZButton *mDecrementButton;
	ZLabel *mExposureLabel;

	ZButton *mRoughnessIncrementButton;
	ZButton *mRoughnessDecrementButton;
	ZLabel *mRoughnessLabel;

	ZButton *mMetalIncrementButton;
	ZButton *mMetalDecrementButton;
	ZLabel *mMetalLabel;

	ZScene* mScene;

	string getFileExtension(string path);

};