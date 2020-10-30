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

using namespace std;

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
	rangeMode = RANGE_AUTOMATIC;
	count = 0;
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
	drawGuideValues = false;
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
	lowest= -1; highest = 1;
}

void ofxHistoryPlot::recalcLowHigh(){
	float lowest = FLT_MAX;
	float highest = -FLT_MIN;
	for (size_t i = 0; i < values.size(); i ++){
		const float val = values[i];
		if (val > highest) highest = val;
		if (val < lowest) lowest = val;
	}
	if (lowest == FLT_MAX) lowest = -1;
	if (highest == -FLT_MIN) highest = 1;

	if(shrinkBackInAutoRange){
		this->lowest = lowest;
		this->highest = highest;
	}else{ //never shrink range
		if(this->lowest < lowest ) this->lowest = lowest;
		if(this->highest > highest) this->highest = highest;
	}
}

void ofxHistoryPlot::update(float newVal){

	if (count < 1 && newVal == newVal/*nan filter*/){
		smoothValue = newVal;
	}

	count++;
	
	if (newVal != newVal && valf != NULL)	//if no value is supplied (default newVal value NAN), use the float* we were given..
		newVal = *valf;	
	
	if (newVal > highest) highest = newVal;
	if (newVal < lowest) lowest = newVal;

	values.push_back(newVal);

	if(showSmoothedPlot) {
		smoothValue = newVal * smoothFactor + smoothValue * (1.0f - smoothFactor);
		smoothValues.push_back(smoothValue);
	}

	bool needsRangeRecalc = false;

	while (values.size() > MAX_HISTORY){
		float valToDelete = *values.begin();
		if(valToDelete <= lowest || valToDelete >= highest){ //we are deleting values from plot, were theey max or min?
			needsRangeRecalc = true;
		}
		values.erase( values.begin() );
	}

	if(needsRangeRecalc || count%60 == 0){
		recalcLowHigh();
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
	float numLinesH = floor(h / gridH);
	gridMesh.setMode(OF_PRIMITIVE_LINES);
	for(int i = 0; i < numLinesH; i++){
		gridMesh.addVertex( ofVec3f(x,  y + gridH * i) );
		gridMesh.addVertex( ofVec3f(x + w,  y + gridH * i) );
	}
	float numLinesW = floor(w / gridH);
	for(int i = 0; i < numLinesW; i++){
		gridMesh.addVertex( ofVec3f( floor(gridH * 0.5) + x + gridH * i, y ) );
		gridMesh.addVertex( ofVec3f( floor(gridH * 0.5) + x + gridH * i, y + h) );
	}
}


void ofxHistoryPlot::refillPlotMesh(ofVboMesh& mesh, deque<float> & vals, float x, float y , float w, float h){

	mesh.clear();
	int start = 0;
	for (size_t i =  start; i < vals.size(); i+= drawSkip){
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

	float plotLow;
	float plotHigh;

	switch (rangeMode) {
		case RANGE_MANUAL: plotLow = manualLowest; plotHigh = manualHighest; break;
		case RANGE_LOWER_FIXED: plotLow = manualLowest; plotHigh = highest; break;
		case RANGE_AUTOMATIC: plotLow = lowest; plotHigh = highest; break;
	}

	bool needsGrid = r != prevRect;

	ofPushStyle();
	if (autoUpdate) update();
	bool haveData = true;
	if (values.size() == 0 ) haveData = false;

	if (drawBackground){
		ofSetColor(bgColor);
		ofDrawRectangle(x, y, w, h);
	}
	if (drawGrid){
		if(needsGrid || gridMesh.getVertices().size() == 0){
			refillGridMesh(x, y, w, h);
		}
		ofSetColor(gridColor);
		ofSetLineWidth(1);
		ofDisableAntiAliasing();
		gridMesh.draw();
		ofEnableAntiAliasing();
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
		ofDrawBitmapString(ofToString(plotHigh, precision), 1 + x , y + 10);
		ofDrawBitmapString(ofToString(plotLow, precision), 1 + x , y + h - 1);
	}

	for(size_t i = 0; i < horizontalGuides.size(); i++){
		float myY = horizontalGuides[i];
		if (myY > plotLow && myY < plotHigh){ //TODO negative!
			float yy = ofMap( myY, plotLow, plotHigh, 0, h, true);
			if(drawGuideValues){
				ofSetColor(horizontalGuideColors[i], 50);
				ofDrawBitmapString(ofToString(horizontalGuides[i], precision), 10 + x, y + h - yy + 10 );
			}
			ofSetColor(horizontalGuideColors[i], 64 );
			ofDrawLine( x, y + h - yy, x + w, y + h - yy );
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
				ofSetColor(lineColor);
			}

			ofPushMatrix();
			if(scissor){
				glEnable(GL_SCISSOR_TEST);
				glScissor(x, ofGetViewportHeight() -y -h, w, h);
			}
			if (respectBorders) h -= 12;
			ofTranslate(x,y + h + (respectBorders ? 12 : 0) - 1);
			float plotValuesRange = plotHigh - plotLow;
			float yscale = (h-1) / plotValuesRange;
			if(drawFromRight){
				ofTranslate(w , 0);
				ofScale(-1,1);
			}
			ofScale(w / MAX_HISTORY, -yscale );
			ofTranslate(0, -plotLow);
			plotMesh.draw();
			if (showSmoothedPlot){
				ofSetColor(lineColor);
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


void ofxHistoryPlot::setRangeAuto(){
	rangeMode = RANGE_AUTOMATIC;
}

void ofxHistoryPlot::setRange(float low, float high){
	rangeMode = RANGE_MANUAL;
	manualLowest = low;
	manualHighest = high;
}

void ofxHistoryPlot::setDrawFromRight(bool _val) {
	drawFromRight = _val;
}

float ofxHistoryPlot::getLowerRange(){
	switch (rangeMode) {
		case RANGE_MANUAL: return manualLowest;
		case RANGE_LOWER_FIXED: return manualLowest;
		case RANGE_AUTOMATIC: return lowest;
	}
	return -1.0;
}


float ofxHistoryPlot::getHigerRange(){
	switch (rangeMode) {
		case RANGE_MANUAL: return manualHighest;
		case RANGE_LOWER_FIXED: return highest;
		case RANGE_AUTOMATIC: return highest;
	}
	return 1.0;
}


void ofxHistoryPlot::setLowerRange(float low){
	rangeMode = RANGE_LOWER_FIXED;
	manualLowest = low;
}

float ofxHistoryPlot::getLowestValue(){
	return lowest;
}

float ofxHistoryPlot::getHighestValue(){
	return highest;
}


void ofxHistoryPlot::setVariableName(const std::string & var){varName = var;};
string ofxHistoryPlot::getVariableName(){return varName;};


void ofxHistoryPlot::setAutoRangeShrinksBack(bool shrink){
	shrinkBackInAutoRange = shrink;
};

void ofxHistoryPlot::setDrawGuideValues(bool doIt){drawGuideValues = doIt;}

void ofxHistoryPlot::setPrecision(int prec){ precision = ofClamp(prec, 0, 15); }

void ofxHistoryPlot::setColor(const ofColor & c){ lineColor = c;}
ofColor ofxHistoryPlot::getColor(){return lineColor;}
void ofxHistoryPlot::setBackgroundColor(const ofColor & c){ bgColor = c;}

void ofxHistoryPlot::setDrawGrid(bool d) { drawGrid = d;}
void ofxHistoryPlot::setGridColor(ofColor c){ gridColor = c;}
void ofxHistoryPlot::setGridUnit(float g){gridUnit = g;}


void ofxHistoryPlot::setShowNumericalInfo(bool show){ showNumericalInfo = show;}
void ofxHistoryPlot::setRespectBorders(bool respect){respectBorders = respect;}
void ofxHistoryPlot::setDrawSkipVal(int skip){ drawSkip = skip; if (drawSkip < 1) drawSkip = 1;}
void ofxHistoryPlot::setLineWidth(float w){ lineWidth = w;}
void ofxHistoryPlot::setDrawBackground(bool d) { drawBackground = d;}
void ofxHistoryPlot::setDrawTitle(bool doit){drawTitle = doit;}

void ofxHistoryPlot::setShowSmoothedCurve(bool show){showSmoothedPlot = show;}
void ofxHistoryPlot::setSmoothFilter(float filter){smoothFactor = filter;};

void ofxHistoryPlot::setCropToRect(bool s){scissor = s;}

std::deque<float>& ofxHistoryPlot::getValues(){ return values; }
