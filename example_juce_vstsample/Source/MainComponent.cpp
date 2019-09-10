/*
  ==============================================================================

	This file was auto-generated!

  ==============================================================================
*/

#include "MainComponent.h"
#include "../../src/ofxHotReloaderUtils.cpp"
#include "../../src/ofxHotReloader.cpp"
#include "../../src/ofxPlugin.cpp"

//==============================================================================
MainComponent::MainComponent()
{
	// Initialise GL objects for rendering here.
	hotreloader = new ofxHotReloader();

	std::string packageName = "com.company.application";
#if defined (_WIN32)
	std::string pathPlugin = juce::File::getSpecialLocation(juce::File::SpecialLocationType::userApplicationDataDirectory).getChildFile(packageName).getFullPathName().toStdString();
	hotreloader->setup("D:/Developer/Crossplatform/of_v0.10.1_vs2017_release/addons/ofxHotReloader/example-plugin/bin/", "example-plugin", pathPlugin);
#elif defined (__APPLE__)
	std::string pathPlugin = juce::File::getSpecialLocation(juce::File::SpecialLocationType::userApplicationDataDirectory).getChildFile("Application Support").getChildFile(packageName).getFullPathName().toStdString();
	hotreloader->setup("/Users/user/Developer/openFrameworks/of_v0.10.1_osx_release/addons/ofxHotReloader/example-plugin/bin/", "example-plugin", pathPlugin);
#endif

	plugin = hotreloader->createPluginObject("MyPlugin");
	reload();

	hotreloader->addCallbackAfterLoad(
		[&]() -> void {
		std::cout << "callback" << std::endl;
		reload();
	}
	);


	// Make sure you set the size of the component after
	// you add any child components.
	setSize(800, 600);

}

MainComponent::~MainComponent()
{
	if (plugin) {
		delete plugin;
		plugin = nullptr;
	}

	if (hotreloader) {
		delete hotreloader;
		hotreloader = nullptr;
	}

	// This shuts down the GL system and stops the rendering calls.
	shutdownOpenGL();
}

//==============================================================================
void MainComponent::initialise()
{
}

void MainComponent::reload()
{
	if (plugin) plugin->setup();
}

void MainComponent::shutdown()
{
	// Free any GL objects created for rendering here.
}

void MainComponent::render()
{
	hotreloader->check();

	if (plugin) {
		float in[2] = { 2, 3 };
		plugin->update(&in, &out);

		plugin->custom("test");
		plugin->custom("test2");
	}

	// This clears the context with a black background.
	//OpenGLHelpers::clear (Colours::black);

	auto desktopScale = (float)openGLContext.getRenderingScale();
	OpenGLHelpers::clear(getLookAndFeel().findColour(ResizableWindow::backgroundColourId));

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glViewport(0, 0, roundToInt(desktopScale * getWidth()), roundToInt(desktopScale * getHeight()));

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glBegin(GL_POINTS);
	glVertex3f(0.0, 0.5, 0.0);//drawn white or default color first loop and green all other loops
	glColor3f(1.0, 0.5, 0.0);//set red 100% green 50% blue 0%
	glVertex3f(0.1, 0.5, 0.0);//drawn orange
	glVertex3f(0.2, 0.5, 0.0);//drawn orange
	glColor3f(1.0, 0.0, 0.0);//set red 100% green 0% blue 0%
	glVertex3f(0.3, 0.5, 0.0);//drawn red
	glEnd();
	glBegin(GL_POINTS);
	glVertex3f(0.4, 0.5, 0.0);//drawn red
	glColor3f(0.0, 1.0, 0.0);//set red 0% green 100% blue 0%
	glVertex3f(0.5, 0.5, 0.0);//drawn green
	glVertex3f(0.6, 0.5, 0.0);//drawn green
	glEnd();

	if (plugin) {
		float in[2] = { (float)roundToInt(desktopScale * getWidth()), (float)roundToInt(desktopScale * getHeight()) };
		plugin->draw(in);
	}

}

//==============================================================================
void MainComponent::paint(Graphics& g)
{
	// You can add your component specific drawing code here!
	// This will draw over the top of the openGL background.
}

void MainComponent::resized()
{
	// This is called when the MainComponent is resized.
	// If you add any child components, this is where you should
	// update their positions.
}
