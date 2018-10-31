#pragma once

#include "ofxPlugin.h"
 
class ofxHotReloader {

private:
	typedef void(*FuncInitMainLoop)(void*);
	FuncInitMainLoop funcInitMainLoop;

	typedef void*(*FuncCreatePlugin)(char*);
	FuncCreatePlugin funcCreatePlugin;

	typedef std::function<void()> CallbackOnLoaded;
	CallbackOnLoaded callbackOnLoaded;

#ifdef _WIN32
	HINSTANCE instanceLib;
#elif __APPLE__
	void* instanceLib;
#endif

	string nameTempDir;

	float timeLastUpdated;
	float timeUpdateInterval;

	std::time_t tDest;

	string pathOriginalLib;
	string pathOriginalPdb;
	bool loaded;

	std::vector<ofxPlugin*> plugins;

	void clear();
	void initMainLoop(void* ptr);
	void generatePath(string pathDir, string& pathLib, string& pathPdb);
	void load();

public:
	ofxHotReloader();
	~ofxHotReloader();

	void setup(string pathOriginalLib, float timeUpdateInterval = 2000);
	void addCallbackOnLoaded(CallbackOnLoaded callbackOnLoaded);

	void check(bool force = false);
	ofxPlugin* createPluginObject(char* namePlugin);
};
