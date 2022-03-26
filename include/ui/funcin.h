//
// Created by lukas on 3/26/22.
//

#ifndef ZPATH_FUNCIN_H
#define ZPATH_FUNCIN_H

#include <utility>
#include <vector>

using namespace std;

class ZNodeView;

class FuncIn {
public :

    vector<vector<float>> x;
    vector<vector<float>> rootInput;
    vector<complex<float>> cache;
    float start;
    float width;
    ZNodeView* nodeView;

    FuncIn(vector<vector<float>>  ix, vector<vector<float>> iroot,
           vector<complex<float>> icache, float istart,
           float iwidth, ZNodeView *iview) {
        x = std::move(ix);
        rootInput = std::move(iroot);
        cache = std::move(icache);
        start = istart;
        width = iwidth;
        nodeView = iview;
    }


};
#endif //ZPATH_FUNCIN_H
