#include "zview.h"

#include <iostream>
#include "mesh/zscene.h"
#include "mesh/zrenderer.h"
#include "scenes/basicscene.h"

using std::cout;
using std::endl;


class ZTiledView : public ZView {

public:

	ZTiledView(ZScene* scene, float maxWidth, float maxHeight, int tilesX, int tilesY, string resourcePath);
	void setTileCount(int x, int y);
	void computeBounds();
	void setTileMargin(int margin);

	void onKeyPress(int key, int scancode, int action, int mods);
	bool onMouseEvent(int button, int action, int mods, int x, int y);
	void onCursorPosChange(double x, double y);
	void setScene(ZScene* scene);

	void zoomBy(float zoom);
	void setFocalLength(float focalLength);
	void setWireMode(bool);
	void setPerspective(bool);
	ZScene* getScene();
	void setNearClipping(float nearClipping);
    vector<Z3DView*> getTiles();
    void setAASamples(int samples);
private:
	int mTileCountX;
	int mTileCountY;

	int mTileMargin = 2;
	
	int mInitialTileWeightX = 0;
	int mTileWeightX = 0;


	ZView* mRootView;
	ZScene* mScene;
	string mResourcePath;
	vector<Z3DView*> mTiles;
};