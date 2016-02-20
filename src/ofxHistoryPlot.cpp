/*
 *  ofxHistoryPlot.cpp
 *  emptyExample
 *
 *  Created by Oriol Ferrer Mesià on 28/01/11.
 *  Copyright 2011 uri.cat. All rights reserved.
 *
 */

#include "ofxHistoryPlot.h"
#include <float.h>

ofxHistoryPlot::ofxHistoryPlot(float * val, string varName, float maxHistory, bool autoUpdate_){
	
	autoUpdate = autoUpdate_;
	this->varName = varName;
	valf = val;
	if (val != NULL){
		lowest = *valf * 0.99;
		highest = *valf * 1.01;
	}else{
		lowest = -1;
		highest = 1;
	}
	MAX_HISTORY = maxHistory;
	manualRange = false;
	onlyLowestIsFixed = false;
	count = 1;
	precision = 2;
	lineWidth = 1.0f;
	drawSkip = 1;
	showNumericalInfo = true;
	respectBorders = true;
	drawTitle = true;
	drawBackground = true;
	bgColor = ofColor(0);
	gridColor = ofColor(255,16);
	drawGrid = true;
	shrinkBackInAutoRange = false;
	plotNeedsRefresh = true;
	gridMesh.setMode(OF_PRIMITIVE_LINES);
	plotMesh.setMode(OF_PRIMITIVE_LINE_STRIP);
	plotMesh.getVertices().reserve(MAX_HISTORY * 2);
	smoothPlotMesh.setMode(OF_PRIMITIVE_LINE_STRIP);
	gridUnit = 40;
	smoothFactor = 0.1;
	smoothValue = 0;
	showSmoothedPlot = false;
	scissor = false;
	lineColor = ofColor(255,0,0);
	drawFromRight = false;
}

void ofxHistoryPlot::setMaxHistory(int max){
	MAX_HISTORY = max;
}

void ofxHistoryPlot::reset(){
	values.clear();
	smoothValues.clear();
	count = 0;
}


void ofxHistoryPlot::update(float newVal){

	if (count <= 1 && newVal == newVal/*nan filter*/){
		smoothValue = newVal;
	}

	count++;
	
	if (newVal != newVal && valf != NULL)	//if no value is supplied (default value NAN), use the float* we were given..
		newVal = *valf;	
	
	int skip = 3;
	if((!manualRange || onlyLowestIsFixed) && shrinkBackInAutoRange){
		//if (!autoUpdate) skip = 1;	//if not doing this too fast, no need to skip range processing
		if ( count%skip == 0 ){			
			if (!onlyLowestIsFixed) lowest = FLT_MAX;
			highest = -FLT_MIN;
			for (int i = 0; i < values.size(); i+=skip){
				float val = values[i];
				if (val > highest) highest = val;
				if (!onlyLowestIsFixed) if (val < lowest) lowest = val;
			}	
			if (lowest == FLT_MAX) lowest = -1;
			if (highest == -FLT_MIN) highest = 1;
		}
	}
	if(!manualRange){
		if ( newVal > highest) highest = newVal;
		if ( newVal < lowest && !onlyLowestIsFixed) lowest = newVal;
	}

	values.push_back(newVal);

	if(showSmoothedPlot) {
		smoothValue = newVal * smoothFactor + smoothValue * (1.0f - smoothFactor);
		smoothValues.push_back(smoothValue);
	}

	while (values.size() > MAX_HISTORY){
		values.erase( values.begin() );
	}

	if(showSmoothedPlot) {
		while (smoothValues.size() > MAX_HISTORY){
			smoothValues.erase( smoothValues.begin() );
		}
	}

	plotNeedsRefresh = true;
}

void ofxHistoryPlot::draw(float x, float y){
	draw(x,y,DEFAULT_WIDTH,DEFAULT_HEIGHT);
}

void ofxHistoryPlot::draw(int x, int y){
	draw((float)x,(float)y,DEFAULT_WIDTH,DEFAULT_HEIGHT);
}

void ofxHistoryPlot::refillGridMesh(float x, float y , float w, float h){

	gridMesh.clear();
	int gridH = gridUnit;
	float numLinesH = h / gridH;
	gridMesh.setMode(OF_PRIMITIVE_LINES);
	for(int i = 0; i < numLinesH; i++){
		gridMesh.addVertex( ofVec2f(x,  y + gridH * i) );
		gridMesh.addVertex( ofVec2f(x + w,  y + gridH * i) );
	}
	float numLinesW = w / gridH;
	for(int i = 0; i < numLinesW; i++){
		gridMesh.addVertex( ofVec2f( floor(gridH * 0.5) + x + gridH * i, y ) );
		gridMesh.addVertex( ofVec2f( floor(gridH * 0.5) + x + gridH * i, y + h) );
	}
}


