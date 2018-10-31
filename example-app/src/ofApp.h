#pragma once

#include "ofMain.h"
#include "ofxHotReloader.h"
#include "../../example-plugin/src/MyPlugin2Data.h"

class ofApp : public ofBaseApp {

	ofxHotReloader hotreloader;
	ofxPlugin* plugin;
	ofxPlugin* plugin2;

	MyPlugin2DataIn in2;
	MyPlugin2DataOut out2;

	float out;

public:

	void reload();

	void setup();
	void update();
	void draw();
	void exit();

	void keyPressed(int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y);
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void mouseEntered(int x, int y);
	void mouseExited(int x, int y);
	void windowResized(int w, int h);
	void dragEvent(ofDragInfo dragInfo);
	void gotMessage(ofMessage msg);

};
