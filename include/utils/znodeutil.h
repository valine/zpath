//
// Created by lukas on 1/18/21.
//

#ifndef ZPATH_ZNODEUTIL_H
#define ZPATH_ZNODEUTIL_H

class ZLabel;

#include <queue>
#include <set>
#include "ui/znodeview.h"
#include <string>

using namespace std;

class ZNodeUtil {

private:

    map<string, ZNodeView::Type> mTypes;
    set<string> mFunctions;
    /**
     * Pull from delete nodes instead of creating new one
     */
    queue<ZNodeView*> mDeleteNodes;

public:
    static ZNodeUtil& get(){
        static ZNodeUtil instance; // Guaranteed to be destroyed.
        // Instantiated on first use.
        return instance;
    }

    //////////////////////
    /// Node IO

    string nodeToString(ZNodeView* node) {

        ZNodeView::Type type = node->getType();

        string nodeString;
        nodeString+="type:" + ZNodeView::getName(type) + "\n";
        nodeString+="id:" + to_string(node->getIndexTag()) + "\n";
        nodeString+="pos:" + to_string(node->getOffsetX()) + "," +  to_string(node->getOffsetY()) + "\n";
        nodeString+="size:" + to_string(node->getMaxWidth()) + "," +  to_string(node->getMaxHeight()) + "\n";

        string edges;
        for (const auto& socket : node->mInputIndices) {
            for (auto socketInput : socket) {
                ZNodeView* prevNode = socketInput.first;
                int prevNodeIndex = socketInput.second;

                edges+="n" + to_string(prevNode->getIndexTag());
                edges+="s" + to_string(prevNodeIndex);
            }
            edges += "|";
        }

        if (!edges.empty()) {
            edges.pop_back();
            nodeString+="edges:";
            nodeString+=edges;
            nodeString+="\n";
        }

        if (node->getSocketCount().x > 0) {
            nodeString += "defaultin:";
            for (int i = 0; i < node->getSocketCount().x; i++) {
                nodeString += to_string(node->getConstantInput(i)) + ",";
            }
            if (!node->getConstantInputs().empty()) {
                nodeString.pop_back();
            }
            nodeString += "\n";
        }

        if (node->getSocketCount().y > 0) {
            nodeString += "defaultout:";
            for (int i = 0; i < node->getSocketCount().y; i++) {
                nodeString += to_string(node->getConstantValue(i)) + ",";
            }
            if (!node->getConstantInputs().empty()) {
                nodeString.pop_back();
            }
            nodeString += "\n";
        }

        if (node->getSocketCount().x > 0) {
            nodeString += "mag:";
            for (int i = 0; i < node->getSocketCount().x; i++) {
                nodeString += to_string(node->getInputMagnitude(i)) + ",";
            }
            nodeString += "\n";
        }
        if (node->getSocketCount().y > 0) {
            nodeString += "magOut:";
            for (int i = 0; i < node->getSocketCount().y; i++) {
                nodeString += to_string(node->getOutputMagnitude(i)) + ",";
            }
            nodeString += "\n";
        }

        // Parse inner group node
        if (type == ZNodeView::Type::GROUP) {
            nodeString+="group:";

            auto groupNodes = node->getGroupNodes();
            for (auto gnode : groupNodes) {
                nodeString+=to_string(gnode->getIndexTag())+",";
             }
            nodeString.pop_back();
            nodeString+="\n";
        }

        if (!node->getText().empty()) {
            nodeString+="name:";
            nodeString+=escapeText(node->getText());
            nodeString+="\n";
        }

        return nodeString;
    }

    string escapeText(string text) {
        string escape = "\\";
        findAndReplace(text, "\n", escape + "\n");
        findAndReplace(text, ":", escape + ":");
        findAndReplace(text, "=", escape + "=");
        findAndReplace(text, ",", escape + ",");
        return text;
    }

    string serialize(set<ZNodeView*> nodes) {
        string sep = "=\n";

        string graph;
        for (auto node : nodes) {
            graph += sep;
            graph += nodeToString(node);
        }
        return graph;
    }

