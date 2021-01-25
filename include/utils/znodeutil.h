//
// Created by lukas on 1/18/21.
//

#ifndef ZPATH_ZNODEUTIL_H
#define ZPATH_ZNODEUTIL_H

class ZLabel;

#include <ui/nodeview.h>
#include <queue>

using namespace std;

class ZNodeUtil {

    //string graphToString(ZNodeView* node);

public:
    static ZNodeUtil& get(){
        static ZNodeUtil instance; // Guaranteed to be destroyed.
        // Instantiated on first use.
        return instance;
    }

    vector<ZNodeView *> stringToGraph(string input) {


        string testString = "3 + 4(10+2*2)+1";


        vector<string> tokens = getTokens(testString);


        stack<string> operators;
        queue<string> outputs;



        return vector<ZNodeView*>();
    }

    vector<string> getTokens(string input) {
        vector<string> tokens;

        input.erase(std::remove_if(input.begin(), input.end(), ::isspace), input.end());

        char testArray[input.length() + 1];
        strcpy(testArray, input.c_str());
        char *copy = strdup(testArray);

        const char* delim = " +-*/^()";
        char* afterSplit = strtok(testArray, delim);
        char d = copy[afterSplit - testArray + strlen(afterSplit)];

        tokens.emplace_back(string(afterSplit));
        tokens.emplace_back(string(1, d));

        while (afterSplit != nullptr) {

            d = copy[afterSplit - testArray + strlen(afterSplit)];
            cout << "t:" << afterSplit << endl;
            cout << "d:" << d << endl;
            afterSplit = strtok(nullptr, delim);

            if (afterSplit != nullptr) {
                tokens.emplace_back(string(afterSplit));
                tokens.emplace_back(string(1, d));
            }
        }

        return tokens;
    }

    bool isNumber(const std::string& s) {
        return !s.empty() && std::find_if(s.begin(),
                s.end(), [](unsigned char c) {
            return !std::isdigit(c);
        }) == s.end();
    }
};


#endif //ZPATH_ZNODEUTIL_H
