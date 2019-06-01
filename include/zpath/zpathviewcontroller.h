#include "ui/zviewcontroller.h"

#include "mesh/zscene.h"
#include "mesh/zpointlight.h"
#include "ui/ztexture.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "mesh/zobject.h"
#include "ui/zscrollview.h"

using namespace glm;
using namespace std;

class ZPathViewController : public ZViewController {

public:
 	ZPathViewController(string resources);
 	void onCreate();
 	void onClick(ZButton* sender);
 	void onSliderValueChanged(ZSlider* sender, float value);
 	void addObject(ZObject* object);
 	void onFileDrop(int count, const char** paths);

private:
	bool mIsQuadView = true;

	ZTiledView *mTileView = nullptr;

	ZButton *mGridViewButton;
	ZButton *mBackgroundBlurButton;
	ZButton *mDeleteButton;
	ZButton *mIncrementButton;
	ZButton *mDecrementButton;
	ZLabel *mExposureLabel;
	ZButton *mBillboardButton;
	ZButton *mAnimationButton;
    ZButton* mSaveImageButton;

	ZSlider* mExposureSlider;
	ZSlider* mRoughnessSlider;
	ZSlider* mMetalSlider;
	ZSlider* mRedSlider;
	ZSlider* mBlueSlider;
	ZSlider* mGreenSlider;
	ZSlider* mAlphaSlider;
	ZSlider* mFocalLengthSlider;

	ZLabel *mRoughnessLabel;
	ZLabel *mMetalLabel;
	ZLabel *mRedLabel;
	ZLabel *mBlueLabel;
	ZLabel *mGreenLabel;
	ZLabel *mAlphaLabel;
	ZLabel *mFocalLengthLabel;

	ZScene* mScene;

	static int mGridSizeX;
	static int mGridSizeY;

	string getFileExtension(string path);

};