    set<ZNodeView*> deserialize(string nodeString) {
        vector<string> snodes = split(nodeString, '=');

        map<int, ZNodeView*> nodeMap;
        map<int, string> edgeMap;
        set<ZNodeView*> nodes;
        for (string snode : snodes) {
            if (snode.empty()) {
                continue;
            }

            // String to node
            vector<string> attrs = split(snode, '\n');
            ZNodeView::Type type = ZNodeView::Type::LAST;
            vec2 pos;
            vec2 size;
            vector<float> inputs;
            vector<float> outputs;
            vector<float> magnitudesIn;
            vector<float> magnitudesOut;
            string name = "";
            int id;
            string edges;
            for (const auto& attr : attrs) {
                if (attr.empty()) {
                    continue;
                }

                vector<string> keyvalue = split(attr, ':');
                string key = keyvalue.at(0);
                string value = keyvalue.at(1);

                if (key == "type") {
                    type = mTypes.at(value);
                } else if (key == "pos") {
                    vector<string> spos = split(value, ',');
                    pos = vec2(stoi(spos.at(0)), stoi(spos.at(1)));
                } else if (key == "size") {
                    vector<string> ssize = split(value, ',');
                    size = vec2(stoi(ssize.at(0)), stoi(ssize.at(1)));
                } else if (key == "id") {
                    id = stoi(value);
                } else if (key == "edges") {
                    edges = value;
                } else if (key == "defaultin") {
                    vector<string> inputsS = split(value, ',');
                    for (const string& input : inputsS) {
                        inputs.push_back(stof(input));
                    }
                } else if (key == "defaultout") {
                    vector<string> outputS = split(value, ',');
                    for (const string& output : outputS) {
                        outputs.push_back(stof(output));
                    }
                } else if (key == "mag") {
                    vector<string> magnitudesS = split(value, ',');
                    for (const string& magnitude : magnitudesS) {
                        magnitudesIn.push_back(stoi(magnitude));
                    }
                } else if (key == "magOut") {
                    vector<string> magnitudesS = split(value, ',');
                    for (const string& magnitude : magnitudesS) {
                        magnitudesOut.push_back(stoi(magnitude));
                    }
                } else if (key == "name") {

                    if (!value.empty()) {
                        name = value;
                    }
                }
            }

            ZNodeView* node = newNode(type);
            node->setOffset(pos);
            node->setMaxWidth(size.x);
            node->setMaxHeight(size.y);
            node->setIndexTag(id);
            node->setText(name);
            nodeMap.insert({id, node});
            edgeMap.insert({id, edges});

            // Node constant inputs
            int index = 0;
            for (float input : inputs) {
                int magnitude = 1;

                // Older versions don't have magnitude so check for it exists
                if (magnitudesIn.size() > index) {
                    magnitude = magnitudesIn.at(index);
                }

                node->setConstantValueInput(index, input, magnitude);
                index++;
            }

            // Node constant outputs
            int oindex = 0;
            for (float output : outputs) {
                int magnitude = 1;
                if (magnitudesOut.size() > index) {
                    magnitude = magnitudesOut.at(index);
                }
                node->setConstantValue(index, output, magnitude);
                oindex++;
            }

            nodes.insert(node);
        }

        for (auto node : nodes) {
            string edges = edgeMap.at(node->getIndexTag());

            int socketIndex = 0;
            vector<string> sockets = split(edges, '|');
            for (const auto& socket : sockets) {
                if (socket.empty()) {
                    socketIndex++;
                    continue;
                }
                vector<string> edge = split(socket, 'n');
                for (const auto& edgeAttr : edge) {
                    if (edgeAttr.empty()) {
                        continue;
                    }
                    vector<string> socketAttr = split(edgeAttr, 's');
                    int prevNodeIndex = stoi(socketAttr.at(0));
                    int prevSocketIndex = stoi(socketAttr.at(1));

                    if (nodeMap.count(prevNodeIndex) > 0) {
                        connectNodes(prevSocketIndex, socketIndex,
                                     nodeMap.at(prevNodeIndex), node);
                    }
                }
                socketIndex++;
            }
        }
        return nodes;
    }

