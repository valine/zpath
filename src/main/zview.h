
#include <string> 
#define GLEW_STATIC

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "zshader.h"

using std::string; 


class ZView {
	
	public:
		ZView(float maxWidth, float maxHeight, string dataPath, int shaderID);
		void draw();
		void setShader(int shaderID);
	private:
		int mShaderID;
		float mMaxWidth; 
		float mMaxHeight;

		GLuint mVertexBuffer;
		float mVertices[3 * 4];
		int mFaceIndices[6];
};