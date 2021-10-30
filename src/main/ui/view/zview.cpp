#include <utils/zsettingsstore.h>
#include <ui/zviewcontroller.h>
#include <utils/zcornerrenderer.h>
#include <utils/colormode.h>
#include "ui/zview.h"

ZView::ZView(float maxWidth, float maxHeight) {
    setup(maxWidth, maxHeight, this);
}

ZView::ZView(float maxWidth, float maxHeight, vec4 bg, ZView* parent) {
    setup(maxWidth, maxHeight, parent);
    setBackgroundColor(bg);
}

ZView::ZView(vec2 size, ZView* parent) {
    setup(size.x, size.y, parent);
}

ZView::ZView(vec2 size, vec4 bg, ZView* parent) {
    setup(size.x, size.y, parent);
    setBackgroundColor(bg);
}

ZView::ZView(float maxWidth, float maxHeight, ZView* parent) {
    setup(maxWidth, maxHeight, parent);
}

ZView::ZView(float maxWidth, float maxHeight, ZView* parent, bool isScrollable) {
    mIsScrollable = isScrollable;
    setup(maxWidth, maxHeight, parent);
}

void ZView::setup(float maxWidth, float maxHeight, ZView *parent) {
    setOutlineColor(ZSettingsStore::getInstance().getInactiveColor());
    mMaxWidth = maxWidth;
    mMaxHeight = maxHeight;
    mWindowHeight = parent->getWindowHeight();
    mWindowWidth = parent->getWindowWidth();

    mParentView = parent;
    if (parent != this) {
        parent->addSubView(this);
    }

}

void ZView::onKeyPress(int key, int scancode, int action, int mods) {

    bool modifier = false;
    if ((key == GLFW_KEY_LEFT_SHIFT || key == GLFW_KEY_RIGHT_SHIFT) && action == GLFW_PRESS) {
        mShiftKeyPressed = true;
        modifier = true;
    } else if ((key == GLFW_KEY_LEFT_SHIFT || key == GLFW_KEY_RIGHT_SHIFT) && action == GLFW_RELEASE) {
        mShiftKeyPressed = false;
        modifier = true;
    }

    if ((key == GLFW_KEY_LEFT_ALT || key == GLFW_KEY_RIGHT_ALT) && action == GLFW_PRESS) {
        mAltKeyPressed = true;
        modifier = true;
    } else if ((key == GLFW_KEY_LEFT_ALT || key == GLFW_KEY_RIGHT_ALT) && action == GLFW_RELEASE) {
        mAltKeyPressed = false;
        modifier = true;
    } else if ((key == GLFW_KEY_LEFT_CONTROL || key == GLFW_KEY_RIGHT_CONTROL || (key == GLFW_KEY_LEFT_SUPER || key == GLFW_KEY_RIGHT_SUPER)) && action == GLFW_PRESS) {
        mControlKeyPressed = true;
        modifier = true;
    } else if ((key == GLFW_KEY_LEFT_CONTROL || key == GLFW_KEY_RIGHT_CONTROL || (key == GLFW_KEY_LEFT_SUPER || key == GLFW_KEY_RIGHT_SUPER)) && action == GLFW_RELEASE) {
        mControlKeyPressed = false;
        modifier = true;
    }

    for (auto & mView : mViews) {
        if ((isMouseInBounds(mView)) || (action == GLFW_RELEASE && modifier) || key == GLFW_KEY_BACKSPACE) {
            mView->onKeyPress(key, scancode, action, mods);
        }
    }

}

void ZView::onCharacterInput(unsigned int character) {
    for (auto & mView : mViews) {
        //if ((isMouseInBounds(mView))) {
            mView->onCharacterInput(character);
       //ioj }
    }
}

void ZView::setConsumeClicks(bool consume) {
    mConsumeClicks = consume;
}