    vector<string> split(const string &s, char delim) {
        char escape = '\\';
        vector<string> result;
        stringstream ss (s);
        string item;

        string last = "";
        while (getline(ss, item, delim)) {

            // Delimiter was escaped
            if (!last.empty() && !result.empty() &&
                    last.at(last.size() - 1) == escape) {

                last = item;
                int rsize = result.size() - 1;
                result.at(rsize).erase( result.at(rsize).size() - 1);
                result.at(result.size() - 1) += delim + item;
            } else {
                result.push_back(item);
                last = item;
            }

        }
        return result;
    }


    /// End Node IO
    /////////////////////

    ZNodeView* newNode(ZNodeView::Type type) {
        ZNodeView* node = nullptr;
        if (!mDeleteNodes.empty()) {
            while (node == nullptr && !mDeleteNodes.empty()) {
                node = mDeleteNodes.front();
                mDeleteNodes.pop();
            }

            if (node == nullptr) {
                node = new ZNodeView(type);
            } else {
                node->setType(type);
            }
        } else {
            node = new ZNodeView(type);
        }
        return node;
    }

    set<ZNodeView*> duplicateNodes(set<ZNodeView*> selected) {
        map<ZNodeView*, ZNodeView*> tmpMap;
        set<ZNodeView*> originals;
        set<ZNodeView*> duplicates;

        for (ZNodeView* node : selected) {

            ZNodeView* dupNode = ZNodeUtil::get().newNode(node->getType());

            if (!node->mGroupNodes.empty()) {
                dupNode->mGroupNodes = duplicateNodes(node->mGroupNodes);
                for (ZNodeView* groupNode : dupNode->mGroupNodes) {
                    groupNode->mGroupParent = dupNode;
                    groupNode->setVisibility(false);

                    if (groupNode->getType() == ZNodeView::Type::GROUP_IN) {
                        dupNode->mGroupInput = groupNode;
                        dupNode->mGroupInput->setInputProxy(dupNode);
                    } else if (groupNode->getType() == ZNodeView::Type::GROUP_OUT) {
                        dupNode->mGroupOutput = groupNode;
                    }
                }
            }

            node->getAddNodeInterface()(dupNode, false);

            dupNode->copyParameters(node);
            dupNode->resetInitialPosition();
            dupNode->invalidate();

            originals.insert(node);
            duplicates.insert(dupNode);

            tmpMap.insert(pair<ZNodeView*, ZNodeView*>(node, dupNode));
        }

        for (ZNodeView* original : originals) {
            ZNodeView* duplicate = tmpMap.at(original);

            int outputIndex = 0;
            // Looping over sockets
            for (const vector<pair<ZNodeView*, int>>& edges : original->mOutputIndices) {
                // Looping over individual edges
                for(pair<ZNodeView*, int> edge : edges) {
                    ZNodeView* nextNode = edge.first;
                    int inputIndex = edge.second;

                    if (tmpMap.find(nextNode) != tmpMap.end()) {
                        connectNodes(outputIndex, inputIndex, duplicate, tmpMap.at(nextNode));
                    }
                }
                outputIndex++;
            }
        }

        return duplicates;
    }

    void findAndReplace(std::string& subject, const std::string& search,
                              const std::string& replace) {
        size_t pos = 0;
        while ((pos = subject.find(search, pos)) != std::string::npos) {
            subject.replace(pos, search.length(), replace);
            pos += replace.length();
        }
    }

    void deleteNodes(vector<ZNodeView*> nodes) {
        for (ZNodeView* node : nodes) {
            deleteNode(node);
        }
    }

    void deleteNodes(set<ZNodeView*> nodes) {
        for (ZNodeView* node : nodes) {
            deleteNode(node);
        }
    }

