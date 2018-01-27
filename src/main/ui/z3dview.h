#include "zview.h"
#include <iostream>
#include <ui/zshader.h>
#include <vector> 
#include <string>

using std::cout;
using std::endl;

class Z3DView : public ZView {

public:
	Z3DView(float maxWidth, float maxHeight, string resourcePath);

	void onKeyPress(int key, int scancode, int action, int mods);
	void onMouseEvent(int button, int action, int mods, int x, int y);
	void onCursorPosChange(double x, double y);
	void draw();
private:
	int mDebug; 

	vector<float> mVertices;
	vector<int> mFaceIndices;

	GLuint mVertexBuffer;
	GLuint mFaceIndicesBuffer;

	int mColorLocation;
	int mPositionLocation;

		
	ZShader *mShader;
};