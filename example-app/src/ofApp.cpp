#include "ofApp.h"


//--------------------------------------------------------------
void ofApp::reload()
{
	if (!plugin) plugin = hotreloader.createPluginObject("MyPlugin");
	if (!plugin2) plugin2 = hotreloader.createPluginObject("MyPlugin2");
	if (!plugin3) plugin3 = new MyPlugin();

	if (plugin) plugin->setup();
	if (plugin2) plugin2->setup();
	if (plugin3) plugin3->setup();

}

//--------------------------------------------------------------
void ofApp::setup() { 

	hotreloader.setup("../../../example-plugin/bin/", "example-plugin", "plugins");

	reload();

	hotreloader.addCallbackAfterLoad(
		[&]() -> void {
			std::cout << "callback" << std::endl;
			reload();
		}
	); 
}

//--------------------------------------------------------------
void ofApp::update() {

	hotreloader.check();

	if (plugin) {
		float in[2] = { 2, 3 };
		plugin->update(&in, &out);

		plugin->custom("test");
		plugin->custom("test2");
	}

	if (plugin2) {
		
		in2.a = 2;
		in2.b = 3;
		
		plugin2->update(&in2, &out2);
		plugin2->custom("test");
	}

	if (plugin3) {
		float in[2] = { 2, 3 };
		plugin3->update(&in, &out);

		plugin3->custom("test");
		plugin3->custom("test2");
	}


}

//--------------------------------------------------------------
void ofApp::draw() {
	if (plugin) plugin->draw();
	if (plugin2) plugin2->draw();
	if (plugin3) plugin3->draw();

	ofDrawBitmapString("plugin update output  : " + ofToString(out), 10, 20);
	ofDrawBitmapString("plugin2 update output : " + ofToString(out2.c), 10, 40);
}

//--------------------------------------------------------------
void ofApp::exit() {
	if (plugin) delete plugin;
	if (plugin2) delete plugin2;
	if (plugin3) delete plugin3;
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key) {

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y) {

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button) {

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button) {

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button) {

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y) {

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y) {

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h) {

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg) {

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo) {

}
