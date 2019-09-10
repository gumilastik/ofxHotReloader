#include "ofxHotReloader.h"
#include "ofxHotReloaderUtils.h"

#if defined(_WIN32)
#include <windows.h>
#elif defined(__APPLE__)
#include <dlfcn.h>
#endif

#include <iostream>
#include <ctime>


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
	callbackBeforeLoad = nullptr;
	callbackAfterLoad = nullptr;
	instanceLib = NULL;

	funcInitGL = NULL;
	funcGetGL = NULL;
	funcDeinitGL = NULL;
	funcCreatePlugin = NULL;

	tDest = 0;

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

	ofxHotReloaderUtils::clearSubdirectories(pathPluginsFolder); 
}

// ofxHotReloaderUtils::getDirectoryPath("plugins")
void ofxHotReloader::setup(std::string pathOriginalLibFolder, std::string pathOriginalLibName, std::string pathPluginsFolder, unsigned long long timeUpdateInterval) {
#if defined (_WIN32) && !defined (_DEBUG)  
	std::string pathOriginalLibFilename = pathOriginalLibName + ".dll";
#elif defined (_WIN32) && defined (_DEBUG)  
	std::string pathOriginalLibFilename = pathOriginalLibName + "_debug.dll";
#elif defined (__APPLE__)
	std::string pathOriginalLibFilename = "lib" + pathOriginalLibName + ".dylib";
#endif

	this->pathOriginalLib = ofxHotReloaderUtils::getFileAbsolutePath(ofxHotReloaderUtils::joinFilePath(pathOriginalLibFolder, pathOriginalLibFilename));
	this->pathOriginalPdb = ofxHotReloaderUtils::joinFilePath(ofxHotReloaderUtils::getEnclosingDirectory(pathOriginalLib), ofxHotReloaderUtils::getFileBaseName(pathOriginalLib) + ".pdb");

	this->pathPluginsFolder = pathPluginsFolder;
	this->timeUpdateInterval = timeUpdateInterval;

	ofxHotReloaderUtils::clearSubdirectories(pathPluginsFolder);

	check(true);
}

void ofxHotReloader::addCallbackBeforeLoad(CallbackLoad callbackBeforeLoad) {
	this->callbackBeforeLoad = callbackBeforeLoad;
}

void ofxHotReloader::addCallbackAfterLoad(CallbackLoad callbackAfterLoad) {
	this->callbackAfterLoad = callbackAfterLoad;
}

void ofxHotReloader::generatePath(std::string pathDir, std::string & pathLib, std::string & pathPdb) {
	ofxHotReloaderUtils::createDirectory(pathDir);

	pathLib = ofxHotReloaderUtils::joinFilePath(ofxHotReloaderUtils::getFileAbsolutePath(pathDir), ofxHotReloaderUtils::getFileName(pathOriginalLib));
	pathPdb = ofxHotReloaderUtils::joinFilePath(ofxHotReloaderUtils::getEnclosingDirectory(pathLib), ofxHotReloaderUtils::getFileBaseName(pathLib) + ".pdb");
}

bool ofxHotReloader::load(void* ptrPrevGL) {
	std::string pathLib;
	std::string pathPdb;

	generatePath(ofxHotReloaderUtils::joinFilePath(pathPluginsFolder, nameTempDir), pathLib, pathPdb);

#ifdef _WIN32
	instanceLib = LoadLibraryA(pathLib.c_str());
#elif __APPLE__
	// change id of dylib to temp id
	ofxHotReloaderUtils::Execute("install_name_tool -id /" + ofxHotReloaderUtils::getFileBaseName(pathOriginalLib) + (nameTempDir != "" ? "." + nameTempDir : "") + "." + ofxHotReloaderUtils::getFileExtension(pathOriginalLib) + " \"" + pathLib + "\"");
	instanceLib = dlopen(pathLib.c_str(), RTLD_LAZY);
#endif

	if (!instanceLib) {
#ifdef _WIN32
		std::cout << "could not load the dynamic library: " << pathLib << std::endl;
		std::cout << "error: " << GetLastError() << std::endl;
#elif __APPLE__
		std::cout << "could not load the dynamic library: " << pathLib << std::endl;
		std::cout << "error: " << dlerror() << " " << std::endl;
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

		generatePath(pathPluginsFolder, pathLib, pathPdb);

		if (ofxHotReloaderUtils::copyFileFromTo(pathOriginalLib, pathLib)) {
#ifdef _WIN32
			ofxHotReloaderUtils::copyFileFromTo(pathOriginalPdb, pathPdb);
#endif
        }

		loaded = true;

#if defined(JUCE_APP_VERSION)
		initGL(ptrPrevGL);
#else
		initGL((void*)ofGetMainLoop().get());
#endif

		// callback before
		if (callbackBeforeLoad) (callbackBeforeLoad)();

		// hotswap
		for (size_t i = 0; i < plugins.size(); i++) {
			ofxPluginBase* ptr = (ofxPluginBase*)(funcCreatePlugin)((char*)plugins[i]->namePlugin.c_str());
			ptr->clone(plugins[i]->plugin);

			delete plugins[i]->plugin;
			plugins[i]->plugin = ptr;
		} 

		// callback after
		if (callbackAfterLoad) (callbackAfterLoad)();
	}

	return loaded;
}

bool ofxHotReloader::check(bool force) {
	bool reloaded = false;

	if (force || ofxHotReloaderUtils::getElapsedTimeMillis() - timeLastUpdated > timeUpdateInterval) {
		if (!ofxHotReloaderUtils::checkFileExits(pathOriginalLib)) {
			if (!loaded) {
				reloaded = load();
			}
		}
		else {
			unsigned long long tSrc = ofxHotReloaderUtils::getFileModificationTime(pathOriginalLib);

			std::string pathLib;
			std::string pathPdb;

			generatePath(ofxHotReloaderUtils::joinFilePath(pathPluginsFolder, nameTempDir), pathLib, pathPdb);

			if (!ofxHotReloaderUtils::checkFileExits(pathLib) || tSrc > tDest) {

				// check for open for read
				FILE *fp = fopen(pathOriginalLib.c_str(), "rb");
				if (fp != NULL) {
					fclose(fp);

					std::cout << "reload" << std::endl;

					tDest = tSrc;
					nameTempDir = std::to_string(tSrc);

					generatePath(ofxHotReloaderUtils::joinFilePath(pathPluginsFolder, nameTempDir), pathLib, pathPdb);

#ifdef _WIN32
					HINSTANCE instanceLibPrevious = instanceLib;
#elif __APPLE__
					void* instanceLibPrevious = instanceLib;
#endif

					if (ofxHotReloaderUtils::copyFileFromTo(pathOriginalLib, pathLib)) {
#ifdef _WIN32
						ofxHotReloaderUtils::copyFileFromTo(pathOriginalPdb, pathPdb);
#endif
                        reloaded = load(getGL());
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
		}

		timeLastUpdated = ofxHotReloaderUtils::getElapsedTimeMillis();
	}

	return reloaded;
}

ofxPlugin* ofxHotReloader::createPluginObject(const char * namePlugin) {
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
