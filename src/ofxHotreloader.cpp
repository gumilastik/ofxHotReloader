#include "ofxHotreloader.h"

#if defined(_WIN32)
#include <windows.h>
#elif defined(__APPLE__)
#include <dlfcn.h>
#endif

void ofxHotReloader::initGL(void * ptr) {
	if (!loaded || funcInitGL == NULL) return;

	(funcInitGL)(ptr);
}

void* ofxHotReloader::getGL() {
	if (!loaded || funcGetGL == NULL) return nullptr;

	return (funcGetGL)();
}

void ofxHotReloader::deinitGL() {
	if (!loaded || funcDeinitGL == NULL) return;

	(funcDeinitGL)();
}

ofxHotReloader::ofxHotReloader() {
    nameTempDir = "";
    callbackOnLoaded = nullptr;
    instanceLib = NULL;
    
    funcInitGL = NULL;
    funcGetGL = NULL;
    funcDeinitGL = NULL;
    funcCreatePlugin = NULL;
    
    loaded = false;
}

ofxHotReloader::~ofxHotReloader() {
    deinitGL();
    
#ifdef _WIN32
    if (instanceLib) {
        FreeLibrary(instanceLib);
    }
#elif __APPLE__
    if (instanceLib) {
        dlclose(instanceLib);
    }
#endif
    
    Utils::clearSubdirectories(pathPlugins);
}

// Utils::getDirectoryPath("plugins")
void ofxHotReloader::setup(std::string pathOriginalLib, std::string pathPlugins, unsigned long long timeUpdateInterval) {
    this->pathOriginalLib = Utils::getFileAbsolutePath(pathOriginalLib);
    this->pathOriginalPdb = Utils::joinFilePath(Utils::getEnclosingDirectory(pathOriginalLib), Utils::getFileBaseName(pathOriginalLib) + ".pdb");

    this->pathPlugins = pathPlugins;
    this->timeUpdateInterval = timeUpdateInterval;
    
    Utils::clearSubdirectories(pathPlugins);

    check(true);
}

void ofxHotReloader::generatePath(std::string pathDir, std::string & pathLib, std::string & pathPdb) {
	Utils::createDirectory(pathDir);

    pathLib = Utils::joinFilePath(Utils::getFileAbsolutePath(pathDir), Utils::getFileName(pathOriginalLib));
	pathPdb = Utils::joinFilePath(Utils::getEnclosingDirectory(pathLib), Utils::getFileBaseName(pathLib) + ".pdb");
}

void ofxHotReloader::load(void* ptrPrevGL) {
	std::string pathLib;
	std::string pathPdb;

	generatePath(Utils::joinFilePath(pathPlugins, nameTempDir), pathLib, pathPdb);

#ifdef _WIN32
	instanceLib = LoadLibraryA(pathLib.c_str());
#elif __APPLE__
    // change id of dylib to temp id
    Utils::Execute("install_name_tool -id /" + Utils::getFileBaseName(pathOriginalLib) + (nameTempDir != "" ? "." + nameTempDir : "") + "." + Utils::getFileExtension(pathOriginalLib) + " \"" + pathLib + "\"");
    instanceLib = dlopen(pathLib.c_str(), RTLD_LAZY);
#endif

    if (!instanceLib) {
#ifdef _WIN32
        std::cout << "could not load the dynamic library: " << GetLastError() << std::endl;
#elif __APPLE__
        std::cout << "could not load the dynamic library: " << dlerror() << std::endl;
#endif

		funcInitGL = NULL;
		funcGetGL = NULL;
		funcDeinitGL = NULL;
		funcCreatePlugin = NULL;

		loaded = false;
	}
	else {
		// resolve function address here
#ifdef _WIN32
		funcInitGL = (FuncInitGL)GetProcAddress(instanceLib, "initGL");
		funcGetGL = (FuncGetGL)GetProcAddress(instanceLib, "getGL");
		funcDeinitGL = (FuncDeinitGL)GetProcAddress(instanceLib, "deinitGL");
		funcCreatePlugin = (FuncCreatePlugin)GetProcAddress(instanceLib, "createPlugin");
#elif __APPLE__
		funcInitGL = (FuncInitGL)dlsym(instanceLib, "initGL");
		funcGetGL = (FuncGetGL)dlsym(instanceLib, "getGL");
		funcDeinitGL = (FuncDeinitGL)dlsym(instanceLib, "deinitGL");
		funcCreatePlugin = (FuncCreatePlugin)dlsym(instanceLib, "createPlugin");
#endif

		generatePath(pathPlugins, pathLib, pathPdb);

		if (Utils::copyFileFromTo(pathOriginalLib, pathLib)) {
			Utils::copyFileFromTo(pathOriginalPdb, pathPdb);
		}
        
		loaded = true;
 
#if defined(JUCE_APP_VERSION)
		initGL(ptrPrevGL);
#else
		initGL((void*)ofGetMainLoop().get());
#endif

		// hotswap
		for (size_t i = 0; i < plugins.size(); i++) {
			ofxPluginBase* ptr = (ofxPluginBase*)(funcCreatePlugin)((char*)plugins[i]->namePlugin.c_str());
			ptr->clone(plugins[i]->plugin);

            delete plugins[i]->plugin;
			plugins[i]->plugin = ptr;
		}

		// callback
		if(callbackOnLoaded) (callbackOnLoaded)();
	}
}

