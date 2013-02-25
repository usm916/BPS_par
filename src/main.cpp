#include "ofMain.h"
#include "testApp.h"
#include "ofAppGlutWindow.h"

int main() {
	ofAppGlutWindow window;
//	ofSetupOpenGL(&window, 1280, 256, OF_WINDOW);
//	ofSetupOpenGL(&window, 800, 600, OF_WINDOW);
//	ofSetupOpenGL(&window, 1024, 768, OF_WINDOW);
	ofSetupOpenGL(&window, 1280, 720, OF_FULLSCREEN);
//	ofSetupOpenGL(&window, 1920, 1080, OF_WINDOW);
	ofRunApp(new testApp());
}