void ZView::onMouseEvent(int button, int action, int mods, int x, int y) {
    vec2 scale = getScale();
    float sx = x / scale.x;
    float sy = y / scale.y;

    if (getVisibility() && isClickable()) {
        if (!mConsumeClicks) {
            for (int i = 0; i < mViews.size(); i++) {
                if (mViews.size() > i) {
                    ZView *view = mViews.at(i);
                    if (view != nullptr) {

                        if (isMouseInBounds(view)) {
                            view->onMouseEvent(button, action, mods, x, y);
                        } else if (action == GLFW_RELEASE && view->anyMouseDown()) {
                            view->onMouseEvent(button, action, mods, x, y);
                        }
                        if (action == GLFW_RELEASE) {
                            view->onMouseDrag(vec2(sx, sy), vec2(mMouseDownX, mMouseDownY),
                                              vec2(sx - mMouseDownX, sy - mMouseDownY), mouseUp, button);
                        }
                    }
                }
            }
        }

        if (button == GLFW_MOUSE_BUTTON_1 && action == GLFW_RELEASE) {
            mMouseDown = false;
            if (mOnClick != nullptr) {
                mOnClick(this);
            }
        }

        if (button == GLFW_MOUSE_BUTTON_3 && action == GLFW_RELEASE) {
            mMiddleMouseDown = false;
        }

        if (button == GLFW_MOUSE_BUTTON_2 && action == GLFW_RELEASE) {
            mRightMouseDown = false;
        }

        if (button == GLFW_MOUSE_BUTTON_1 && action == GLFW_PRESS) {
            mMouseDown = true;
            mWasClicked = true;
        }

        if (button == GLFW_MOUSE_BUTTON_3 && action == GLFW_PRESS) {
            mMiddleMouseDown = true;
        }

        if (button == GLFW_MOUSE_BUTTON_2 && action == GLFW_PRESS) {
            mRightMouseDown = true;
        }

        if (action == GLFW_PRESS) {
            onMouseDrag(vec2(sx, sy), vec2(mMouseDownX, mMouseDownY),
                        vec2(sx - mMouseDownX, sy - mMouseDownY), mouseDown, button);
            mMouseDownX = sx;
            mMouseDownY = sy;
        }
    }
}

void ZView::onMouseOver() {

}

void ZView::setMiddleMouseDown(bool mouseDown) {
    mMiddleMouseDown = mouseDown;
}

void ZView::setRightMouseDown(bool mouseDown) {
    mRightMouseDown = mouseDown;
}

void ZView::onMouseDrag(vec2 absolute, vec2 start, vec2 delta, int state, int button) {
    if (state == mouseUp) {
        mMouseDragDelta = vec2(0);
    } else if (state == mouseDrag || state == mouseDown) {
        mMouseDragDelta = absolute - start;
    }
}

void ZView::onCursorPosChange(double x, double y) {
    vec2 scale = getScale();
    float sx = x / scale.x;
    float sy = y / scale.y;

    mLastX = mMouseX;
    mLastY = mMouseY;

    mMouseX = (int) sx;
    mMouseY = (int) sy;

    int button = 0;
    if (mouseIsDown()) {
        button = GLFW_MOUSE_BUTTON_1;
    } else if(rightMouseIsDown()) {
        button = GLFW_MOUSE_BUTTON_2;

    } else if (middleMouseIsDown()) {
        button = GLFW_MOUSE_BUTTON_3;
    }

    if (anyMouseDown()) {
        onMouseDrag(vec2(sx, sy), vec2(mMouseDownX, mMouseDownY),
                    vec2(sx - mMouseDownX, sy - mMouseDownY), mouseDrag, button);
    }

    if (!mMouseOver && isMouseInBounds(this)) {
        mMouseOver = true;
        onMouseOver();
    }

    for (int i = 0; i < mViews.size(); i++) {
        if (mViews.size() > i) {
            ZView* view = mViews.at(i);
            view->onCursorPosChange(x, y);
            if (!isMouseInBounds(view) && view->mMouseOver) {
                view->onMouseLeave();
                view->mMouseOver = false;
            }
        }

    }
}

void ZView::setMouseDown(bool isDown) {
    mMouseDown = isDown;
}

bool ZView::isMouseInBounds(ZView *view) const {
    if (view == nullptr || !view->getVisibility()) {
        return false;
    }
    vec2 inner = view->getInnerTranslation();
    bool isInViewX = view->getLeft() - inner.x < view->getMouse().x  && view->getRight() - inner.x > view->getMouse().x ;
    bool isInViewY = view->getTop() - inner.y < view->getMouse().y && view->getBottom() - inner.x > view->getMouse().y;
    return isInViewX && isInViewY;
}

void ZView::onScrollChange(double x, double y) {
    if (getVisibility()) {
        for (auto view : mViews) {
            computeBounds();
            if (isMouseInBounds(view)) {
                view->onScrollChange(x, y);
            }
        }
    }
}

void ZView::onCreate() {
    if (mCreateComplete) {
        return;
    }
    init();

    mCreateComplete = true;
}

string ZView::getTag() {
    return mTag;
}

void ZView::setTag(string tag) {
    mTag = tag;
}

