#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    
    learned_functions = vector<pfunct_type>(4);

    // Load SVM data model
    dlib::deserialize(ofToDataPath("data_ecstatic_smile.func")) >> learned_functions[0];
    dlib::deserialize(ofToDataPath("data_small_smile.func")) >> learned_functions[1];
    dlib::deserialize(ofToDataPath("data_o.func")) >> learned_functions[2];
    dlib::deserialize(ofToDataPath("data_neutral.func")) >> learned_functions[3];

    // Setup value filters for the classifer
    neutralValue.setFc(0.04);
    bigSmileValue.setFc(0.04);
    smallSmileValue.setFc(0.04);
    oValue.setFc(0.04);

    // Setup grabber
    grabber.setup(1280,720);
    

    // Setup tracker
    tracker.setup();
    
    
    // Setup GUI
    gui.setup();
    gui.add(lx.setup("light x", 0, 0, 5000));
    gui.add(ly.setup("light y", 1000, 0, 5000));
    gui.add(lz.setup("light z", 0, 0, 5000));
    
    gui.add(rx.setup("model rotate x", 360, 0, 360));
    gui.add(ry.setup("model rotate y", 0, 0, 360));
    gui.add(rz.setup("model rotate z", 360, 0, 360));
    
    gui.add(px.setup("model trans x", 0, 0, ofGetWidth()));
    gui.add(py.setup("model trans y", 100, -ofGetHeight(), ofGetHeight()));
    gui.add(pz.setup("model trabs z", 70, -1000, 1000));
    
    
    // Setup 3D models
    ofBackground(255, 255, 255);
    
    ofSetVerticalSync(true);
    
//    mdl.setRotation(0, 180, 1, 0, 0);
//    mdl.setScale(0.9, 0.9, 0.9);
//    mdl.setPosition(ofGetWidth()/2, ofGetHeight()/2, 0);
    
    mdl1.loadModel("VG18_3.obj", 20);
    mdl1.setPosition(0, 0, 0);
    mdl1.setScale(1, 1, 1);

    mdl2.loadModel("VG18_1", 20);
    mdl2.setPosition(0, 0, 0);
    mdl2.setScale(1, 1, 1);
    
    mdl3.loadModel("VG18_9.obj", 20);
    mdl3.setPosition(0, 0, 0);
    mdl3.setScale(1, 1, 1);

    mdl4.loadModel("VG18_7.obj");
    mdl4.setPosition(0, 0, 0);
    mdl4.setScale(1, 1, 1);
    
    mdl5.loadModel("grass.obj", 20);
    mdl5.setPosition(0, 0, 0);
    mdl5.setRotation(0, 180, 1, 0, 0);
    mdl5.setScale(.3, .3, .3);
    
    curFileInfo = ".obj";
    
        
    light.setPosition(lx, ly, lz);
    cam.setDistance(500);
}

//--------------------------------------------------------------
void ofApp::update(){
    
    grabber.update();
    
    if(grabber.isFrameNew()){
        tracker.update(grabber);
        
        if(tracker.size() > 0){
            // Run the classifiers and update the filters
            bigSmileValue.update(learned_functions[0](makeSample()));
            smallSmileValue.update(learned_functions[1](makeSample()));
            oValue.update(learned_functions[2](makeSample()));
            neutralValue.update(learned_functions[3](makeSample()));
        }
    }
    
    for (int x = 0; x < grabber.getWidth(); x++)
    {
        for (int y = 0; y < grabber.getHeight(); y ++)
        {
            grabber.getPixels().setColor(x, y, grabber.getPixels().getColor(x, y));
        }
    }
    texture.loadData(grabber.getPixels());
    
    switchState();
}

//--------------------------------------------------------------
void ofApp::draw(){
    
    texture.draw(0,0);
    
//    grabber.draw(0, 0);
//    tracker.drawDebug();
    
#ifndef __OPTIMIZE__
    ofSetColor(ofColor::red);
    ofDrawBitmapString("Warning! Run this app in release mode to get proper performance!",10,60);
    ofSetColor(ofColor::white);
#endif
    
    ofPushMatrix();
    ofTranslate(ofGetWidth() - 350, 100);
    
    for (int i = 0; i < 4; i++) {
        ofSetColor(255);

        string str;
        float val;
        switch (i) {
            case 0:
                str = "BIG SMILE";
                val = bigSmileValue.value();
                break;
            case 1:
                str = "SMALL SMILE";
                val = smallSmileValue.value();
                break;
            case 2:
                str = "OOO MOUTH";
                val = oValue.value();
                break;
            case 3:
                str = "NEUTRAL MOUTH";
                val = neutralValue.value();
                break;
        }

        ofDrawBitmapStringHighlight(str, 20, 0);
        ofDrawBitmapStringHighlight(ofToString(val), 20, 15);
        ofDrawRectangle(20, 20, 300 * val, 30);

        ofNoFill();
        ofDrawRectangle(20, 20, 300, 30);
        ofFill();

        ofTranslate(0, 70);
    }

    ofPopMatrix();
    
    
    // ---------GUI draw------------
    gui.draw();
    
    
    // ------3D models draw---------
    ofEnableDepthTest();
//    cam.begin();
    
        ofEnableLighting();
        light.enable();
        light.setPosition(lx, ly, lz);
    
        // draws all the other file types which are loaded into model.
//        mdl.setPosition(px, py, pz);
//        mdl.setRotation(0, 180, rx, ry, rz);
//        mdl.drawFaces();
    
        ofPushMatrix();
    
        addModelToFace();

        ofPopMatrix();
    
        light.disable();
        ofDisableLighting();
    
//    cam.end();
    ofDisableDepthTest();
    
}


