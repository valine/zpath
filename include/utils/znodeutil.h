//
// Created by lukas on 1/18/21.
//

#ifndef ZPATH_ZNODEUTIL_H
#define ZPATH_ZNODEUTIL_H

class ZLabel;

#include <ui/nodeview.h>

using namespace std;

class ZNodeUtil {

    vector<ZNodeView *> stringToGraph(string input) {
        istringstream iss(input);
        vector<string> tokens;
        copy(istream_iterator<string>(iss),
             istream_iterator<string>(),
             back_inserter(tokens));

    }

    //string graphToString(ZNodeView* node);

    static ZNodeUtil& get(){
        static ZNodeUtil instance; // Guaranteed to be destroyed.
        // Instantiated on first use.
        return instance;
    }

};


#endif //ZPATH_ZNODEUTIL_H
