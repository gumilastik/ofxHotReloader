#include "ofxPluginBase.h"

std::map<std::string, std::function<void*()>>& getMapOfConstrutors()
{
	static std::map<std::string, std::function<void*()>> mapOfConstrutors;
	return mapOfConstrutors;
}

std::map<std::pair<std::type_index, std::string>, ofxPluginBase::CustomFunc>& getMapOfCustomFunctions() {
	static std::map<std::pair<std::type_index, std::string>, ofxPluginBase::CustomFunc> mapOfCustomFunctions;
	return mapOfCustomFunctions;
}

void ofxPluginUtils::addConstrutor(std::string name, std::function<void*()> function)
{
	getMapOfConstrutors()[name] = function;
}

void ofxPluginUtils::addCustomFunction(const std::type_info& type, char* name, ofxPluginBase::CustomFunc func) {
	getMapOfCustomFunctions()[std::make_pair<std::type_index, std::string>(type, name)] = func;
}

void ofxPluginBase::setup(void * in, void * out) {
}

void ofxPluginBase::update(void * in, void * out) {
}

void ofxPluginBase::draw(void * in, void * out) {
}

void ofxPluginBase::custom(char * name, void * in, void * out) {
	if (name != nullptr && getMapOfCustomFunctions().find(std::make_pair<std::type_index, std::string>(typeid(*this), name)) != getMapOfCustomFunctions().end()) {
		(this->*getMapOfCustomFunctions()[std::make_pair<std::type_index, std::string>(typeid(*this), name)])(in, out);
	}
}

void initMainLoop(void* ptr)
{
	ofSetMainLoop(shared_ptr<ofMainLoop>((ofMainLoop*)ptr));
}

void* createPlugin(char* name)
{
	ofxPluginBase* ptr = nullptr;

	if (getMapOfConstrutors().find(name) != getMapOfConstrutors().end()) {
		ptr = (ofxPluginBase*)getMapOfConstrutors()[name]();
	}

	return ptr;
}

// lib entry point
#ifdef _WIN32
BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}
#elif __APPLE__
__attribute__((constructor)) void DllMain()
{
}
#endif