void ofApp::addModelToFace()
{
    ofPushStyle();
    
    tracker.drawDebugPose();
    
    for(auto face : tracker.getInstances())
    {
        // Apply the pose matrix
        ofPushView();
        face.loadPoseMatrix();
        
        // Now position 0,0,0 is at the forehead
//        ofSetColor(255,0,0,50);
//        ofDrawRectangle(0, 0, 200, 200);
//
//        ofPushMatrix();
//        ofSetColor(0,255,0,50);
//        ofRotate(-90, 1, 0, 0);
//        ofDrawRectangle(0, 0, 200, 200);
//        ofPopMatrix();
//
//        ofPushMatrix();
//        ofSetColor(0,0,255,50);
//        ofRotate(90, 0, 1, 0);
//        ofDrawRectangle(0, 0, 200, 200);
//        ofPopMatrix();
        
        //// py = 100 is about hairline, pz = 70 looks actually on head
        mdl.setPosition(px, py, pz);
        mdl.setRotation(0, 180, rx, ry, rz);
        mdl.drawFaces();
        
        
        ofPopView();
    }
    
    ofPopStyle();

    ofDrawBitmapStringHighlight("Tracker fps: " + ofToString(tracker.getThreadFps()), 10, ofGetHeight() - 40);
    
}


void ofApp::switchState()
{
    if (changeState == true)
    {
        switchModel(currentState);
        changeState = false;
    }
    
    if (neutralValue.value() > 0.5)
    {
        currentState = NEUTRAL;
        
        if(lastState != NEUTRAL)
        {
            changeState = true;
            lastState = NEUTRAL;
        }
    }
    
    if (smallSmileValue.value() > 0.5)
    {
        currentState = SMALLSMILE;
        
        if(lastState != SMALLSMILE)
        {
            changeState = true;
            lastState = SMALLSMILE;
        }
    }
    
    if (bigSmileValue.value() > 0.5)
    {
        currentState = BIGSMILE;
        
        if(lastState != BIGSMILE)
        {
            changeState = true;
            lastState = BIGSMILE;
        }
    }
    
    if (oValue.value() > 0.5)
    {
        currentState = OMOUTH;
        
        if(lastState != OMOUTH)
        {
            changeState = true;
            lastState = OMOUTH;
        }
    }
    
    
    cout << "current state is : " << currentState << endl;
}


void ofApp::switchModel(int currentState)
{
    
    switch(currentState)
    {
        case null:
            break;
            
        case NEUTRAL:
            mdl = mdl1;
            break;
            
        case SMALLSMILE:
            mdl = mdl2;
            break;
            
        case BIGSMILE:
            mdl = mdl3;
            break;
            
        case OMOUTH:
            mdl = mdl4;
            break;
    }
}


// Function that creates a sample for the classifier containing the mouth and eyes
sample_type ofApp::makeSample(){
    auto outer = tracker.getInstances()[0].getLandmarks().getImageFeature(ofxFaceTracker2Landmarks::OUTER_MOUTH);
    auto inner = tracker.getInstances()[0].getLandmarks().getImageFeature(ofxFaceTracker2Landmarks::INNER_MOUTH);
    
    auto lEye = tracker.getInstances()[0].getLandmarks().getImageFeature(ofxFaceTracker2Landmarks::LEFT_EYE);
    auto rEye = tracker.getInstances()[0].getLandmarks().getImageFeature(ofxFaceTracker2Landmarks::RIGHT_EYE);
    
    ofVec2f vec = rEye.getCentroid2D() - lEye.getCentroid2D();
    float rot = vec.angle(ofVec2f(1,0));
    
    vector<ofVec2f> relativeMouthPoints;
    
    ofVec2f centroid = outer.getCentroid2D();
    for(ofVec2f p : outer.getVertices()){
        p -= centroid;
        p.rotate(rot);
        p /= vec.length();
        
        relativeMouthPoints.push_back(p);
    }
    
    for(ofVec2f p : inner.getVertices()){
        p -= centroid;
        p.rotate(rot);
        p /= vec.length();
        
        relativeMouthPoints.push_back(p);
    }
    
    sample_type s;
    for(int i=0;i<20;i++){
        s(i*2+0) = relativeMouthPoints[i].x;
        s(i*2+1) = relativeMouthPoints[i].y;
    }
    return s;
}
