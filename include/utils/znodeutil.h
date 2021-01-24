//
// Created by lukas on 1/18/21.
//

#ifndef ZPATH_ZNODEUTIL_H
#define ZPATH_ZNODEUTIL_H

class ZLabel;

#include <ui/nodeview.h>

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
        testString.erase(std::remove_if(testString.begin(), testString.end(), ::isspace), testString.end());

        char testArray[testString.length() + 1];
        strcpy(testArray, testString.c_str());
        char *copy = strdup(testArray);

        const char* delim = " +-*/^()";
        char* afterSplit = strtok(testArray, delim);

        while (afterSplit != nullptr) {

            char d = copy[afterSplit - testArray + strlen(afterSplit)];
            cout << "t:" << afterSplit << endl;
            cout << "d:" << d << endl;



            afterSplit = strtok(nullptr, delim);
        }

        return vector<ZNodeView*>();
    }
};


#endif //ZPATH_ZNODEUTIL_H