void ZView::draw() {
    initBuffers();

    if (mVisible && mNeedsRender) {
        if (mVertsInvalid) {
            glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffer);
            glBufferSubData(GL_ARRAY_BUFFER, 0, 16 * sizeof(float), mVertices);

            glBindBuffer(GL_ARRAY_BUFFER, mTexBuffer);
            glBufferSubData(GL_ARRAY_BUFFER, 0, 8 * sizeof(float), mTexCoords);
            mVertsInvalid = false;
        }

        redrawCornerRadius();

        glViewport(0, 0, mWindowWidth * mDP, mWindowHeight * mDP);

        // Update scale, useful for zooming a view out
        mat4 projection = ortho(0.0f, (float)
            mWindowWidth, (float) mWindowHeight,
        0.0f, -1.0f, 10.0f);
        vec2 absoluteScale = getScale();
        mat4 scaleMat = scale(projection, vec3(absoluteScale.x, absoluteScale.y, 0));

        vec4 backgroundColor = max(vec4(0), mBackgroundColor.get(mColorMode));

        if (mShowHighlight) {
            backgroundColor = mHighlightColor.get(mColorMode);
        }


        drawShadow();

//        if (glm::length(mCornerRadius) > 0) {
//            //updateCornerRadius();
//        } else

        if (length(mCornerRadius) < 0.01) {
            mShader->use();
            glUniformMatrix4fv(glGetUniformLocation(mShader->mID, "uVPMatrix"), 1, GL_FALSE, glm::value_ptr(scaleMat));

            // Set the view background color
            glUniform4f(glGetUniformLocation(mShader->mID, "uColor"),
                        backgroundColor.r, backgroundColor.g, backgroundColor.b, backgroundColor.a);
            glLineWidth(mLineWidth);
            if (mDrawWire == full) {
                glBindVertexArray(mEdgeVAO);
                glDrawElements(GL_LINES, EDGE_INDEX_COUNT, GL_UNSIGNED_INT, nullptr);
            } else if (mDrawWire == outline) {
                glUniform4f(glGetUniformLocation(mShader->mID, "uColor"),
                            backgroundColor.r, backgroundColor.g, backgroundColor.b, backgroundColor.a);

                glBindVertexArray(mVAO);
                glDrawElements(GL_TRIANGLES, FACE_INDEX_COUNT, GL_UNSIGNED_INT, nullptr);

                vec4 outlineColor = mOutlineColor.get(mColorMode);
                glBindVertexArray(mOutlineVAO);
                glUniform4f(glGetUniformLocation(mShader->mID, "uColor"),
                            outlineColor.r, outlineColor.g, outlineColor.b, outlineColor.a);

                glDrawElements(GL_LINES, OUTLINE_INDEX_COUNT, GL_UNSIGNED_INT, nullptr);

                glBindVertexArray(mVAO);
            } else {
                glBindVertexArray(mVAO);
                glDrawElements(GL_TRIANGLES, FACE_INDEX_COUNT, GL_UNSIGNED_INT, nullptr);
            }
        } else if (mRoundedRect != nullptr) {
            mImageShader->use();

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, mRoundedRect->getID());

            mImageShader->setVec4("uTint", white.get(mColorMode));

            // Update scale, useful for zooming a view out
            glUniformMatrix4fv(glGetUniformLocation(mImageShader->mID, "uVPMatrix"), 1,
                               GL_FALSE, glm::value_ptr(scaleMat));

            glBindVertexArray(mVAO);
            glDrawElements(GL_TRIANGLES, FACE_INDEX_COUNT, GL_UNSIGNED_INT, nullptr);
        }

        if (mBackgroundImage != nullptr) {
            mImageShader->use();

            mImageShader->setVec4("uTint", mTint.get(mColorMode));

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, mBackgroundImage->getID());

            // Update scale, useful for zooming a view out
            glUniformMatrix4fv(glGetUniformLocation(mImageShader->mID, "uVPMatrix"), 1,
                    GL_FALSE, glm::value_ptr(scaleMat));

            glBindVertexArray(mVAO);
            glDrawElements(GL_TRIANGLES, FACE_INDEX_COUNT, GL_UNSIGNED_INT, nullptr);
        }

        glBindVertexArray(0);
        glBindTexture(GL_TEXTURE_2D, 0);

        for (ZView* view : mViews) {
            if (view != mShadowView) {
                view->draw();
            }
        }
        mNeedsRender = false;
    }
}

void ZView::updateCornerRadius() {
    mRadiusInvalid = true;
}

void ZView::redrawCornerRadius() {
    if (mRadiusInvalid) {
        mRadiusInvalid = false;
        if (length(mCornerRadius) > 0) {
            if (mRoundedRect == nullptr) {
                auto tex = ZCornerRenderer::get().
                        createTexture(getWidth() * mDP, getHeight() * mDP,
                                      max(vec4(0), getBackgroundColor().get(mColorMode)),
                                      mOutlineColor.get(mColorMode), mLineWidth * mDP, mCornerRadius * mDP);
                mRoundedRect = tex;
            } else {
                ZCornerRenderer::get().
                        draw(getWidth() * mDP, getHeight() * mDP,
                             mCornerRadius * mDP,
                             max(vec4(0), getBackgroundColor().get(mColorMode)),
                             mOutlineColor.get(mColorMode), mLineWidth * mDP, mRoundedRect, false);
            }
        }

    }
}

