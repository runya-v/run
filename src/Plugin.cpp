/*!
 * \brief  Базовый класс плагина
 * \author Rostislav Velichko. rostislav.vel@gmail.com
 * \date   12.10.2012
 */

#include <exception>
#include <iostream>
#include <sstream>
#include <utility>
#include <map>

#include <boost/foreach.hpp>
#include <boost/filesystem.hpp>

#ifdef _WIN32
#include <windows.h>
#else // _WIN32
#include <dirent.h>
#include <dlfcn.h>
#endif // _WIN32

#ifdef __GNUC__
__extension__
#endif

#include "Plugin.hpp"
////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef _WIN32
#define LIBRARY_EXTENSION            ".dll"
#define LIB_OPEN(pLibName)           LoadLibrary((pLibName))
#define LIB_GET_PROC(pLib, pLibName) GetProcAddress((pLib),(pLibName))
#define LIB_ERROR                    GetLastError()
#define LIB_CLOSE(pLib)              FreeLibrary((pLib))
#define LIB_CLOSE_RESULT(result)     ((result) == true)
#else // _WIN32
#define LIBRARY_EXTENSION            ".so"
#define LIB_OPEN(pLibName)           ::dlopen((pLibName), RTLD_LAZY)
#define LIB_GET_PROC(pLib, pLibName) ::dlsym((pLib),(pLibName))
#define LIB_ERROR                    ::dlerror()
#define LIB_CLOSE(pLib)              ::dlclose((pLib))
#define LIB_CLOSE_RESULT(result)     ((result) == 0)
#endif // _WIN32
////////////////////////////////////////////////////////////////////////////////////////////////////


namespace bfs = boost::filesystem;


class Config {
public:
    Config(Plugin *parent, const std::string &name, int argc, char **argv, char **env) {
        Plugin::s_plugin->init(parent, name, argc, argv, env);
    }
};
////////////////////////////////////////////////////////////////////////////////////////////////////


Plugin *Plugin::s_plugin = NULL;


void Plugin::init(Plugin *parent, const std::string &name, int argc, char **argv, char **env) {
    _parent = parent;
    _name   = name;
    _argc   = argc;
    _argv   = argv;
    _env    = env;
}


void Plugin::loadLibrary(const std::string &lib_path) {
    bfs::path path(lib_path);
    std::string extension = path.extension().string();

    if (extension == LIBRARY_EXTENSION) {
        Plugin *plugin = get(path.string());

        if (plugin) {
            plugin->execute();
        }
    }
}


Plugin::Plugin()
    : _parent(NULL)
{
    Plugin::s_plugin = this;
}


Plugin::~Plugin()
{}


void Plugin::load(const std::string &lib_path) {
    bfs::path work_path = bfs::absolute(lib_path);

    if (bfs::is_directory(work_path)) {
        BOOST_FOREACH(
            const bfs::path &path,
            std::make_pair(bfs::directory_iterator(work_path), bfs::directory_iterator())
            )
        {
            loadLibrary(path.string());
        }
    }
    else {
        loadLibrary(work_path.string());
    }
}


Plugin* Plugin::get(const std::string &name) {
    PluginIter it = _plugins.find(name);

    if (it not_eq _plugins.end()) {
        if (this not_eq it->second.second) {
            std::clog << "Get library: `" << name << "`\n" << std::flush;
            return it->second.second;
        }
    }
    else {
        auto lib = LIB_OPEN(name.c_str());

        if (lib) {
            std::clog << "Load library: `" << name << "`\n" << std::flush;

            auto get_ptr = LIB_GET_PROC(lib, "Get");
            GetPluginFunc get_func;
            std::memcpy(&get_func, &get_ptr, sizeof(get_ptr));

            if (get_func not_eq nullptr) {
                Plugin *plugin = get_func(this, name.c_str(), _argc, _argv, _env);

                if (plugin) {
                    plugin->_name = name;
                    _plugins[name] = std::make_pair(lib, plugin);
                    return plugin;
                }
                else {
                    std::cerr << "Error: Get Plugin in `" << name.c_str() << "`.\n" << std::flush;
                }
            }
            else {
                std::cerr << "Error: Get Func in `" << name.c_str() << "`." << std::endl;
            }
        }
        else {
            std::cerr << "Error: Load lib `" << name.c_str() << "`: " << LIB_ERROR << ".\n" << std::flush;
        }
    }
    return NULL;
}


bool Plugin::close(const std::string &name) {
    PluginIter it = _plugins.find(_name);

    if (it not_eq _plugins.end()) {
        if (LIB_CLOSE_RESULT(LIB_CLOSE(it->second.first))) {
            _plugins.erase(it);
            return true;
        }
    }
    return false;
}


void Plugin::close() {
    for (const PluginValue &value : _plugins) {
        if (not LIB_CLOSE_RESULT(LIB_CLOSE(value.second.first))) {
            std::cerr << "Can`t close library: `" << value.first << "`\n" << std::flush;
        }
        else {
            std::clog << "Close library: `" << value.first << "`\n" << std::flush;
        }
    }
}


std::size_t Plugin::size() {
    return _plugins.size();
}
////////////////////////////////////////////////////////////////////////////////////////////////////


extern "C" {

    Plugin* Get(Plugin *parent, const char *name, int argc, char **argv, char **env) {
        if (Plugin::s_plugin not_eq NULL) {
            Config(parent, name, argc, argv, env);
            return Plugin::s_plugin;
        }
        return NULL;
    }
} // extern "C"
