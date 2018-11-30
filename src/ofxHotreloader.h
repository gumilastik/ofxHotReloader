#pragma once

#if defined(_WIN32)
#include <windows.h>
#elif defined(__APPLE__)
#endif

#include "ofxPlugin.h"

class ofxHotReloader {
private:
	typedef void(*FuncInitGL)(void*);
	FuncInitGL funcInitGL;

	typedef void*(*FuncGetGL)(void);
	FuncGetGL funcGetGL;

	typedef void(*FuncDeinitGL)(void);
	FuncDeinitGL funcDeinitGL;

	typedef void*(*FuncCreatePlugin)(const char*);
	FuncCreatePlugin funcCreatePlugin;

	typedef std::function<void()> CallbackLoad;
	CallbackLoad callbackBeforeLoad;
	CallbackLoad callbackAfterLoad;

#if defined(_WIN32)
	HINSTANCE instanceLib;
#elif defined(__APPLE__)
	void* instanceLib;
#endif

	std::string nameTempDir;

	unsigned long long timeLastUpdated;
	unsigned long long timeUpdateInterval;

	unsigned long long tDest;

    std::string pathPluginsFolder;
	std::string pathOriginalLib;
	std::string pathOriginalPdb;
	bool loaded;

	std::vector<ofxPlugin*> plugins;

	void initGL(void* ptr);
	void* getGL();
	void deinitGL();
	void generatePath(std::string pathDir, std::string& pathLib, std::string& pathPdb);
	bool load(void* ptrPrevGL = nullptr);

public:
	ofxHotReloader();
	~ofxHotReloader();

	void setup(std::string pathOriginalLibFolder, std::string pathOriginalLibName, std::string pathPluginsFolder = "plugins", unsigned long long timeUpdateInterval = 2000);
	void addCallbackBeforeLoad(CallbackLoad callbackBeforeLoad);
	void addCallbackAfterLoad(CallbackLoad callbackAfterLoad);
	
	bool check(bool force = false);
	ofxPlugin* createPluginObject(const char* namePlugin);
};