void ZView::drawShadow() {
    if (mElevation < 0.01) {
        return;
    }

    float shadowRadius = 30 * mElevation;
    float offset = -5 * mElevation;
    if (mElevation > 0 && mShadowView == nullptr) {

        mShadowView = new ZView(getMaxWidth() + shadowRadius, getMaxWidth() + shadowRadius, this, false);
        mShadowView->setClippingEnabled(false);
        mShadowView->setBackgroundColor(transparent);
        // Shadow view should be invisible so it doesn't render as a child view
        ZTexture *shadow = ZShadowRenderer::get().createShadow(getWidth(), getHeight(), shadowRadius);
        //mShadowView->setVisibility(false);
        mShadowView->setBackgroundImage(shadow);
        mShadowView->setOffset(offset, offset);
        mShadowView->setGravity(topLeft);
       // mShadowView->setBackgroundColor(blue);
    }

    if (mShadowView != nullptr && mShadowView->mParentView != nullptr) {
        if ((abs((mShadowView->getMaxWidth() - shadowRadius) - getWidth()) > 1 ||
            abs((mShadowView->getMaxHeight() - shadowRadius) - getHeight()) > 1) && getWidth() > 1 && getHeight() > 1) {
            ZShadowRenderer::get().updateShadow(mShadowView->mBackgroundImage->getID(), getWidth(), getHeight(), shadowRadius);
            mShadowView->setMaxWidth(getWidth() + shadowRadius);
            mShadowView->setMaxHeight(getHeight() + shadowRadius);
            mShadowView->setOffset(offset, offset);
        }

        if (getWidth() > 1 && getHeight() > 1) {
            mShadowView->draw();
        }
    }
}

float ZView::getElevation() {
    return mElevation;
}

void ZView::init() {
    computeBounds();
}

void ZView::initBuffers() {
    if (!mBuffersInit) {
        mBuffersInit = true;
        glGenBuffers(1, &mVertexBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffer);
        glBufferData(GL_ARRAY_BUFFER, 16 * sizeof(float), &mVertices, GL_STATIC_DRAW);

        glGenBuffers(1, &mFaceIndicesBuffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mFaceIndicesBuffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, FACE_INDEX_COUNT * sizeof(int), &mFaceIndices, GL_STATIC_DRAW);

        glGenBuffers(1, &mEdgeIndicesBuffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEdgeIndicesBuffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, EDGE_INDEX_COUNT * sizeof(int), &mEdgeIndices, GL_STATIC_DRAW);

        glGenBuffers(1, &mOutlineIndicesBuffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mOutlineIndicesBuffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, OUTLINE_INDEX_COUNT * sizeof(int), &mOutlineIndices, GL_STATIC_DRAW);

        glGenBuffers(1, &mTexBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, mTexBuffer);
        glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(float), mTexCoords, GL_STATIC_DRAW);

        glGenVertexArrays(1, &mVAO);
        glBindVertexArray(mVAO);
        glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffer);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 4, nullptr);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mFaceIndicesBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, mTexBuffer);
        glBindVertexArray(0);

        glGenVertexArrays(1, &mEdgeVAO);
        glBindVertexArray(mEdgeVAO);
        glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffer);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 4, nullptr);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEdgeIndicesBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, mTexBuffer);
        glBindVertexArray(0);

        glGenVertexArrays(1, &mOutlineVAO);
        glBindVertexArray(mOutlineVAO);
        glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffer);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 4, nullptr);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mOutlineIndicesBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, mTexBuffer);
        glBindVertexArray(0);
    }
}

void ZView::setShader(ZShader *shader) {
    mShader = shader;


    for (auto & mView : mViews) {
        mView->setShader(shader);
    }
}

void ZView::setTextShader(ZShader *shader) {
    mTextShader = shader;
    for (auto & mView : mViews) {
        mView->setTextShader(shader);
    }
}

ZShader* ZView::getTextShader() {
    return mTextShader;
}

void ZView::setImageShader(ZShader *shader) {
    mImageShader = shader;
    for (auto & mView : mViews) {
        mView->setImageShader(shader);
    }
}

ZShader* ZView::getImageShader() {
    return mImageShader;
}

void ZView::setWindowWidth(int width) {
    mWindowWidth = width;
    for (auto & mView : mViews) {
        mView->setWindowWidth(width);
    }
}

void ZView::setWindowHeight(int height) {
    mWindowHeight = height;
    for (auto & mView : mViews) {
        mView->setWindowHeight(height);
    }
}

