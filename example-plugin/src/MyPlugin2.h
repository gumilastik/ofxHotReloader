#include "ofMain.h"
#include "ofxPluginBase.h"

#include "MyPlugin2Data.h"

class MyPlugin2 : public ofxPluginBase {
	float a = 0;
	float b = 0;

public: 
	MyPlugin2() {
		cout << "creating MyPlugin2" << endl;
		 
	}
	 
	~MyPlugin2() {
		cout << "deleting MyPlugin2" << endl;
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
		ofPushView();
		ofPushMatrix();
		ofPushStyle();

		//ofSetupScreenOrtho(0, 0, 800, 600);

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
		ofPopMatrix();
		ofPopView();
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

PLUGIN_LOADER_WITH_FUNCTIONS(MyPlugin2,
	{
		PLUGIN_ADD_FUNCTION(MyPlugin2, test);
	}
);
