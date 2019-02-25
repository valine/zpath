#ifndef ZOBJECT_H
#define ZOBJECT_H
#include <vector>
#include <iostream>
#include "mesh/zmesh.h"
#include "mesh/zmaterial.h"
#include "mesh/zanimator.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <algorithm>
#include "mesh/zonobjectclickedlistener.h"

using namespace std;
using namespace glm;
class ZOnObjectClickedListener;


class ZObject {
public:
	ZObject();
	void setMesh(ZMesh* mesh);
	ZMesh* getMesh();

	void setMaterial(ZMaterial* material);
	ZMaterial* getMaterial();
	void setSelected(bool selected);
	bool isSelected();
	vec3 getOrigin();
	void setOrigin(vec3 origin);

	void setTranslation(vec3);
	vec3 getTranslation();
	void translateBy(vec3);

	void setRotation(vec3);
	vec3 getRotation();
	void rotateBy(float angle);

	void setRotationAngle(float);
	float getRotationAngle();

	void setScale(vec3);
	vec3 getScale();
	void scaleBy(vec3);

	void setTransform(mat4);
	mat4 getTransform();
	void transformBy(mat4);

	bool isBillboard();
	void setBillboard(bool billboard);

	void setModelMatrix(mat4);
	mat4 getModelMatrix();
	bool getModelMatrixSet();

	ZObject* getParent();
	void setParent(ZObject* parent);
	vector<ZObject*> getChildren();

	void setSelectable(bool);
	bool getSelectable();

 	void setVisible(bool);
	bool getVisible();

	void setListener(ZOnObjectClickedListener* listener);

	float getType();
	void setType(float);
	void startAnimation(ZAnimator* animator);
	vector<ZAnimator*> getAnimators();
	void animationFinished(ZAnimator* animator);
	virtual void onSelection();

private: 
	ZMesh* mMesh;
	ZMaterial* mMaterial;
	bool mSelected = false;
	vec3 mOrigin = vec3(0);

	vec3 mTranslation = vec3(0);
	vec3 mRotation = vec3(1,0,0);
	vec3 mScale = vec3(1);
	mat4 mTransform = mat4(1);

	float mRotationAngle = 0;

	mat4 mModelMatrix;
	bool mModelMatrixSet = false;
	bool mIsBillboard = false;

	ZObject* mParent = nullptr;
	vector<ZObject*> mChildren;
	vector<ZAnimator*> mAnimators;


	ZOnObjectClickedListener* mListener = nullptr;
	float mType = 0.0;

	bool mSelectable = true;
	bool mVisible = true;

	void addChild(ZObject *child);

};
#endif