void ofxHotReloader::addCallbackOnLoaded(CallbackOnLoaded callbackOnLoaded) {
	this->callbackOnLoaded = callbackOnLoaded;
}

void ofxHotReloader::check(bool force) {
	if (force || Utils::getElapsedTimeMillis() - timeLastUpdated > timeUpdateInterval) {
		if (!Utils::checkFileExits(pathOriginalLib)) {
			if (!loaded) {
				load();
			}
			return;
		}

		unsigned long long tSrc = Utils::getFileModificationTime(pathOriginalLib);

		std::string pathLib;
		std::string pathPdb;
		
		generatePath(Utils::joinFilePath(pathPlugins, nameTempDir), pathLib, pathPdb);

		if (!Utils::checkFileExits(pathLib) || tSrc > tDest) {

			// check for open for read
			FILE *fp = fopen(pathOriginalLib.c_str(), "rb");
			if (fp != NULL) {
				fclose(fp);

				std::cout << "reload" << std::endl;

				tDest = tSrc;
				nameTempDir = std::to_string(tSrc);

				generatePath(Utils::joinFilePath(pathPlugins, nameTempDir), pathLib, pathPdb);

#ifdef _WIN32
				HINSTANCE instanceLibPrevious = instanceLib;
#elif __APPLE__
				void* instanceLibPrevious = instanceLib;
#endif
				
				if (Utils::copyFileFromTo(pathOriginalLib, pathLib)) {
                    Utils::copyFileFromTo(pathOriginalPdb, pathPdb);
					load(getGL());
				}
				
#if defined(JUCE_APP_VERSION)
				// disable for share GL context for JUCE
#else
				if (loaded) {
#ifdef _WIN32
					if (instanceLibPrevious) {
						FreeLibrary(instanceLibPrevious);
					}
#elif __APPLE__
					if (instanceLibPrevious) {
						dlclose(instanceLibPrevious);
					}
#endif
				}
#endif
			}
		}
		timeLastUpdated = Utils::getElapsedTimeMillis();
	}
}

ofxPlugin* ofxHotReloader::createPluginObject(char * namePlugin) {
	if (!loaded || funcCreatePlugin == NULL) return nullptr;

	ofxPluginBase* pluginBase = (ofxPluginBase*)(funcCreatePlugin)(namePlugin);
	if (pluginBase == NULL) {
		std::cout << "could not find the plugin " << namePlugin << std::endl;
		return nullptr;
	}

	ofxPlugin* plugin = new ofxPlugin(pluginBase, &plugins, namePlugin);
	plugins.push_back(plugin);

	return plugin;
}
