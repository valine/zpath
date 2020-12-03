//
// Created by lukas on 8/4/20.
//

#ifndef ZPATH_ZNODEEDITOR_H
#define ZPATH_ZNODEEDITOR_H


static const int NO_SELECTION = -1;
static const int SOCKET_DRAG_IN = -2;
static const int SOCKET_DRAG_OUT = -3;
static const int NODE_DRAG = -4;

static const int NODE_WIDTH = 80;
static const int NODE_HEIGHT = 100;
static const int DEFAULT_NODE_X = 10;
static const int DEFAULT_NODE_Y = 10;
static const int NODE_MARGIN = 10;

static const int NODE_CONTAINER_OFFSET = 30;

#include <ui/zview.h>
#include "nodeview.h"
#include "zmagnitudepicker.h"
#include <queue>
#include <set>
#include <mutex>
#include <condition_variable>

class ZNodeEditor : public ZView {
public:
    ZNodeEditor(float maxWidth, float maxHeight, ZView *parent);
    void addNode(ZNodeView::Type type);
    void onMouseDrag(vec2 absolute, vec2 start, vec2 delta, int state) override;

    // Background evaluation
    queue<ZNodeView*> mEvalQueue;
    set<ZNodeView*> mEvalSet;
    bool mRunEvaluation = true;
    mutex mEvalMutex;
    condition_variable mEvalConVar;

    static void startEvaluation(ZNodeEditor* editor);
private:

    vector<ZNodeView*> mNodeViews;
    ZLineView* mTmpLine;
    ZMagnitudePicker* mMagnitudePicker;

    ZView* mHeader;
    ZView* mLineContainer;
    ZView* mNodeContainer;

    vec2 mInitialOffset{};
    vec2 mInitialSize{};
    int mDragNode = 0;
    int mDragType = NO_SELECTION;
    int mDragSocket = NO_SELECTION;
    vector<ZLineView*> mLineBucket;
    vec2 mAddNodePosition = vec2(DEFAULT_NODE_X, DEFAULT_NODE_Y);

    ZNodeView::Type mLastType = ZNodeView::Type::ADD;

    bool isSocketDrag();

    int getMouseOverNode();
    int getMouseOverOutSocket(ZNodeView *node);
    int getMouseOverInSocket(ZNodeView *node);

    void updateLines();

    ZLineView * getLine(int index);

    void onMouseUp();
    void onMouseMove(const vec2 &absolute, const vec2 &delta);

    void onMouseDown();
    void onScrollChange(double x, double y) override;
    void onKeyPress(int key, int scancode, int action, int mods) override;

    template<typename T>
    void remove(vector<T> &vec, size_t pos);

    bool needsEval();
};


#endif //ZPATH_ZNODEEDITOR_H
