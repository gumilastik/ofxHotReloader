#pragma once

#include "ofMain.h"

#include <typeindex>
#include <typeinfo>

class ofxPluginBase {
public:
	typedef void(ofxPluginBase::*CustomFunc)(void*, void*);

	ofxPluginBase() {}
	virtual ~ofxPluginBase() {}
	 
	virtual void setup(void* in = nullptr, void* out = nullptr);
	virtual void update(void* in = nullptr, void* out = nullptr);
	virtual void draw(void* in = nullptr, void* out = nullptr);

	virtual void custom(char* name = nullptr, void* in = nullptr, void* out = nullptr);
	virtual void clone(ofxPluginBase* obj) {};
}; 

namespace ofxPluginUtils {
	void addConstrutor(std::string name, std::function<void*()> function);
	void addCustomFunction(const std::type_info& type, char* name, ofxPluginBase::CustomFunc func);
};


#ifdef _WIN32
#define HOTRELOADER_API __declspec(dllexport)
#else
#define HOTRELOADER_API
#endif

extern "C" {
	HOTRELOADER_API void initMainLoop(void* ptr);
    HOTRELOADER_API void* createPlugin(char* name);
}
