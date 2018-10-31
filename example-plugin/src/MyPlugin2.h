#include "ofMain.h"
#include "ofxPluginBase.h"

#include "MyPlugin2Data.h"

class MyPlugin2 : public ofxPluginBase {
	float a = 0;
	float b = 0;

public: 
	MyPlugin2() {
		cout << "create MyPlugin2" << endl;
		 
	}
	 
	~MyPlugin2() {
		cout << "delete MyPlugin2" << endl;
	}

	void test(void* in, void* out) {
		std::cout << "test4" << " a: " << a << std::endl;
	}

	void setup(void* in, void* out) {
		//MyPlugin::mapOfCustomFunctions2["test"] = (CustomFunc)&MyPlugin::test;

		// custom function sample 
		/*
		getMapOfCustomFunctions()["test"] = [this](void* in, void* out) { test(in, out); };

		// custom function sample
		getMapOfCustomFunctions()["test2"] = [](void* in, void* out) {
		std::cout << "test2" << std::endl;
		};
		*/
		
		//mapOfCustomFunctions2["test"] = (CustomFunc)&MyPlugin2::test;
	}


	void draw(void* in, void* out) {
		  
		ofPushStyle();
	  
        ofNoFill();
		ofSetLineWidth(4);
        ofSetColor(ofColor::red);
        ofDrawCircle(a, b, 100);
 
		a += 0.1;
		b += 0.2;
	
		if (a > 200) {
			a = 0;
			b = 0;
		}

		ofPopStyle();
	} 
	 
	void update(void* in, void* out) {
		MyPlugin2DataIn& dataIn = *(MyPlugin2DataIn*)in;
		MyPlugin2DataOut& dataOut = *(MyPlugin2DataOut*)out;

		dataOut.c = dataIn.a + dataIn.b;
	}

	void clone(ofxPluginBase* pluginBase) {
		MyPlugin2* plugin = (MyPlugin2*)pluginBase;

		a = plugin->a;
		b = plugin->b;
	}
}; 

static int MyPlugin2Loader = ([]() {

	ofxPluginUtils::addCustomFunction(typeid(MyPlugin2), "test", (ofxPluginBase::CustomFunc)&MyPlugin2::test);

	ofxPluginUtils::addConstrutor("MyPlugin2", []() {
		return new MyPlugin2();
	});

}(), 0);
