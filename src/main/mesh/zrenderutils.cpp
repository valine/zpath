#include "mesh/zrenderutils.h"



void ZRenderUtils::saveRender(int fbo, int width, int height) {


}

vec3 ZRenderUtils::extractScale(mat4 matrix) {
    float col0 = glm::length(vec3(matrix[0]));
    float col1 = glm::length(vec3(matrix[1]));
    float col2 = glm::length(vec3(matrix[2]));

    return vec3(col0, col1, col2);
}


vec3 ZRenderUtils::extractTranslation(mat4 matrix) {
    return vec3(matrix[3]);
}

mat4 ZRenderUtils::getViewMatrix(ZCamera* camera) {
    mat4 viewMatrix = ZRenderUtils::getModelMatrix(camera, nullptr);
    viewMatrix = inverse(viewMatrix);

    if (camera->isManualView()) {
        viewMatrix = camera->getViewMatrix();
    } 

    return viewMatrix;

}

mat4 ZRenderUtils::getModelMatrix(ZObject* object, ZCamera* camera) {

	if (object->getModelMatrixSet()) {
        return object->getModelMatrix();
    } 
    
    mat4 modelMatrix = object->getTransform();
    
    mat4 parentMat = mat4(1);
    if (object->getParent() != nullptr) {
        parentMat = getModelMatrix(object->getParent(), camera);
    }

    modelMatrix = scale(modelMatrix, object->getScale());
    modelMatrix = translate(modelMatrix, object->getTranslation());
    
    mat4 billboard = mat4(1);

    if (object->isBillboard()) {
        mat4 cameraMat;
        if (camera->isManualView()) {
            cameraMat = inverse(camera->getViewMatrix());
        } else {
            cameraMat = ZRenderUtils::getModelMatrix(camera, nullptr);
        }

        vec4 cameraPos = cameraMat * vec4(0,0,0,1);
        vec4 up = cameraMat * vec4(0,1,0,0);
        vec3 up3 = dvec3(up);
        vec3 cameraPos3 = dvec3(cameraPos);

        vec4 objectCenterTmp = modelMatrix * vec4(object->getOrigin(), 1.0);
        vec3 objectCenter = dvec3(objectCenterTmp);

        mat4 snapToCenter = mat4(1);
        snapToCenter = inverse(translate(snapToCenter, objectCenter));

        mat4 lookAt = glm::lookAt(
            vec3(0), // Camera is at, in World Space
            cameraPos3 - objectCenter, // and looks at the origin
            up3  // Head is up (set to 0,-1,0 to look upside-down)
        );

        mat4 retranslation = mat4(1);
        retranslation = translate(retranslation, objectCenter);
        billboard = retranslation * inverse(lookAt) * snapToCenter;

        return billboard * modelMatrix;
    } else {
        modelMatrix = rotate(modelMatrix, radians(object->getRotationAngle()), object->getRotation());
        modelMatrix = parentMat * modelMatrix;
    }
    
    return modelMatrix;
}