void ofxHistoryPlot::refillPlotMesh(ofVboMesh& mesh, deque<float> & vals, float x, float y , float w, float h){

	mesh.clear();
	int start = 0;
	if (count >= MAX_HISTORY){
		start = drawSkip - (count) % (drawSkip);
	}

	for (int i =  start; i < vals.size(); i+= drawSkip){
		mesh.addVertex(ofVec3f(i, vals[i]));
	}
}


void ofxHistoryPlot::addHorizontalGuide(float yval, ofColor c){
	horizontalGuides.push_back(yval);
	plotNeedsRefresh = true;
	horizontalGuideColors.push_back(c);
}


void ofxHistoryPlot::clearHorizontalGuides(){
	horizontalGuides.clear();
	plotNeedsRefresh = true;
	horizontalGuideColors.clear();
}


void ofxHistoryPlot::draw(float x, float y , float w, float h){

	bool needsMesh = false;
	ofRectangle r = ofRectangle(x,y,w,h);
	if ( r != prevRect || autoUpdate || plotNeedsRefresh){
		needsMesh = true;
		plotNeedsRefresh = false;
	}

	bool needsGrid = r != prevRect;

	ofPushStyle();
	if (autoUpdate) update();
	bool haveData = true;
	if (values.size() == 0 ) haveData = false;

	if (drawBackground){
		ofSetColor(bgColor);
		ofRect(x, y, w, h);
		if (drawGrid){
			if(needsGrid){
				refillGridMesh(x, y, w, h);
			}
			ofSetColor(gridColor);
			ofSetLineWidth(1);
			ofDisableAntiAliasing();
			gridMesh.draw();
			ofEnableAntiAliasing();
		}
	}
	ofSetColor(lineColor);
	float cVal;
	if(haveData) cVal = *values.rbegin();
	if(drawTitle){
		string text = varName + string(haveData ? (" " + ofToString(cVal, precision)) : "");
		ofDrawBitmapString(text, x + w - (text.length()) * 8  , y + 10);
	}
	if ( showNumericalInfo ){
		ofSetColor(85);
		ofDrawBitmapString(ofToString(highest, precision), 1 + x , y + 10);
		ofDrawBitmapString(ofToString(lowest, precision), 1 + x , y + h - 1);
	}

	for(int i = 0; i < horizontalGuides.size(); i++){
		float myY = horizontalGuides[i];
		if (myY > lowest && myY < highest){ //TODO negative!
			float yy = ofMap( myY, lowest, highest, 0, h, true);
			ofSetColor(horizontalGuideColors[i], 50);
			ofDrawBitmapString(ofToString(horizontalGuides[i], precision), 10 + x, y + h - yy + 10 );
			ofSetColor(horizontalGuideColors[i], 64 );
			ofLine( x, y + h - yy, x + w, y + h - yy );
		}
	}


	if (haveData){
		ofNoFill();

		ofSetLineWidth(lineWidth);

			if(needsMesh){
				refillPlotMesh(plotMesh, values, x, y, w, h);
				if (showSmoothedPlot){
					refillPlotMesh(smoothPlotMesh, smoothValues, x, y, w, h);
				}
			}

			if(showSmoothedPlot){
				ofSetColor(lineColor.r * 0.25, lineColor.g * 0.25, lineColor.b * 0.25, lineColor.a);
			}else{
				ofSetColor(lineColor.r, lineColor.g, lineColor.b, lineColor.a);
			}

			ofPushMatrix();
			if(scissor){
				glEnable(GL_SCISSOR_TEST);
				glScissor(x, ofGetHeight() -y -h, w, h);
			}
			if (respectBorders) h -= 12;
			ofTranslate(x,y + h + (respectBorders ? 12 : 0) - 1);
			float plotValuesRange = highest - lowest;
			float yscale = (h-1) / plotValuesRange;
			if(drawFromRight){
				ofTranslate(w , 0);
				ofScale(-1,1);
			}
			ofScale(w / MAX_HISTORY, -yscale );
			ofTranslate(0, -lowest);
			plotMesh.draw();
			if (showSmoothedPlot){
				ofSetColor(lineColor.r, lineColor.g, lineColor.b, lineColor.a);
				smoothPlotMesh.draw();
			}
			if(scissor){
				glDisable(GL_SCISSOR_TEST);
			}
			ofPopMatrix();
		ofFill();
	}
	ofPopStyle();
	prevRect = r;
}


void ofxHistoryPlot::setRange(float low, float high){
	manualRange = true;
	onlyLowestIsFixed = false;
	lowest = low;
	highest = high;
}

void ofxHistoryPlot::setDrawFromRight(bool _val) {
	drawFromRight = _val;
}

float ofxHistoryPlot::getLowerRange(){
	return lowest;
}


float ofxHistoryPlot::getHigerRange(){
	return highest;
}


void ofxHistoryPlot::setLowerRange(float low){
	manualRange = true;
	onlyLowestIsFixed = true;
	lowest = low;
}



string ofxHistoryPlot::getVariableName(){
    return varName;
};