void ZView::onWindowChange(int windowWidth, int windowHeight) {
    if (mParentView == this) {
        mMaxWidth = windowWidth;
        mMaxHeight = windowHeight;
    }
    
    computeBounds();
    for (int i = 0; i < mViews.size(); i++) {
        if (i < mViews.size() && mViews.at(i)->getVisibility()) {
            mViews.at(i)->onWindowChange(right, bottom);
        }
    }

    invalidate();
}

void ZView::onDpChange(float dp) {
    mDP = dp;
    for (ZView* view : mViews) {
        view->onDpChange(dp);
    }
}

// Called once before first draw
void ZView::onLayoutFinished(){
    for (ZView* view : mViews) {
        view->onLayoutFinished();
    }
}

void ZView::setMargin(int marginLeft, int marginTop, int marginRight, int marginBottom) {

    mMarginLeft = marginLeft;
    mMarginTop = marginTop;
    mMarginRight = marginRight;
    mMarginBottom = marginBottom;

    computeBounds();
    invalidate();
}

void ZView::setMargin(vec4 margin) {
    mMarginLeft = margin.x;
    mMarginTop = margin.y;
    mMarginRight =  margin.z;
    mMarginBottom =  margin.w;

    computeBounds();
    invalidate();
}

void ZView::setOffset(double x, double y) {
    mOffsetX = x;
    mOffsetY = y;
    computeBounds();
    invalidate();
}

void ZView::setOffset(vec2 pos) {
    setOffset(pos.x, pos.y);
}

void ZView::setXOffset(int x) {
    mOffsetX = x;
    invalidate();
}

void ZView::setYOffset(int y) {
    mOffsetY = y;
    invalidate();
}

void ZView::offsetBy(int x, int y) {
    mOffsetX += x;
    mOffsetY += y;
    computeBounds();
    invalidate();
}

double ZView::getOffsetX() {
    return mOffsetX;
}

double ZView::getOffsetY() {

    return mOffsetY;
}

int ZView::getMaxWidth() {
    return mMaxWidth;
}

int ZView::getMaxHeight() {
    return mMaxHeight;
}

int ZView::getHeight() {
    return getBottom() - getTop();
}

int ZView::getWidth() {
    return getRight() - getLeft();
}

int ZView::getMarginLeft() {
    return mMarginLeft;
}

int ZView::getMarginTop() {
    return mMarginTop;
}

int ZView::getMarginRight() {
    return mMarginRight;
}

int ZView::getMarginBottom() {
    return mMarginBottom;
}

void ZView::setMarginLeft(float m){
    mMarginLeft = m;
}

void ZView::setMarginTop(float m){
    mMarginTop = m;
}

void ZView::setMarginRight(float m){
    mMarginRight = m;
}

void ZView::setMarginBottom(float m){
    mMarginBottom = m;
}

void ZView::invalidate() {
    mNeedsRender = true;
    for (ZView* view : mViews) {
        view->invalidate();
    }
    //glfwPostEmptyEvent();
}

vec2 ZView::getCenter() {
    return vec2(((getRight() - getLeft()) / 2.0) + getLeft(),
            ((getBottom() - getTop()) / 2.0) + getTop());
}

void ZView::setBackgroundImage(ZTexture* background) {
    mBackgroundImage = background;
}

int ZView::getWindowHeight() {
    return mWindowHeight;
}

int ZView::getWindowWidth() {
    return mWindowWidth;
}

void ZView::computeBounds() {
    float prevLeft = getLeft();
    float prevRight = getRight();

    float prevTop = getTop();
    float prevBottom = getBottom();

    float prevWidth = prevRight - prevLeft;
    float prevHeight = prevBottom - prevTop;

    calculateBounds();

    mVertices[0] = getLeft();
    mVertices[1] = getTop();

    mVertices[4] = getRight();
    mVertices[5] = getTop();

    mVertices[8] = getLeft();
    mVertices[9] = getBottom();

    mVertices[12] = getRight();
    mVertices[13] = getBottom();

    mVertsInvalid = true;

    if (abs(getWidth() - prevWidth) > 0.01 || abs(getHeight() - prevHeight) > 0.01) {
        onSizeChange();
    }
}

int ZView::getLocalTop() {
    return getOffsetY();
}
int ZView::getLocalBottom() {
    return getOffsetY() + getMaxHeight();
}

int ZView::getLocalLeft() {
    return getOffsetX();
}

int ZView::getLocalRight() {
    return getOffsetX() + getMaxWidth();
}

void ZView::calculateBounds() {
    calculateLeft();
    calculateTop();
    calculateBottom();
    calculateRight();
}

