#include "ui/zviewcontroller.h"

#include "mesh/zscene.h"
#include "mesh/zpointlight.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace glm;
using namespace std;

class ZPathViewController : public ZViewController {

public:
 	ZPathViewController(string resources);
 	void onCreate();
 	void onClick(ZButton* sender);

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

};