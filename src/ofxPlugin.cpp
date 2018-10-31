#include "ofxPlugin.h"

ofxPlugin::ofxPlugin(ofxPluginBase * plugin, std::vector<ofxPlugin*>* plugins, string namePlugin) {
	this->plugin = plugin;
	this->plugins = plugins;
	this->namePlugin = namePlugin;
}

ofxPlugin::~ofxPlugin() {
	if (plugins) {
		for (size_t i = 0; i < plugins->size(); i++) {
			if (plugins->operator[](i) == this) {
				plugins->erase(plugins->begin() + i);
				break;
			}
		}
		plugins = nullptr;
	}

	if (plugin) {
		delete plugin;
		plugin = nullptr;
	}
}

void ofxPlugin::setup(void* in, void* out) {
	if (plugin) plugin->setup(in, out);
}

void ofxPlugin::update(void* in, void* out) {
	if (plugin) plugin->update(in, out);
}

void ofxPlugin::draw(void* in, void* out) {
	if (plugin) plugin->draw(in, out);
}

void ofxPlugin::custom(char* name, void* in, void* out) {
	if (plugin) plugin->custom(name, in, out);
}

void ofxPlugin::clone(ofxPluginBase* obj) {
	if (plugin) plugin->clone(obj);
}