    void deleteNode(ZNodeView* node) {
        if (node == nullptr) {
            return;
        }

        node->setVisibility(false);
        node->setLineWidth(0);
        deleteConnections(node);

        deleteNodes(node->getGroupNodes());

        node->setGroupParent(nullptr);
        node->mGroupOutput = nullptr;
        node->mGroupInput = nullptr;
        node->mInputProxy = nullptr;
        node->mGroupNodes.clear();
        node->setProjectID(-1);
        node->setIndexTag(-1);

        for (auto laplace : node->mHeadlessLaplaceNodes) {
            deleteNode(laplace);
        }
        node->mHeadlessLaplaceNodes.clear();

        ZNodeUtil::get().submitForRecycle(node);
    }

    void deleteConnections(ZNodeView* node) {
        for (const vector<pair<ZNodeView *, int>> &inputs : node->mInputIndices) {
            for (pair<ZNodeView *, int> input : inputs) {
                ZNodeView *prevNode = input.first;
                int index = 0;
                if (prevNode != nullptr) {
                    for (pair<ZNodeView *, int> outputNode: prevNode->mOutputIndices.at(input.second)) {
                        if (outputNode.first == node) {
                            remove(prevNode->mOutputIndices.at(input.second), index);
                        } else {
                            index++;
                        }
                    }
                }
            }
        }

        for (const vector<pair<ZNodeView *, int>> &outputs : node->mOutputIndices) {
            for (pair<ZNodeView *, int> output : outputs) {
                ZNodeView *nextNode = output.first;
                if (nextNode != nullptr) {
                    int pairIndex = 0;
                    for (auto inputIndice : nextNode->mInputIndices.at(output.second)) {
                        if (inputIndice.first == node) {
                            remove(nextNode->mInputIndices.at(output.second), pairIndex);
                        } else {
                            pairIndex++;
                        }
                    }
                    nextNode->invalidateNodeRecursive();
                }
            }
        }
        node->invalidateSingleNode();
    }

    void submitForRecycle(ZNodeView* node) {
        if (node != nullptr) {
            mDeleteNodes.push(node);
        }
    }

    ZNodeUtil() {
        for (int i = 0; i != ZNodeView::Type::LAST; i++) {
            auto type = static_cast<ZNodeView::Type>(i);
            mTypes.insert({ZNodeView::getName(type), type});
            mFunctions.insert(ZNodeView::getName(type));
        }

        mTypes.insert({"^", ZNodeView::Type::POW});
    }

    int getNodePriority(ZNodeView* node) {
        set<string> operators = {"+", "-", "*", "/", "^"};

        string name = node->getName(node->getType());
        // Is arithmetic operator
        if (operators.count(name) > 0) {
            return getPriority(name);
        }
        return 5;
    }

    /**
     * Returns true if the parentheses are valid.
     * @param symbols
     * @return
     */
    bool validateParentheses(vector<string> symbols) {
        int count = 0;
        for (string symbol : symbols) {
            if (symbol == "(") {
                count++;
            } else if (symbol == ")") {
                count--;
            }
        }

        return count == 0;
    }

