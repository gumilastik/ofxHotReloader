#pragma once

#if defined(_WIN32)
#include <windows.h>
#elif defined(__APPLE__)
#endif

#include <typeindex>
#include <typeinfo>

#include <functional>
#include <string>
#include <vector>
#include <map>

#if defined(JUCE_APP_VERSION)
#include "../JuceLibraryCode/JuceHeader.h"
#else
#include "ofMain.h"
#endif

#define PLUGIN_ADD_FUNCTION(className, funcName) ofxPluginUtils::addCustomFunction(typeid(className), #funcName, (ofxPluginBase::CustomFunc)&className::funcName)

#define PLUGIN_ADD_CONSTRUCTOR(className) \
	ofxPluginUtils::addConstrutor(#className, []() { \
		return new className(); \
	});

#define PLUGIN_LOADER(className) \
static int _loader_##className = ([]() { \
 \
PLUGIN_ADD_CONSTRUCTOR(className) \
 \
}(), 0);

#define PLUGIN_LOADER_WITH_FUNCTIONS(className, code) \
static int _loader_##className = ([]() { \
 \
PLUGIN_ADD_CONSTRUCTOR(className) \
code \
 \
}(), 0);

class ofxPluginBase {
public:
	typedef void(ofxPluginBase::*CustomFunc)(void*, void*);

	ofxPluginBase();
	virtual ~ofxPluginBase();

	virtual ofxPluginBase* getPtrPlugin();;

	virtual void setup(void* in = nullptr, void* out = nullptr);
	virtual void update(void* in = nullptr, void* out = nullptr);
	virtual void process(void* in = nullptr, void* out = nullptr);
	virtual void draw(void* in = nullptr, void* out = nullptr);
	virtual void cleanup(void* in = nullptr, void* out = nullptr);

	virtual void custom(char* name = nullptr, void* in = nullptr, void* out = nullptr);
	virtual void clone(ofxPluginBase* obj);

}; 

namespace ofxPluginUtils {
	void addConstrutor(std::string name, std::function<void*()> function);
	void addCustomFunction(const std::type_info& type, char* name, ofxPluginBase::CustomFunc func);
};

#if defined(_WIN32)
#define HOTRELOADER_API __declspec(dllexport)
#else
#define HOTRELOADER_API
#endif

extern "C" {
	HOTRELOADER_API void initGL(void* ptr);
	HOTRELOADER_API void* getGL();
	HOTRELOADER_API void deinitGL();
	HOTRELOADER_API void* createPlugin(const char* name);
}
