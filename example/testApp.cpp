#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){

	ofEnableSmoothing();
	ofEnableAlphaBlending();
	ofSetVerticalSync(true);
	ofSetFrameRate(60);
	ofBackground(22, 22, 22, 255);

	mouseIsPressed = false;
	int numSamples = 350;
	
	plot = new ofxHistoryPlot( NULL, "mouseY", numSamples, false); //NULL cos we don't want it to auto-update. confirmed by "true"
	plot->setRange(0, ofGetHeight()); //hard range, will not adapt to values off-scale
	plot->addHorizontalGuide(ofGetHeight()/2, ofColor(255,0,0)); //add custom reference guides
	plot->setColor( ofColor(0,255,0) ); //color of the plot line
	plot->setShowNumericalInfo(true);  //show the current value and the scale in the plot
	plot->setRespectBorders(true);	   //dont let the plot draw on top of text
	plot->setLineWidth(1);				//plot line width
	plot->setBackgroundColor(ofColor(0,220)); //custom bg color
	//custom grid setup
	plot->setDrawGrid(true);
	plot->setGridColor(ofColor(30)); //grid lines color
	plot->setGridUnit(14);
	plot->setCropToRect(true);


	plot2 = new ofxHistoryPlot( &currentFrameRate, "currentFrameRate auto updated", numSamples, true);	//true for autoupdate
	//plot2->setLowerRange(0); //set only the lowest part of the range upper is adaptative to curve
	plot2->setAutoRangeShrinksBack(true); //plot scale can shrink back after growing if plot curves requires it
	plot2->setColor( ofColor(255,0,255) );
	plot2->setShowNumericalInfo(true);
	plot2->setRespectBorders(true);
	plot2->setLineWidth(1);
	plot2->setDrawFromRight(true);

	plot2->setShowSmoothedCurve(true); //plot a smoothed version of the values, but alos the original in lesser alpha
	plot2->setSmoothFilter(0.1); //smooth filter strength
	plot2->setCropToRect(true);
	plot2->update(0);

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