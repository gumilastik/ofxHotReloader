#include "ofxHotreloader.h"

#ifdef _WIN32
#include <windows.h>
#elif __APPLE__
#include <dlfcn.h>
#endif

void ofxHotReloader::clear() {
	ofDirectory dir = ofDirectory("plugins");
    std::vector<ofFile> files = dir.getFiles();
	for (size_t i = 0; i < files.size(); i++) {
		if (ofDirectory(files[i]).isDirectory()) {
			ofDirectory::removeDirectory(files[i].getAbsolutePath(), true, false);
		}
	}
}

void ofxHotReloader::initMainLoop(void * ptr) {
	if (!loaded || funcInitMainLoop == NULL) return;

	(funcInitMainLoop)(ptr);
}

void ofxHotReloader::generatePath(string pathDir, string & pathLib, string & pathPdb) {
	ofDirectory dir(pathDir);
	if (!dir.exists()) dir.create(true);

    pathLib = ofToDataPath(ofFilePath::join(dir.getAbsolutePath(), ofFilePath::getFileName(pathOriginalLib)), true);
	pathPdb = ofFilePath::getEnclosingDirectory(pathLib) + ofFilePath::getBaseName(pathLib) + ".pdb";
}

void ofxHotReloader::load() {
	ofDirectory dir;
	string pathLib;
	string pathPdb;

	generatePath(ofFilePath::join("plugins", nameTempDir), pathLib, pathPdb);

#ifdef _WIN32
	instanceLib = LoadLibraryA(pathLib.c_str());
#elif __APPLE__
    // change id of dylib to temp id
    ofSystem("install_name_tool -id /" + ofFilePath::getBaseName(pathOriginalLib) + (nameTempDir != "" ? "." + nameTempDir : "") + "." + ofFilePath::getFileExt(pathOriginalLib) + " \"" + pathLib + "\"");
    instanceLib = dlopen(pathLib.c_str(), RTLD_LAZY);
#endif

    if (!instanceLib) {
#ifdef _WIN32
        std::cout << "could not load the dynamic library: " << GetLastError() << std::endl;
#elif __APPLE__
        std::cout << "could not load the dynamic library: " << dlerror() << std::endl;
#endif

		funcInitMainLoop = NULL;
		funcCreatePlugin = NULL;

		loaded = false;
	}
	else {
		// resolve function address here
#ifdef _WIN32
		funcInitMainLoop = (FuncInitMainLoop)GetProcAddress(instanceLib, "initMainLoop");
		funcCreatePlugin = (FuncCreatePlugin)GetProcAddress(instanceLib, "createPlugin");
#elif __APPLE__
		funcInitMainLoop = (FuncInitMainLoop)dlsym(instanceLib, "initMainLoop");
		funcCreatePlugin = (FuncCreatePlugin)dlsym(instanceLib, "createPlugin");
#endif

		generatePath("plugins", pathLib, pathPdb);

		ofFile::copyFromTo(pathOriginalLib, pathLib, false, true);
        if(ofFile(pathOriginalPdb).exists()) {
            ofFile::copyFromTo(pathOriginalPdb, pathPdb, false, true);
        }
        
		loaded = true;
 
		initMainLoop((void*)ofGetMainLoop().get());

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

ofxHotReloader::ofxHotReloader() {
	nameTempDir = "";
	loaded = false;
	callbackOnLoaded = nullptr;
    instanceLib = NULL;
    
	clear();
}

ofxHotReloader::~ofxHotReloader() {
	clear();
}

void ofxHotReloader::setup(string pathOriginalLib, float timeUpdateInterval) {
	this->pathOriginalLib = ofFilePath::getAbsolutePath(pathOriginalLib, true);
	this->pathOriginalPdb = ofFilePath::getEnclosingDirectory(pathOriginalLib) + ofFilePath::getBaseName(pathOriginalLib) + ".pdb";

	this->timeUpdateInterval = timeUpdateInterval;

	check(true);
}

void ofxHotReloader::addCallbackOnLoaded(CallbackOnLoaded callbackOnLoaded) {
	this->callbackOnLoaded = callbackOnLoaded;
}

void ofxHotReloader::check(bool force) {
	if (force || ofGetElapsedTimeMillis() - timeLastUpdated > timeUpdateInterval) {

		ofFile fSrc(pathOriginalLib);
		if (!fSrc.exists()) {
			if (!loaded) {
				load();
			}
			return;
		}

		std::time_t tSrc = std::filesystem::last_write_time(fSrc);
		fSrc.close();

		ofDirectory dir;
		string pathLib;
		string pathPdb;
		
		generatePath(ofFilePath::join("plugins", nameTempDir), pathLib, pathPdb);

		ofFile fDest(pathLib);
		if (!fDest.exists() || tSrc > tDest) {
			fDest.close();

			// check for open for read
			FILE *fp = fopen(pathOriginalLib.c_str(), "rb");
			if (fp != NULL) {
				fclose(fp);

				std::cout << "reload" << std::endl;

				tDest = tSrc;
				nameTempDir = ofToString(tSrc);

				generatePath(ofFilePath::join("plugins", nameTempDir), pathLib, pathPdb);

#ifdef _WIN32
				HINSTANCE instanceLibPrevious = instanceLib;
#elif __APPLE__
				void* instanceLibPrevious = instanceLib;
#endif

				if (ofFile::copyFromTo(pathOriginalLib, pathLib, false, true)) {
                    if(ofFile(pathOriginalPdb).exists()) {
                        ofFile::copyFromTo(pathOriginalPdb, pathPdb, false, true);
                    }
					load();
				}

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
			}
		}
		timeLastUpdated = ofGetElapsedTimeMillis();
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