void ZView::addSubView(ZView *view) {
    mViews.push_back(view);
    if (view->getParentView() != nullptr && view->getParentView() != view &&
            view->getParentView() != this) {
        view->getParentView()->removeSubView(view);
    }
    view->setParentView(this);
    view->setRootView(mRootView);
    view->onDpChange(mDP);
    view->setColorMode(mColorMode);
    view->updateCornerRadius();
    if (mShader != nullptr) {
        view->setShader(mShader);
        view->setTextShader(mTextShader);
        view->setImageShader(mImageShader);
    }

    if (!view->mCreateComplete) {
        view->onCreate();
    }

    view->onLayoutFinished();
    cacheScale();
    calculateBounds();
}

void ZView::clearSubViews() {
    for (auto & mView : mViews) {
        mView = NULL;
    }

    mViews.clear();
    mViews.shrink_to_fit();
}

void ZView::setParentView(ZView *parentView) {
    mParentView = parentView;
}

ZView* ZView::getParentView() {
    return mParentView;
}

bool ZView::mouseIsDown() {
    return mMouseDown;
}

bool ZView::middleMouseIsDown() {
    return mMiddleMouseDown;
}

bool ZView::rightMouseIsDown() {
    return mRightMouseDown;
}

bool ZView::shiftKeyPressed() {
    return mShiftKeyPressed;
}

bool ZView::altKeyPressed() {
    return mAltKeyPressed;
}

bool ZView::controlKeyPressed() {
    return mControlKeyPressed;
}

int ZView::getMouseDownX() {
    return mMouseDownX;
}

int ZView::getMouseDownY() {
    return mMouseDownY;
}

vec2 ZView::getMouse() {
    return vec2(mMouseX, mMouseY);
}

vec2 ZView::getRelativeMouse() {
    return vec2(mMouseX - getLeft(), mMouseY - getTop());
}

double ZView::getLastX() {
    return mLastX;
}

double ZView::getLastY() {
    return mLastY;
}

vector<ZView*> ZView::getSubViews() {
    return mViews;
}


void ZView::setBackgroundColor(float color[4]) {
    mBackgroundColor = vec4(color[0], color[1], color[2], color[3]);
    updateCornerRadius();
    invalidate();
}

void ZView::setBackgroundColor(ZColor color) {
    mBackgroundColor = color;
    updateCornerRadius();
    invalidate();
}

ZColor ZView::getBackgroundColor() {
    return mBackgroundColor;
}

void ZView::setGravity(Gravity gravity) {
    mGravity = gravity;
    invalidate();
}

void ZView::setMaxWidth(int width) {
    mMaxWidth = width;
    computeBounds();
}

void ZView::setMaxHeight(int height) {
    mMaxHeight = height;
    computeBounds();
}

void ZView::onSizeChange() {
    updateCornerRadius();
}

bool ZView::anyMouseDown() {
    return mouseIsDown() || middleMouseIsDown() || rightMouseIsDown();
}

void ZView::onFileDrop(int count, const char** paths) {
    for (auto & mView : mViews) {
        mView->onFileDrop(count, paths);
    }
}

void ZView::onExit() {
    for (auto & mView : mViews) {
        mView->onExit();
    }
}

void ZView::setVisibility(bool visible) {
    mVisible = visible;
    invalidate();
}

bool ZView::getVisibility() {
    return mVisible;
}

bool ZView::needsRender() {
    return mNeedsRender;
}

void ZView::bringToFront() {
    ZView* parent = getParentView();
    parent->removeSubView(this);
    parent->addSubView(this);
}

int ZView::calculateLeft() {
    int thisLeft = mOffsetX + mMarginLeft;
    vec2 translation = getInnerTranslation();
    float scale = getRelativeScale().x;
    float parentLeft = mParentView->getLeft() / scale + translation.x;
    float parentRight = mParentView->getRight() / scale + translation.x;

    if (mParentView == this) {
        mLeft = thisLeft;
        return mLeft;
    } else {
        if (mGravity == bottomRight || mGravity == topRight) {
            thisLeft = parentRight - mMarginRight - mMaxWidth - mOffsetX;

            if (mClipping && thisLeft - mMarginLeft < parentLeft) {
                thisLeft = parentLeft + mMarginLeft;
            }

            mLeft = thisLeft;
            return mLeft;
        } else {
            if (!mAllowNegativeSize) {
                if (mClipping && thisLeft + parentLeft > parentRight) {
                    mLeft = parentRight;
                    return mLeft;
                }
            }

            if (mClipping && thisLeft + mParentView->getInnerTranslation().x < 0) {
                mLeft = (int) (parentLeft - mParentView->getInnerTranslation().x);
                return mLeft;
            }

            mLeft = parentLeft + thisLeft;
            return mLeft;
        }
    }
}

