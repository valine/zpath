//
// Created by lukas on 3/22/22.
//

#ifndef ZPATH_ZNODEDEFSTORE_H
#define ZPATH_ZNODEDEFSTORE_H
class ZNodeUtil;

#include <map>
#include "ui/nodetype.h"
#include <vector>
#include "utils/datastore.h"

#ifndef ZPATH_ZNODEUTIL_H
#include "utils/znodeutil.h"
#endif

#include <filesystem>
#if __APPLE__
using std::__fs::filesystem::directory_iterator;
using std::__fs::filesystem::directory_entry;
#else
using std::filesystem::directory_iterator;
using std::filesystem::directory_entry;
#endif

class ZNodeDefStore {
public:
    static ZNodeDefStore& get(){
        static ZNodeDefStore instance; // Guaranteed to be destroyed.
        // Instantiated on first use.
        return instance;
    }

    vector<NodeType*> getMathNodeTypes() {
        return mMathNodeTypes;
    }

    NodeType* getMathNodeType(const string& name) {
        return mMathNameMap.at(name);
    }

    vector<NodeType*> getJsonNodeTypes() {
        return mJsonNodeTypes;
    }

    vector<NodeType*> getAllNodeTypes() {
        std::vector<NodeType*> v = mJsonNodeTypes;
        v.insert(v.end(), mMathNodeTypes.begin(), mMathNodeTypes.end());
        return v;
    }

    NodeType* getJsonNodeType(const string& name) {
        return mJsonNameMap.at(name);
    }

private:
    vector<NodeType*> mMathNodeTypes;
    map<string, NodeType*> mMathNameMap;

    vector<NodeType*> mJsonNodeTypes;
    map<string, NodeType*> mJsonNameMap;

    ZNodeDefStore();

    void indexNodes() {
        for (NodeType* nodeType : mMathNodeTypes) {
            mMathNameMap.insert({nodeType->mName, nodeType});
        }

        for (NodeType* nodeType : mJsonNodeTypes) {
            mJsonNameMap.insert({nodeType->mName, nodeType});
        }
    }

    string getFileName(const string& s) {
        char sep = '/';

        #ifdef _WIN32
                sep = '\\';
        #endif

        size_t i = s.rfind(sep, s.length());
        if (i != string::npos) {
            string name = (s.substr(i+1, s.length() - i));
            return name;
        }

        return("");
    }

};


#endif //ZPATH_ZNODEDEFSTORE_H
