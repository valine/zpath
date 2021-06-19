//
// Created by Lukas Valine on 6/18/21.
//

#ifndef ZPATH_ZNODESTORE_H
#define ZPATH_ZNODESTORE_H

#include <filesystem>

using std::__fs::filesystem::directory_iterator;
using std::__fs::filesystem::directory_entry;

class ZNodeStore {

private:


public:
    static ZNodeStore& get(){
        static ZNodeStore instance; // Guaranteed to be destroyed.
        // Instantiated on first use.
        return instance;
    }

    void saveGraph(vector<ZNodeView*> graph, string name) {
        set<ZNodeView*> graphSet;
        for (auto node : graph) {
            graphSet.insert(node);
        }
        saveGraph(graphSet, name);
    }


    void saveGraph(set<ZNodeView*> graph, string name) {
        string sgraph = ZNodeUtil::get().serialize(graph);

        string projectFolder = "resources/graphs/";
        string path = ZSettingsStore::get().getResourcePath() + projectFolder;
        std::ofstream out(path + name);
        out << sgraph;
        out.close();
    }

    vector<string> getProjectNames() {
        string projectFolder = "resources/graphs/";
        string path = ZSettingsStore::get().getResourcePath() + projectFolder;

        vector<string> names;
        for (const std::__fs::filesystem::directory_entry &file : directory_iterator(path)) {
            names.push_back(getFileName(file.path()));
        }

        return names;
    }

    set<ZNodeView*> loadGraph(string name) {
        string projectFolder = "resources/graphs/";
        string path = ZSettingsStore::get().getResourcePath() + projectFolder + name;

        std::ifstream t(path);
        std::string dataString;
        t.seekg(0, std::ios::end);
        dataString.reserve(t.tellg());
        t.seekg(0, std::ios::beg);
        dataString.assign((std::istreambuf_iterator<char>(t)),
                          std::istreambuf_iterator<char>());

        return ZNodeUtil::get().deserialize(dataString);
    }

    static string getFileName(const string& s) {
        char sep = '/';

#ifdef _WIN32
        sep = '\\';
#endif

        size_t i = s.rfind(sep, s.length());
        if (i != string::npos) {
            return(s.substr(i+1, s.length() - i));
        }

        return("");
    }

};


#endif //ZPATH_ZNODESTORE_H
