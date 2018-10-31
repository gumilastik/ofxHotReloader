#pragma once

#include "ofxPluginBase.h"
 
class ofxPlugin {
	ofxPluginBase* plugin;
	std::vector<ofxPlugin*>* plugins;
	string namePlugin;

	friend class ofxHotReloader;

public:
	ofxPlugin(ofxPluginBase* plugin, std::vector<ofxPlugin*>* plugins, string namePlugin);
	~ofxPlugin();

	void setup(void* in = nullptr, void* out = nullptr);
	void update(void* in = nullptr, void* out = nullptr);
	void draw(void* in = nullptr, void* out = nullptr);

	void custom(char* name, void* in = nullptr, void* out = nullptr);
	void clone(ofxPluginBase* obj);
};
 