int ZView::calculateRight() {
    int thisRight = mMaxWidth + mMarginLeft + mOffsetX;

    vec2 translation = getInnerTranslation();
    float scale = getRelativeScale().x;
    float parentLeft = mParentView->getLeft() / scale + translation.x;
    float parentRight = mParentView->getRight() / scale + translation.x;

    if (mParentView == this) {
        mRight = thisRight;
        return mRight;
    } else {
        if (mGravity == topRight || mGravity == bottomRight) {
            thisRight = parentRight - mMarginRight - mOffsetX;

            if (!mAllowNegativeSize) {
                if (mClipping && thisRight < parentLeft + mMarginLeft) {
                    thisRight = parentLeft + mMarginLeft;
                }
            }
            mRight = thisRight;
            return mRight;
        }

        if (!mClipping || thisRight + parentLeft + mMarginRight +
            mParentView->getInnerTranslation().x < parentRight) {
            thisRight = parentLeft + thisRight;
        } else {
            thisRight = parentRight - mMarginRight - mParentView->getInnerTranslation().x;
        }

        if (!mAllowNegativeSize) {
            if (thisRight < getLeft()) {
                thisRight = getLeft();
            }
        }

        mRight = thisRight;
        return mRight;
    }

}

int ZView::calculateTop() {
    int thisTop = (int) (mOffsetY + mMarginTop + mParentView->getTranslation().y);

    vec2 translation = getInnerTranslation();
    float parentTop = mParentView->getTop() / getRelativeScale().y  + translation.y;
    float parentBottom = mParentView->getBottom() / getRelativeScale().y + translation.y;

    if (mParentView == this) {
        mTop = thisTop;
        return mTop;
    } else {
        if (mGravity == bottomRight || mGravity == bottomLeft) {
            thisTop = (int) (parentBottom - mMarginBottom - mMaxHeight - mOffsetY);

            if (mClipping && thisTop - mMarginTop < parentTop) {
                thisTop = (int) parentTop + mMarginTop;
            }

            mTop = thisTop;
            return mTop;
        } else {

            if (!mAllowNegativeSize) {
                if (mClipping && thisTop + parentTop > parentBottom) {
                    mTop = (int) parentBottom;
                    return mTop;
                }
            }

            if (mClipping && thisTop + mParentView->getInnerTranslation().y < 0) {
                mTop = (int) (parentTop - mParentView->getInnerTranslation().y);
                return mTop;
            }
            mTop = (int) parentTop + thisTop;
            return mTop;
        }
    }
}

int ZView::calculateBottom() {
    vec2 translation = getInnerTranslation();
    float parentTop = mParentView->getTop() / getRelativeScale().y  + translation.y;
    float parentBottom = mParentView->getBottom() / getRelativeScale().y + translation.y;

    int thisBottom = (int) (mMaxHeight + mMarginTop + mOffsetY + mParentView->getTranslation().y);

    if (mParentView == this) {
        mBottom = thisBottom;
        return mBottom;
    } else {
        if (mGravity == bottomLeft || mGravity == bottomRight) {
            thisBottom = parentBottom - mMarginBottom - mOffsetY;

            if (mClipping && thisBottom < parentTop + mMarginTop) {
                thisBottom = parentTop + mMarginTop;
            }
            mBottom = thisBottom;
            return mBottom;
        }

        if (!mClipping || thisBottom + parentTop + mMarginTop +
            mParentView->getInnerTranslation().y < parentBottom) {
            thisBottom = parentTop + thisBottom;
        } else {
            thisBottom = parentBottom - mMarginBottom -
                    mParentView->getInnerTranslation().y;
        }


        if (!mAllowNegativeSize) {
            if (thisBottom < getTop()) {
                thisBottom = getTop();
            }
        }

        mBottom = thisBottom;
        return thisBottom;
    }
}

int ZView::getLeft() {
    return mLeft;
}

int ZView::getTop() {
    return mTop;
}

int ZView::getRight() {
   return mRight;
}

int ZView::getBottom() {
    return mBottom;
}

vec4 ZView::getMargin() {
    return vec4(mMarginLeft, mMarginTop, mMarginRight, mMarginBottom);
}

void ZView::onGlobalMouseUp(int key) {
    for (auto view : mViews) {
        view->onGlobalMouseUp(key);
    }

    if (key == GLFW_MOUSE_BUTTON_1) {
        mMouseDown = false;
    }

    if (key == GLFW_MOUSE_BUTTON_2) {
        mRightMouseDown = false;
    }

    if (key == GLFW_MOUSE_BUTTON_3) {
        mMiddleMouseDown = false;
    }
}

/**
 * Sets the outline type. Set to none to
 * draw background.
 * @param wire Full, outline, none
 */
void ZView::setOutlineType(WireType wire) {
    mDrawWire = wire;
}

