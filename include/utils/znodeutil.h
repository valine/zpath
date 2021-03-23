//
// Created by lukas on 1/18/21.
//

#ifndef ZPATH_ZNODEUTIL_H
#define ZPATH_ZNODEUTIL_H

class ZLabel;

#include <ui/nodeview.h>
#include <queue>
#include <set>

using namespace std;

class ZNodeUtil {

private:

    map<string, ZNodeView::Type> mTypes;
    //string graphToString(ZNodeView* node);

public:
    static ZNodeUtil& get(){
        static ZNodeUtil instance; // Guaranteed to be destroyed.
        // Instantiated on first use.
        return instance;
    }

    ZNodeUtil() {
        for (int i = 0; i != ZNodeView::Type::LAST; i++) {
            auto type = static_cast<ZNodeView::Type>(i);
            mTypes.insert({ZNodeView::getName(type), type});
        }

        mTypes.insert({"^", ZNodeView::Type::POW});
    }

    vector<ZNodeView *> stringToGraph(string input) {

        set<string> functions = {"max", "min", "sin", "cos", "tan"};
        set<string> operators = {"+", "-", "*", "/", "^"};
    //    string testString = "3 + 4(10.0014 +2*2)+ max(1,2)";
        string testString = "3 + 4 * 2 / ( 1 - 5 ) ^ 2 ^ 3";
        /*
         * 3 4 2 * 1 5 - 2 3 ^ ^ / +
         * 3 8 -4 8
         *
         **/

        //string testString = "sin(max(2,3)/3*3.1415)";

        vector<string> tokens = getTokens(testString);

        stack<string> opStack;
        queue<string> outQueue;

        // Shunting-yard implementation
        for (const string& token : tokens) {
            if (isNumber(token)) {
                outQueue.push(token);
            } else if (functions.count(token) > 0) {
                opStack.push(token);
            } else if (operators.count(token) > 0) {

                // There is operator at top of operator stack
                if (!opStack.empty()) {
                    int tokenPriority = getPriority(token);
                    while (operators.count(opStack.top()) > 0 &&
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
                while(opStack.top() != "(") {
                    outQueue.push(opStack.top());
                    opStack.pop();
                }

                if (opStack.top() == "(") {
                    opStack.pop();
                }

                if (functions.count(opStack.top()) > 0) {
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
                auto* constant = new ZNodeView(ZNodeView::Type::C);
                constant->setConstantValue(0, stof(symbol), 6);
                evalStack.push(constant);
                allNodes.push_back(constant);
            } else {
                if (mTypes.count(symbol) > 0) {
                    ZNodeView::Type type = mTypes.at(symbol);
                    auto node = new ZNodeView(type);
                    allNodes.push_back(node);

                    auto node1 = evalStack.top();
                    evalStack.pop();

                    auto node0 = evalStack.top();
                    evalStack.pop();

                    connectNodes(0,0, node0, node);
                    connectNodes(0,1, node1, node);

                    evalStack.push(node);
                }
            }
        }

        // Todo:
        // 1: Evaluate out queue
        // 2: Once evaluation works it should be more clear how to build a node structure
        // 3: Functions with 3+ arguments need to be handled differently
        // 4: After string to nodes works, implement nodes to string. This should be easier.
        // 5: Add UI / keyboard shortcuts for symbolic operations

        return allNodes;
    }

    static void connectNodes(int outIndex, int inIndex, ZNodeView *firstNode, ZNodeView *secondNode) {
        firstNode->mOutputIndices.at(outIndex).push_back(pair<ZNodeView *, int>(secondNode, inIndex));
        secondNode->mInputIndices.at(inIndex).push_back(pair<ZNodeView *, int>(firstNode, outIndex));
    }

    static vector<string> getTokens(string input) {
        vector<string> tokens;

        input.erase(std::remove_if(input.begin(), input.end(), ::isspace), input.end());

        char testArray[input.length() + 1];
        strcpy(testArray, input.c_str());
        char *fullInput = strdup(testArray);

        const char* delim = " ,+-*/^()";


        char* afterSplit = strtok(testArray, delim);
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
                char d = fullInput[start];
                tokens.emplace_back(string(1, d));
            }
        }

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
