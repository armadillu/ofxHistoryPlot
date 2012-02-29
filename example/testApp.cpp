#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){

	ofEnableSmoothing();
	ofEnableAlphaBlending();
	ofSetVerticalSync(true);
	ofSetFrameRate(60);
	ofBackground(22, 22, 22, 255);

	mouseIsPressed = false;
	int numSamples = 500;
	
	plot = new ofxHistoryPlot( NULL, "mouseY manual update", numSamples, false); //NULL cos we don't want it to auto-update. confirmed by "true"
	plot->setRange(0, ofGetHeight());
	plot->setColor( ofColor(255,0,0) );
	plot->setShowNumericalInfo(true);
	plot->setRespectBorders(true);
	plot->setLineWidth(5);
	
	plot2 = new ofxHistoryPlot( &currentFrameRate, "currentFrameRate auto updated", numSamples, true);	//true for autoupdate
	plot2->setLowerRange(0);
	plot2->setColor( ofColor(0,255,0) );
	plot2->setShowNumericalInfo(true);
	plot2->setRespectBorders(true);
	plot2->setLineWidth(5);
	
	plot->update(0);

}

//--------------------------------------------------------------
void testApp::update(){

	currentFrameRate = ofGetFrameRate();
	
	if (mouseIsPressed){	
		plot->update(mouseY);	//manually add values to the plot
	}
	
	//notice how there's no update on plot2
	
}

//--------------------------------------------------------------
void testApp::draw(){

	plot->draw(10, 10, 640, 240);
	plot2->draw(10, 10 + 10 + 240, 640, 240);

	ofDrawBitmapString("press mouse button and drag", 10, 520 );
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){

	plot->reset();
	plot2->reset();
}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){
	mouseIsPressed = true;

}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){
	mouseIsPressed = false;
}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 

}