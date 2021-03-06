#include "ofMain.h"
#include "ofxPluginBase.h"

class MyPlugin : public ofxPluginBase {
	float a = 0;
	float b = 0;
	 
public: 
	MyPlugin() {
		cout << "creating MyPlugin" << endl;
	}

	~MyPlugin() {
		cout << "deleting MyPlugin" << endl;
	}
	
	void test(void* in, void* out) {
		std::cout << "test3" << " a: " << a << std::endl;
	}
	 
	void setup(void* in, void* out) override {
	}

	void draw(void* in, void* out) override {
		ofPushView();

		// for JUCE
		if (in != nullptr) {
			float(&fIn)[2] = *reinterpret_cast<float(*)[2]>(in);
			ofSetupScreenOrtho(fIn[0], fIn[1]);
		}

		ofPushMatrix();
		ofPushStyle();

        ofSetColor(ofColor::forestGreen); // pink , forestGreen
		ofDrawRectangle(a + 200, b, 200, 100);

		a += 0.5;
		b += 0.9;

		if (a > 200) {
			a = 0;
			b = 0;
		}

		ofPopStyle();
		ofPopMatrix();
		ofPopView();
	}

	 
	void update(void* in, void* out) override {
		float(&fIn)[2] = *reinterpret_cast<float(*)[2]>(in);
		float(&fOut)[1] = *reinterpret_cast<float(*)[1]>(out);

		fOut[0] = fIn[0] * fIn[1] + 100;
	}

	void clone(ofxPluginBase* pluginBase) override {
		MyPlugin* plugin = (MyPlugin*)pluginBase;
		 
		a = plugin->a;
		b = plugin->b;
	}
}; 

PLUGIN_LOADER_WITH_FUNCTIONS(MyPlugin,
	{
		PLUGIN_ADD_FUNCTION(MyPlugin, test);
	}
);