    string graphToExpString(ZNodeView *root, bool includeRoot) {

        string expression;
        set<string> operators = {"+", "-", "*", "/", "^"};

        // Is arithmetic operator
        if (operators.count(root->getName(root->getType())) > 0) {
            string first = floatToString(root->getConstantInput(0));
            string second = floatToString(root->getConstantInput(1));

            if (root->mInputIndices.at(0).size() == 0) {
                expression += first;
            }
            bool firstSocketMultiInput = root->mInputIndices.at(0).size() > 1;
            if (firstSocketMultiInput) {
                expression += "(";
            }
            int index = 0;
            for (pair<ZNodeView *, int> input : root->mInputIndices.at(0)) {
                ZNodeView *child = input.first;

                if (child != nullptr) {
                    bool needParen = getNodePriority(root) > getNodePriority(child);
                    // Recursive
                    if (needParen) {
                        expression += "(";
                    }
                    expression += graphToExpString(child, true);

                    if (needParen) {
                        expression += ")";
                    }

                    if (index < root->mInputIndices.at(0).size() - 1) {
                        expression += " + ";
                    }
                }
                index++;
            }
            if (firstSocketMultiInput) {
                expression += ")";
            }

            expression += " " + root->getName(root->getType()) + " ";

            if (root->mInputIndices.at(1).size() == 0) {
                expression += second;
            }
            bool secondSocketMultiInput = root->mInputIndices.at(1).size() > 1;
            if (secondSocketMultiInput) {
                expression += "(";
            }
            index = 0;
            for (pair<ZNodeView *, int> input : root->mInputIndices.at(1)) {
                ZNodeView *child = input.first;

                if (child != nullptr) {
                    // Recursive
                    bool needParen = getNodePriority(root) > getNodePriority(child);
                    // Recursive
                    if (needParen) {
                        expression += "(";
                    }
                    expression += graphToExpString(child, true);

                    if (index < root->mInputIndices.at(1).size() - 1) {
                        expression += " + ";
                    }
                    if (needParen) {
                        expression += ")";
                    }
                }
                index++;
            }
            if (secondSocketMultiInput) {
                expression += ")";
            }
            return expression;
        }

        if (includeRoot) {
            // Node is a constant
            if (root->getType() == ZNodeView::Type::C) {

                return floatToString(root->getConstantValue(0));
            }

            expression = root->getName(root->getType());
            vector<ZNodeView::SocketType> inputType = root->getSocketType().at(0);

            // Node is a variable
            if (root->getSocketCount().x == 0) {
                return expression;
            }

            // Node is function
            expression += "(";
        }

        int varCount = getVarCount(root);

        if (!includeRoot) {
            varCount = 1;
        }

        for (int socketIndex = 0; socketIndex < varCount; socketIndex++) {

            // No inputs for socket
            if (root->mInputIndices.at(socketIndex).empty()) {
                expression += "x";
            } else {
                int index = 0;
                for (pair<ZNodeView *, int> input : root->mInputIndices.at(socketIndex)) {
                    ZNodeView *child = input.first;
                    if (child != nullptr) {
                        if (child == root) {
                            return "Invalid tree";
                        }
                        expression += graphToExpString(child, true);
                        if (index < root->mInputIndices.at(socketIndex).size() - 1) {
                            expression += " + ";
                        }
                    }
                    index++;
                }
            }

            if (socketIndex < varCount - 1) {
                expression += ", ";
            }

        }

        if (includeRoot) {
            expression += ")";
        }

        if (expression.empty()) {
            expression = "x";
        }
        return expression;
    }

