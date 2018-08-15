#pragma once

#include "ofMain.h"
#include "ofxFaceTracker2.h"
#include "ofxBiquadFilter.h"
#include "ofxAssimpModelLoader.h"
#include "ofxGui.h"


typedef dlib::matrix<double,40,1> sample_type;
typedef dlib::radial_basis_kernel<sample_type> kernel_type;

typedef dlib::decision_function<kernel_type> dec_funct_type;
typedef dlib::normalized_function<dec_funct_type> funct_type;

typedef dlib::probabilistic_decision_function<kernel_type> probabilistic_funct_type;
typedef dlib::normalized_function<probabilistic_funct_type> pfunct_type;


class ofApp : public ofBaseApp {
public:
    void setup();
    void update();
    void draw();
    void switchState();
    void switchModel(int currentState);
    
    void addModelToFace();
    
    
    sample_type makeSample();
    
    ofxFaceTracker2 tracker;
    ofVideoGrabber grabber;
    
    ofxBiquadFilter1f neutralValue;
    ofxBiquadFilter1f smallSmileValue;
    ofxBiquadFilter1f bigSmileValue;
    ofxBiquadFilter1f oValue;
    
    vector<pfunct_type> learned_functions;
    
    ofxAssimpModelLoader mdl;
    ofxAssimpModelLoader mdl1;
    ofxAssimpModelLoader mdl2;
    ofxAssimpModelLoader mdl3;
    ofxAssimpModelLoader mdl4;
    
    ofLight    light;
    
    // stores the info on the current file.
    string curFileInfo;
    
    ofEasyCam cam;
    
    //GUI setup
    ofxPanel gui;
    ofxFloatSlider lx, ly, lz;
    ofxFloatSlider rx, ry, rz;
    ofxFloatSlider px, py, pz;
    
    bool changeState = false;
    
    
    enum State{
        null,
        NEUTRAL,
        SMALLSMILE,
        BIGSMILE,
        OMOUTH
    };
    
    int m_currState;
    
    State lastState = null;
    
    State currentState = NEUTRAL;
    
};
