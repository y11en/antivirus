#include <boost/filesystem/convenience.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <vector>
#include <fstream>
#include <iostream>

#include "IndexerTask.hpp"
#include "Exception.hpp"
#include "Lexicon.hpp"
#include "Urls.hpp"
#include "Decoder.hpp"

namespace fs = boost::filesystem;

typedef std::vector<boost::shared_ptr<IndexerTask> > Tasks;

bool checkFile(const std::string& name) {
    return name.find("output-") == 0 && name.find("-ptr") == std::string::npos;
    //return fs::extension(name) == ".data";
}

boost::shared_ptr<IndexerTask> createTask(const std::string& infoFile) {
    return boost::shared_ptr<IndexerTask>(new IndexerTask(infoFile));
}

void gatherFiles(Tasks& tasks, const fs::path& path) {
    static std::set<std::string> files;
    fs::directory_iterator dir_iter(path);
    fs::directory_iterator end_iter;
    for (; dir_iter != end_iter; ++dir_iter) {
        try {
            if (!fs::is_directory(*dir_iter)) {
                std::string file = dir_iter->string();
                if (checkFile(dir_iter->leaf()) && files.find(file) == files.end()) {
                    files.insert(file);
                    tasks.push_back(createTask(file));
                }
            }
        }
        catch (const std::exception& ex) {
            std::cerr << "error: " << dir_iter->leaf() << " " << ex.what() << std::endl;
            continue;
        }
    }
}

int main(int argc, char* argv[]) {
    fs::path full_path(fs::initial_path());

    if (argc > 1)
        full_path = fs::system_complete(fs::path(argv[1], fs::native));
    
    if (!fs::is_directory(full_path)) {
        std::cerr << "error: not a directory " << full_path.native_directory_string() << std::endl;
        return 1;
    }

    Tasks   tasks;
        
    Decoder::initialize();
    Lexicon::instance().initialize();
    Urls::instance().initialize();
        
    /*while (true)*/ {
        tasks.clear();
        gatherFiles(tasks, full_path);
        try {
            for (Tasks::iterator i = tasks.begin(); i != tasks.end(); ++i) {
                (*i)->execute();
            }
        } catch(Exception& e) {
            std::cerr << e.what() << std::endl;
        }
        //Sleep(1000);
    }
    
    Lexicon::instance().uninitialize();
    Urls::instance().uninitialize();
    Decoder::uninitialize();
    
    odbc::DriverManager::shutdown();
    
    return 0;
}