    vector<ZNodeView *> expStringToGraph(string testString) {
        if (testString.empty()) {
            return vector<ZNodeView*>();
        }

        set<string> variables = {"x", "y", "z"};
        set<string> operators = {"+", "-", "*", "/", "^"};

        for(string var : variables) {
            mFunctions.erase(var);
        }

        for (string op : operators) {
            mFunctions.erase(op);
        }
    //    string testString = "3 + 4(10.0014 +2*2)+ max(1,2)";
        //testString = "3 + 4 * 2 / ( 1 - 5 ) ^ 2 ^ 3";
        /*
         * 3 4 2 * 1 5 - 2 3 ^ ^ / +
         * 3 8 -4 8
         *
         **/

        //string testString = "sin(max(2,3)/3*3.1415)";

        vector<string> tokens = getTokens(testString);

        if (tokens.empty()) {
            return vector<ZNodeView*>();
        }

        // check for parentheses issues
        if (!validateParentheses(tokens)) {
            return vector<ZNodeView*>();
        }

        stack<string> opStack;
        queue<string> outQueue;

        // Shunting-yard implementation
        for (const string& token : tokens) {
            if (isNumber(token) || variables.count(token) > 0) {
                outQueue.push(token);
            } else if (mFunctions.count(token) > 0) {
                opStack.push(token);
            } else if (operators.count(token) > 0) {

                // There is operator at top of operator stack
                if (!opStack.empty()) {
                    int tokenPriority = getPriority(token);
                    while (!opStack.empty() && operators.count(opStack.top()) > 0 &&
                            ((getPriority(opStack.top()) > tokenPriority) ||
                            (getPriority(opStack.top()) == tokenPriority && token != "^")) &&
                            opStack.top() != ")") {
                        outQueue.push(opStack.top());
                        opStack.pop();
                    }
                }
                opStack.push(token);
            } else if (token == "(") {
                opStack.push(token);

            } else if (token == ")") {
                // Could handle mismatched parentheses here, but not doing that for now.
                while(!opStack.empty() && opStack.top() != "(") {
                    outQueue.push(opStack.top());
                    opStack.pop();
                }

                if (!opStack.empty() && opStack.top() == "(") {
                    opStack.pop();
                }

                if (!opStack.empty() && mFunctions.count(opStack.top()) > 0) {
                    outQueue.push(opStack.top());
                    opStack.pop();
                }
            }
        }

        while(!opStack.empty()) {
            outQueue.push(opStack.top());
            opStack.pop();
        }

        vector<ZNodeView*> allNodes;
        stack<ZNodeView*> evalStack;

        while (!outQueue.empty()) {

            string symbol = outQueue.front();
            outQueue.pop();
            if (isNumber(symbol)) {
                auto* constant = get().newNode(ZNodeView::Type::C);
                constant->setConstantValue(0, (float) stod(symbol), 6);
                evalStack.push(constant);
                allNodes.push_back(constant);
            } else {
                if (mTypes.count(symbol) > 0) {
                    ZNodeView::Type type = mTypes.at(symbol);
                    auto node = get().newNode(type);

                    int inputCount = getVarCount(node);
                    if (inputCount > 0) {

                        for (int si = 0; si < inputCount; si++) {
                            int socketIndex = inputCount - si - 1;
                            if (!evalStack.empty()) {
                                auto node1 = evalStack.top();
                                evalStack.pop();

                               // if (node1->getType() != ZNodeView::Type::X) {
                               connectNodes(0, socketIndex, node1, node);
                                //}
                                //else {
                               //     submitForRecycle(node1);
                               // }
                            }
                        }

                        evalStack.push(node);
                        allNodes.push_back(node);

                    } else {
                        // X is default so no need to add a redundant node
                        evalStack.push(node);
                        allNodes.push_back(node);
                    }
                }
            }
        }

        // Todo:
        // 1: Evaluate out queue
        // 2: Once evaluation works it should be more clear how to build a node structure
        // 3: Functions with 3+ arguments need to be handled differently
        // 4: After string to nodes works, implement nodes to string. This should be easier.
        // 5: Add UI / keyboard shortcuts for symbolic operations

        std::reverse(allNodes.begin(), allNodes.end());
        return allNodes;
    }

    static string floatToString(float value) {
        auto format = "%.2f";
        auto size = std::snprintf(nullptr, 0, format, value);
        std::string output(size, '\0');
        std::sprintf(&output[0], format, value);
        return output;
    }

    template <typename T>
    void remove(std::vector<T>& vec, size_t pos){
        auto it = vec.begin();
        std::advance(it, pos);
        vec.erase(it);
    }

    static int getVarCount(ZNodeView* node) {

        if (node->getSocketCount().x == 0) {
             return 0;
        }

        int count = 0;
        int constantCount = 0;
        vector<ZNodeView::SocketType> types = node->getSocketType().at(0);
        for (ZNodeView::SocketType type : types) {
            if (type == ZNodeView::SocketType::VAR) {
                count++;
            }

            if (type == ZNodeView::SocketType::CON) {
                constantCount++;
            }
        }

        if (count == 0) {
            return constantCount;
        }

        return count;

    }

    static void connectNodes(int outIndex, int inIndex, ZNodeView *firstNode, ZNodeView *secondNode) {
        firstNode->mOutputIndices.at(outIndex).push_back(pair<ZNodeView *, int>(secondNode, inIndex));
        secondNode->mInputIndices.at(inIndex).push_back(pair<ZNodeView *, int>(firstNode, outIndex));
    }