float* ZView::getVertices() {
    return mVertices;
}

GLuint ZView::getVertexBuffer() {
    return mVertexBuffer;
}

int ZView::getIndexTag() {
    return mIndexTag;
}

void ZView::setIndexTag(int index) {
    mIndexTag = index;
}

void ZView::resetInitialPosition() {
    mInitialPosition = vec2(mOffsetX, mOffsetY);
}

vec2 ZView::getInitialPosition() {
    return mInitialPosition;
}

bool ZView::isClickable() {
    return mClickable;
}

void ZView::setClickable(bool clickable) {
    mClickable = clickable;
}

vec2 ZView::getMouseDragDelta() {
    return mMouseDragDelta;
}

void ZView::setInitialPosition(vec2 position) {
    mInitialPosition = position;
}

void ZView::setOutlineColor(ZColor color) {
    mOutlineColor = color;
    updateCornerRadius();
}

vec2 ZView::getScale() {
    return mScaleCache;
}

void ZView::setScale(vec2 scale) {
    mScale = scale;
    cacheScale();
}

void ZView::cacheScale() {
    if (mParentView != nullptr && mParentView != this) {
        mScaleCache = mScale * mParentView->getScale();
    } else {
        mScaleCache = mScale;
    }

    for(ZView* view : getSubViews()) {
        view->cacheScale();
    }
}

vec2 ZView::getInnerTranslation() {
    return mInnerTranslation;
}
void ZView::setInnerTranslation(vec2 scale) {
    mInnerTranslation = scale;
}

vec2 ZView::getRelativeScale() {
    return mScale;
}

vec2 ZView::getOffset() {
    return vec2(getOffsetX(), getOffsetY());
}

ZView* ZView::getRootView() {
    return mRootView;
}

void ZView::setRootView(ZView* rootView) {
    mRootView = rootView;
}

void ZView::incrementZCursor(){
    mZCursorPosition += 0.05;
}

float ZView::getZPosition(){
    return mZPosition;
}

void ZView::setZPosition(float z){
    mZPosition = z;
}

float ZView::getZCursorPosition(){
    return mZCursorPosition;
}

void ZView::setName(string name) {
    mName = name;
}

string ZView::getName() {
    return mName;
}

void ZView::setLineWidth(float width) {
    mLineWidth = width;
    updateCornerRadius();
}

void ZView::setAllowNegativeSize(bool allow) {
    mAllowNegativeSize = allow;
}

vec2 ZView::getSize() {
    return vec2(getMaxWidth(), getMaxHeight());
}

void ZView::removeView() {
    for (ZView* child : mViews) {
        child->removeView();
    }

    ZView* parent = getParentView();

    if (parent != nullptr) {
        int index = 0;
        if (parent != this) {
            for (ZView *view : parent->getSubViews()) {
                if (view == this) {
                    break;
                }
                index++;
            }


            parent->removeSubView(index);
        }
        mParentView = nullptr;
       // delete this;
    }
}

void ZView::clearFocus() {
    for (ZView* view : mViews) {
        view->clearFocus();
    }

    releaseFocus(this);
}

void ZView::removeSubView(ZView* view) {
    int index = 0;
    releaseFocus(view);
    for (ZView* aView : mViews) {
        if (aView == view) {
            removeSubView(index);
            aView->setParentView(nullptr);
            break;
        }

        index++;
    }
}

void ZView::removeSubView(int index) {
    auto it = mViews.begin();
    std::advance(it, index);
    mViews.erase(it);
}

ZView::~ZView(){}

ZView* ZView::getFocusedView() {
    return getParentView()->getFocusedView();
}

bool ZView::isViewInFocus() {
    return getParentView()->isViewInFocus();
}

void ZView::requestFocus(ZView* view) {
    getParentView()->requestFocus(view);
}

void ZView::releaseFocus(ZView *forView) {
    getParentView()->releaseFocus(forView);
}

void ZView::onFocusChanged(ZView *viewWithFocus) {
    for (ZView* child : getSubViews()) {
        child->onFocusChanged(viewWithFocus);
    }
}

void ZView::translateBy(vec2 translation) {
    mTranslation += translation;
}
void ZView::setTranslation(vec2 translation) {
    mTranslation = translation;
}

vec2 ZView::getTranslation() {
    return mTranslation;
}

ZView::Gravity ZView::getGravity() {
    return mGravity;
}

void ZView::onMouseLeave() {

}

void ZView::setComsumeClicks() {

}

void ZView::onDoubleClick() {
    for (ZView* child : getSubViews()) {
        child->onDoubleClick();
    }
}

void ZView::setColorMode(ColorMode colorMode) {
    for (auto view : mViews) {
        view->setColorMode(colorMode);
    }

    mColorMode = colorMode;
}
