//
// Created by Lukas Valine on 6/18/21.
//

#ifndef ZPATH_ZNODESTORE_H
#define ZPATH_ZNODESTORE_H

#include <filesystem>
#if __APPLE__
using std::__fs::filesystem::directory_iterator;
using std::__fs::filesystem::directory_entry;
#else
using std::filesystem::directory_iterator;
using std::filesystem::directory_entry;
#endif

class ZNodeStore {

private:


public:
    static ZNodeStore& get(){
        static ZNodeStore instance; // Guaranteed to be destroyed.
        // Instantiated on first use.
        return instance;
    }

    string saveGraph(vector<ZNodeView *> graph, string name, bool fullPath) {
        set<ZNodeView*> graphSet;
        for (auto node : graph) {
            graphSet.insert(node);
        }
        return saveGraph(graphSet, name, fullPath);
    }


    string saveGraph(set<ZNodeView *> graph, string name, bool fullPath) {
        string sgraph = ZNodeUtil::get().serialize(graph);

        string projectFolder = "resources/graphs/";
        string path = ZSettings::get().getResourcePath() + projectFolder;
        string ext = ".zpath";
        string fullPathString = path + name + ext;
        if (fullPath) {
            fullPathString = name;
        }
        std::ofstream out(fullPathString);
        out << sgraph;
        out.close();
        return fullPathString;
    }

    /**
     * Renames a project at path to name
     * @param fromPath Path to prject
     * @param toName
     * @return
     */
    string renameProject(const string& fromPath, const string& toName) {

        string projectFolder = "resources/graphs/";
        string path = ZSettings::get().getResourcePath() + projectFolder;
        string ext = ".zpath";
        string toPath = path + toName + ext;
        rename(fromPath.c_str(), toPath.c_str());
        return toPath;
    }

    /**
     * Delete project. Return true when sucessful.
     * @param path
     * @return
     */
    bool deleteProject(const string& path) {
        int status = remove(path.c_str());
        return status == 0;
    }

    vector<string> getProjectNames() {
        string projectFolder = "resources/graphs/";
        string path = ZSettings::get().getResourcePath() + projectFolder;

        vector<string> names;
        for (const auto &file : directory_iterator(path)) {
            names.push_back(file.path());
        }

        return names;
    }

    set<ZNodeView*> loadGraph(string name) {
        //string projectFolder = "resources/graphs/";
        //string path = ZSettings::get().getResourcePath() + projectFolder + name;

        std::ifstream t(name);
        std::string dataString;
        t.seekg(0, std::ios::end);
        dataString.reserve(t.tellg());
        t.seekg(0, std::ios::beg);
        dataString.assign((std::istreambuf_iterator<char>(t)),
                          std::istreambuf_iterator<char>());

        return ZNodeUtil::get().deserialize(dataString);
    }


};


#endif //ZPATH_ZNODESTORE_H