    static vector<string> getTokens(string input) {
        input += "";
        vector<string> tokens;
        set<string> variables = {"x", "y", "z"};

        // Correct for multiplication shorthand
        for (int i = 1; i < input.size(); i++) {
            string prev =  input.substr(i-1, 1);
            string current =  input.substr(i, 1);
            bool isVar = variables.count(current) > 0;
            bool previousIsNotOp = variables.count(prev) > 0 || isNumber(prev);

            if (isVar && previousIsNotOp) {
                input.insert(i, "*");
            }
        }

        input.erase(std::remove_if(input.begin(), input.end(), ::isspace), input.end());

        char testArray[input.length() + 1];
        strcpy(testArray, input.c_str());
        char *fullInput = strdup(testArray);

        const char* delim = " ,+-*/^()";

        char* afterSplit = strtok(testArray, delim);

        if (afterSplit == nullptr) {
            return vector<string>();
        }
        if (afterSplit - testArray + strlen(afterSplit) > 1) {
            char d = fullInput[0];
            if (d == '(' || d == ')') {
                tokens.emplace_back(string(1, d));
            }
        }

        tokens.emplace_back(string(afterSplit));

        while (afterSplit != nullptr) {
            uint start = afterSplit - testArray + strlen(afterSplit);

            afterSplit = strtok(nullptr, delim);

            if (afterSplit != nullptr) {
                uint end = afterSplit - testArray;

                for (uint i = start; i < end; i++) {
                    char d = fullInput[i];
                    tokens.emplace_back(string(1, d));
                }

                tokens.emplace_back(string(afterSplit));
            } else if (start < input.size()){
                for (uint i = start; i < input.size(); i++) {
                    char d = fullInput[i];
                    tokens.emplace_back(string(1, d));
                }
            }
        }
        
        free(fullInput);

        return tokens;
    }

    static int getPriority(const string& op) {
        switch(op[0]) {
            case '^': return 4;
            case '*': return 3;
            case '/': return 3;
            case '+': return 2;
            case '-': return 2;
        }
    }

    static bool isNumber(const std::string& str) {
        int i = 0, j = str.length() - 1;

        // Handling whitespaces
        while (i < str.length() && str[i] == ' ') {
            i++;
        }
        while (j >= 0 && str[j] == ' ') {
            j--;
        }
        if (i > j) {
            return false;
        }

        if (i == j && !(str[i] >= '0' && str[i] <= '9')) {
            return false;
        }

        if (str[i] != '.' && str[i] != '+'
            && str[i] != '-' && !(str[i] >= '0' && str[i] <= '9')) {
            return false;
        }

        // To check if a '.' or 'e' is found in given
        // string. We use this flag to make sure that
        // either of them appear only once.
        bool flagDotOrE = false;

        for (i; i <= j; i++) {
            // If any of the char does not belong to
            // {digit, +, -, ., e}
            if (str[i] != 'e' && str[i] != '.'
                && str[i] != '+' && str[i] != '-'
                && !(str[i] >= '0' && str[i] <= '9')) {
                return false;
            }

            if (str[i] == '.') {
                // checks if the char 'e' has already
                // occurred before '.' If yes, return 0.
                if (flagDotOrE) {
                    return false;
                }

                // If '.' is the last character.
                if (i + 1 > str.length()) {
                    return false;
                }

                // if '.' is not followed by a digit.
                if (!(str[i + 1] >= '0' && str[i + 1] <= '9')) {
                    return false;
                }
            }

            else if (str[i] == 'e') {
                // set flagDotOrE = 1 when e is encountered.
                flagDotOrE = true;

                // if there is no digit before 'e'.
                if (!(str[i - 1] >= '0' && str[i - 1] <= '9')) {
                    return false;
                }

                // If 'e' is the last Character
                if (i + 1 > str.length()) {
                    return false;
                }

                // if e is not followed either by
                // '+', '-' or a digit
                if (str[i + 1] != '+' && str[i + 1] != '-'
                    && (str[i + 1] >= '0' && str[i] <= '9')) {
                    return false;
                }
            }
        }

        /* If the string skips all above cases, then
        it is numeric*/
        return true;
    }
};


#endif //ZPATH_ZNODEUTIL_H
