#include "mesh/zanimator.h"

ZAnimator::ZAnimator(vec4 desiredValue, ZAnimator::Type type) {
	mDesiredValue = desiredValue;
	mType = type;
}

void ZAnimator::setDesiredValue(vec4 value) {
	mDesiredValue = value;

}

void ZAnimator::setType(ZAnimator::Type type) {
	mType = type;
}

ZAnimator::Type ZAnimator::getType() {
	return mType;
}

vec4 ZAnimator::getDesiredValue() {
	return mDesiredValue;
}
