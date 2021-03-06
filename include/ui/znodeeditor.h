//
// Created by lukas on 8/4/20.
//

#ifndef ZPATH_ZNODEEDITOR_H
#define ZPATH_ZNODEEDITOR_H

static const int NO_SELECTION = -1;
static const int SOCKET_DRAG_IN = -2;
static const int SOCKET_DRAG_OUT = -3;
static const int NODE_DRAG = -4;

static const int NO_BOX_SELECT = 0;
static const int BOX_SELECT = 1;
static const int BOX_SELECT_2 = 2;


static const int DEFAULT_NODE_X = 10;
static const int DEFAULT_NODE_Y = 10;
static const int NODE_MARGIN = 10;

static const int NODE_CONTAINER_OFFSET = 30;

static const int DRAG_THRESHOLD = 5;

static const double DEFAULT_ELEVATION = 0.3;

static const double SELECTED_ELEVATION = 1.0;

static const int NO_GROUP = -1;
static const int GROUP_SELECTED = 1;

#include <ui/zview.h>
#include "znodeview.h"
#include "zmagnitudepicker.h"
#include "zcursorview.h"
#include <queue>
#include <set>
#include <mutex>
#include <condition_variable>
#include "zdrawer.h"
#include "ui/ztextfield.h"



class ZNodeEditor : public ZView {
public:
    ZNodeEditor(float maxWidth, float maxHeight, ZView *parent);
    ZNodeView * addNode(ZNodeView::Type type);
    void onMouseDrag(vec2 absolute, vec2 start, vec2 delta, int state, int button) override;
    void onMouseEvent(int button, int action, int mods, int sx, int sy) override;

    // Background evaluation
    set<ZNodeView*> mEvalSet;
    mutex mEvalMutex;
    mutex mNodeMutex;
    condition_variable mEvalConVar;

    static void startEvaluation(ZNodeEditor* editor);
    void onCursorPosChange(double x, double y) override;
    void onLayoutFinished() override;

    void onExit() override;
    void onCreate() override;
    void onKeyPress(int key, int scancode, int action,int mods) override;
private:

    bool mEvaluateRunning = true;
    vector<string> mRadioButtonNames = {"e-6", "e-5", "e-4", "e-3", "0.01", "0.1", "1", "10", "100", "e3", "e4", "e5", "e6"};

    ZLinearLayout* mButtonPanel;
    vector<ZNodeView*> mNodeViews;
    ZLineView* mTmpLine;
    ZMagnitudePicker* mMagnitudePicker = nullptr;
    bool mNodeListInvalid = false;

    ZView* mBoxSelect = nullptr;
    ZCursorView* mCursorView = nullptr;
    ZDrawer* mDrawer = nullptr;

    int mGroupMode = NO_GROUP;

    int mBoxMode = NO_BOX_SELECT;

    bool mGrab = false;
    bool mWasDoubleClick = false;

    set<ZNodeView*> mSelectedNodes;
    ZNodeView* mLastSelected = nullptr;
    ZNodeView* mSecondLastSelected = nullptr;
    ZNodeView* mPendingDeselect = nullptr;

    ZView* mHeader = nullptr;
    ZView* mLineContainer = nullptr;
    ZView* mNodeContainer = nullptr;

    vec2 mInitialOffset{};
    vec2 mInitialSize{};
    int mDragNode = NO_SELECTION;
    int mDragType = NO_SELECTION;
    int mDragSocket = NO_SELECTION;
    vector<ZLineView*> mLineBucket;
    vec2 mAddNodePosition = vec2(DEFAULT_NODE_X, DEFAULT_NODE_Y);

    ZNodeView::Type mLastType = ZNodeView::Type::SIN;

    int mTmpNodeOffsetY = 0;
    int mTmpNodeOffsetX = 0;
    vector<int> mTmpNodeOffsetYR;

    vector<vector<ZNodeView*>> mTmpNodes;
    ZTextField* mExpressionField;
    int mSelectedProject = 0;
    string mProjectPath = "";

    bool isSocketDrag();

    int getMouseOverNode();
    int getMouseOverOutSocket(ZNodeView *node);
    int getMouseOverInSocket(ZNodeView *node);

    void updateLines();

    ZLineView * getLine(int index);

    void onMouseUp(int button);
    void onMouseMove(const vec2 &absolute, const vec2 &delta);

    void onMouseDown();
    void onScrollChange(double x, double y) override;

    template<typename T>
    void remove(vector<T> &vec, size_t pos);

    void connectNodes(int outIndex, int inIndex, ZNodeView *firstNode, ZNodeView *secondNode) const;

    void addTestNodes();

    void enterBoxSelectMode();

    void exitBoxSelectMode();

    void enterBoxSelect2nd();

    void updateBoxSelect();

    void selectNode(ZNodeView *node);

    void deselectNode(ZNodeView *node);

    void deselectAllNodes();

    void deleteSelectedNodes();

    void deleteNode(ZNodeView *);

    void selectAllNodes();

    void duplicateSelectedNodes();

    void enterGrabMode();

    void exitGrabMode();

    void deleteNodeAsync(ZNodeView *node);

    void deleteConnections(ZNodeView *node);

    void deleteSelectedConnections();

    void quickConnectNodes(ZNodeView *firstNode, ZNodeView *secondNode);

    bool wasMouseDrag();

    void resetCursor();

    vector<string> getNodeTypeNames(vector<ZNodeView::Type> types);

    void addNodeToView(ZNodeView *node, bool autoPosition);

    void addNodeGraph(ZNodeView *root, vec2 position, int depth);

    void deleteNodeRecursive(ZNodeView *root);

    void resetNodeInitialPosition();

    void snapViewToNodes();

    void onSizeChange();

    void selectNodeGraph(ZNodeView *root, int depth);

    void onDoubleClick();

    void selectNodeGraphUnderMouse();

    bool clickConsumed() const;

    string replace(string subject, const string &search, const string &replace);

    void toggleGroupSelection();

    void reindexNodes();

};


#endif //ZPATH_ZNODEEDITOR_H
