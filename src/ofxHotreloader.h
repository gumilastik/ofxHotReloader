#pragma once

#if defined(_WIN32)
#include <windows.h>
#elif defined(__APPLE__)
#endif

#include <iostream>
#include <ctime>

#include "ofxPlugin.h"
#include "Utils.h"

class ofxHotReloader {

private:
	typedef void(*FuncInitGL)(void*);
	FuncInitGL funcInitGL;

	typedef void*(*FuncGetGL)(void);
	FuncGetGL funcGetGL;

	typedef void(*FuncDeinitGL)(void);
	FuncDeinitGL funcDeinitGL;

	typedef void*(*FuncCreatePlugin)(char*);
	FuncCreatePlugin funcCreatePlugin;

	typedef std::function<void()> CallbackOnLoaded;
	CallbackOnLoaded callbackOnLoaded;

#if defined(_WIN32)
	HINSTANCE instanceLib;
#elif defined(__APPLE__)
	void* instanceLib;
#endif

	std::string nameTempDir;

	unsigned long long timeLastUpdated;
	unsigned long long timeUpdateInterval;

	unsigned long long tDest;

    std::string pathPlugins;
	std::string pathOriginalLib;
	std::string pathOriginalPdb;
	bool loaded;

	std::vector<ofxPlugin*> plugins;

	void initGL(void* ptr);
	void* getGL();
	void deinitGL();
	void generatePath(std::string pathDir, std::string& pathLib, std::string& pathPdb);
	void load(void* ptrPrevGL = nullptr);

public:
	ofxHotReloader();
	~ofxHotReloader();

	void setup(std::string pathOriginalLib, std::string pathPlugins, unsigned long long timeUpdateInterval = 2000);
	void addCallbackOnLoaded(CallbackOnLoaded callbackOnLoaded);

	void check(bool force = false);
	ofxPlugin* createPluginObject(char* namePlugin);
};
