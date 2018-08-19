#pragma once

#include "ofMain.h"
#include "ofxFaceTracker2.h"
#include "ofxBiquadFilter.h"
#include "ofxAssimpModelLoader.h"
#include "ofxGui.h"
#include "ofxCv.h"
#include "Particle.h"


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
    
    void setupStrings();
    void updateStrings();
    
    void setupParticles();
    void updateParticles();
    
    
    sample_type makeSample();
    
    ofxFaceTracker2 tracker;
    ofVideoGrabber grabber;    
    ofTexture texture;
    ofPixels pixel;
    ofImage img;
    ofImage bang;
    
    ofxBiquadFilter1f neutralValue;
    ofxBiquadFilter1f smallSmileValue;
    ofxBiquadFilter1f bigSmileValue;
    ofxBiquadFilter1f oValue;
    
    vector<pfunct_type> learned_functions;
    
    ofxCv::KalmanPosition kalman;
    
    
    
    // 3D model setup
    ofxAssimpModelLoader mdl;
    ofxAssimpModelLoader mdl1;
    ofxAssimpModelLoader mdl2;
    ofxAssimpModelLoader mdl3;
    ofxAssimpModelLoader mdl4;
    ofxAssimpModelLoader mdl5;
    ofxAssimpModelLoader mdl6;
    ofxAssimpModelLoader mdl7;
    ofxAssimpModelLoader mdl8;
    ofxAssimpModelLoader mdl9;
    ofxAssimpModelLoader mdl10;
    ofxAssimpModelLoader mdl11;
    ofxAssimpModelLoader mdl12;
    
    std::vector<ofxAssimpModelLoader> n_mdls;
    std::vector<ofxAssimpModelLoader> ss_mdls;
    std::vector<ofxAssimpModelLoader> bs_mdls;
    std::vector<ofxAssimpModelLoader> o_mdls;
    
    std::vector<string> n_strings;
    std::vector<string> ss_strings;
    std::vector<string> bs_strings;
    std::vector<string> o_strings;
    
    ofLight    light;
    
    // stores the info on the current file.
    string curFileInfo;
    
    ofEasyCam cam;
    
    // Particle setup
    std::vector<Particle> particles;
    glm::vec3 boundingSize;
    
    
    //GUI setup
    ofxPanel gui;
    ofxFloatSlider lx, ly, lz;
    ofxFloatSlider rx, ry, rz;
    ofxFloatSlider px, py, pz;
    ofxFloatSlider size;
    
    
    // State setup
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
    
    // Grabber setup
    int grabber_w = 1280;
    int grabber_h = 720;
    
    // Font
    ofTrueTypeFont font;
    
    
    float p_scl = 0;
    float n_scl;
    

    
};
