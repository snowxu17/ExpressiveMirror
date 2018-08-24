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
    
    void setUpModels();
    void switchState();
    void switchModel(int currentState);
    void addModelToFace();
    
    void setupStrings();
    void switchString(int currentState);
    void drawStrings();
    
    void setupParticles();
    void updateParticles();
    
    sample_type makeSample();
    
    ofxFaceTracker2 tracker;
    std::vector<ofxFaceTracker2Instance> instances;
    
    // Video grabber & video texture
    ofVideoGrabber grabber;    
    ofTexture texture;
    
    // Image setup
    int imgSize = 90;
    ofImage img; // Background texture

    ofImage n;
    ofImage ss;
    ofImage bs;
    ofImage o;
    std::vector<ofImage> emojis;
    
    // Expression values
    ofxBiquadFilter1f neutralValue;
    ofxBiquadFilter1f smallSmileValue;
    ofxBiquadFilter1f bigSmileValue;
    ofxBiquadFilter1f oValue;
    
    vector<pfunct_type> learned_functions;
    
    // Kalman soothing filter
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
    ofxAssimpModelLoader mdl13;
    ofxAssimpModelLoader mdl14;
    ofxAssimpModelLoader mdl15;
    ofxAssimpModelLoader mdl16;
    ofxAssimpModelLoader mdl17;
    ofxAssimpModelLoader mdl18;
    ofxAssimpModelLoader mdl19;
    ofxAssimpModelLoader mdl20;
    
    std::vector<ofxAssimpModelLoader> n_mdls;
    std::vector<ofxAssimpModelLoader> ss_mdls;
    std::vector<ofxAssimpModelLoader> bs_mdls;
    std::vector<ofxAssimpModelLoader> o_mdls;
    
    // Conversation strings
    string str;
    std::vector<string> n_strings;
    std::vector<string> ss_strings;
    std::vector<string> bs_strings;
    std::vector<string> o_strings;

    // Lighting & camera
    ofLight light;
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
    
    float threshold = 1000.;
    float timeCounter = 0;
    
    bool isTracking = true;
    
    // Grabber setup
    int grabber_w = 1920;
    int grabber_h = 1080;
    
    int w = ofGetWidth();
    int h = ofGetHeight();
    
    float x_offset = grabber_w/3;
    
    // Font
    ofTrueTypeFont font;
    ofTrueTypeFont font2;
    ofTrueTypeFont font3;
    
};
