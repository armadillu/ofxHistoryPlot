/*
 *  ofxHistoryPlot.h
 *  emptyExample
 *
 *  Created by Oriol Ferrer Mesià on 28/01/11.
 *  Copyright 2011 uri.cat. All rights reserved.
 *
 */

#ifndef OFX_HISTORY_PLOT_DEFINED
#define OFX_HISTORY_PLOT_DEFINED

#include "ofMain.h"
#include <limits>

#define DEFAULT_WIDTH	160.0f
#define DEFAULT_HEIGHT	120.0f

class ofxHistoryPlot{

	public :

	ofxHistoryPlot(float * val, std::string varName, float maxHistory, bool autoUpdate_ = true);

	void update( float val = numeric_limits<float>::quiet_NaN() );	//adds in the plot current value of the specified var,
	//usually you would call this once per frame if not autoUpdate_ == false for the graph to update

	void draw(float x, float y , float w = DEFAULT_WIDTH, float h = DEFAULT_HEIGHT);	//draws a box
	void draw(float x, float y );	//draws a box
	void draw(int x, int y );

	void reset(); //clears out history

	void setRange(float low, float high);	//plot range is manually set
	void setLowerRange(float low);	//low is fixed, high is auto
	void setRangeAuto(); //low and high range of the plot is auto-calculated
	float getLowerRange();
	float getHigerRange();
	void setAutoRangeShrinksBack(bool shrink); //is the range allowed to shrink?

	void addHorizontalGuide(float yval, ofColor c);
	void clearHorizontalGuides();
	void setDrawGuideValues(bool doIt);

	void setMaxHistory(int max); //how many samples to keep at max (older samples deleted if more are added)
	void setPrecision(int prec);//number of decimals to show

	void setColor(const ofColor & c);
	ofColor getColor();
	void setBackgroundColor(const ofColor & c);

	void setDrawGrid(bool d);
	void setGridColor(ofColor c);
	void setGridUnit(float g); //pixels

	void setShowNumericalInfo(bool show);
	void setRespectBorders(bool respect);
	void setDrawSkipVal(int skip); //draw evey n samples, might speed up drawing
	void setLineWidth(float w);
	void setDrawBackground(bool d);
	void setDrawTitle(bool doit);

	void setDrawFromRight(bool val); //begin drawing graph from right instead of left

	float getLowestValue();
	float getHighestValue();

	void setShowSmoothedCurve(bool show);
	void setSmoothFilter(float filter);

	void setCropToRect(bool s);//use glScissor to avoid drawing out of rect bounds

	std::deque<float>& getValues();

	std::string getVariableName();
	void setVariableName(const std::string & var);

	float getHeight(){ return DEFAULT_HEIGHT;}
	float getWidth(){ return DEFAULT_WIDTH;}

private:

	enum RangeMode{
		RANGE_MANUAL,
		RANGE_LOWER_FIXED,
		RANGE_AUTOMATIC
	};

	void refillGridMesh(float x, float y , float w, float h);
	void refillPlotMesh(ofVboMesh& mesh, deque<float> & vals, float x, float y , float w, float h);

	std::string			varName;
	float *				valf;
	
	std::deque<float>	values;
	std::deque<float>	smoothValues;

	std::vector<float>	horizontalGuides;
	bool					drawGuideValues;
	std::vector<ofColor>	horizontalGuideColors;

	float		lowest, highest; //auto-updated range of all the current data values
	float		manualLowest, manualHighest;

	RangeMode	rangeMode;

	bool			drawBackground;

	bool			autoUpdate;
	bool			showNumericalInfo;
	bool			respectBorders;
	bool			drawGrid;
	bool			shrinkBackInAutoRange;
	bool			drawFromRight; //begin drawing graph from right instead of left
	bool			drawTitle;
	bool			scissor;

	int			MAX_HISTORY;

	int			precision;
	
	unsigned int	count;
	
	ofColor		lineColor;
	ofColor		bgColor;
	ofColor		gridColor;

	int			drawSkip;
	float		lineWidth;
	float		gridUnit;

	bool			showSmoothedPlot;
	float		smoothValue; //average of the last plotted vals
	float		smoothFactor; //(0 1.0] >> 1.0 means no smoothing

	bool 		plotNeedsRefresh;

	ofVboMesh 	gridMesh;
	ofVboMesh 	plotMesh;
	ofVboMesh 	smoothPlotMesh;

	ofRectangle prevRect;

	void recalcLowHigh();
};

#endif
