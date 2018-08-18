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
    grabber.setup(grabber_w,grabber_h);

    // Setup tracker
    tracker.setup();
    
    // Setup GUI
    gui.setup();
    gui.add(lx.setup("light x", 1200, 0, 5000));
    gui.add(ly.setup("light y", 200, 0, 5000));
    gui.add(lz.setup("light z", 1000, 0, 5000));
    
//    gui.add(rx.setup("model rotate x", 0, 0, 360)); //360 when facepose is used
//    gui.add(ry.setup("model rotate y", 0, 0, 360));
//    gui.add(rz.setup("model rotate z", 0, 0, 360));
//
//    gui.add(px.setup("model trans x", 0, -ofGetWidth(), ofGetWidth()));
//    gui.add(py.setup("model trans y", 500, -ofGetHeight(), ofGetHeight()));
//    gui.add(pz.setup("model trabs z", 350, -1000, 1000));
    
//    gui.add(size.setup("size", 0, 100, 1000));
    
    // Setup 3D models
    ofBackground(50);
    ofSetVerticalSync(true);
    
    mdl1.loadModel("Bird_LP.dae", 20);
    mdl1.setPosition(0, -200, 0);
    mdl1.setScale(.6, .6, .6);
    mdl1.setRotation(0, 70, 1, 1, 0);
    n_mdls.push_back(mdl1);

    mdl2.loadModel("Shoe_LP.dae", 20);
    mdl2.setPosition(0, 0, 0);
    mdl2.setScale(1, 1, 1);
    mdl2.setRotation(0, 180, 0, 1, 0);
    ss_mdls.push_back(mdl2);
    
    mdl3.loadModel("BowlingBall.dae", 20);
    mdl3.setPosition(0, 0, 0);
    mdl3.setRotation(0, 50, 1, -1, 0);
    mdl3.setScale(1, 1, 1);
    n_mdls.push_back(mdl3);

    mdl4.loadModel("Coffee.dae");
    mdl4.setPosition(0, 0, 0);
    mdl4.setScale(.5, .5, .5);
    mdl4.setRotation(0, 30, 1, 0, 0);
    ss_mdls.push_back(mdl4);
    
    mdl5.loadModel("Tooth_w.dae");
    mdl5.setPosition(0, 0, 0);
    mdl5.setRotation(0, 180, 0, 0, 0);
    mdl5.setScale(1, 1, 1);
    o_mdls.push_back(mdl5);
    
    mdl6.loadModel("Teeth.dae");
    mdl6.setPosition(0, 0, 0);
    mdl6.setRotation(0, 180, 0, 0, 0);
    mdl6.setScale(.8, .8, .8);
    bs_mdls.push_back(mdl6);
    
    mdl7.loadModel("Clam.dae");
    mdl7.setPosition(0, 0, 0);
    mdl7.setRotation(0, 60, 1, 0, 0);
    mdl7.setScale(.8, .8, .8);
    bs_mdls.push_back(mdl7);
    
    mdl8.loadModel("UFO.dae");
    mdl8.setPosition(0, 0, 0);
    mdl8.setRotation(0, 180, 0, 1, 0);
    mdl8.setRotation(0, 20, 1, 0, 0);
    mdl8.setScale(.8, .8, .8);
    o_mdls.push_back(mdl8);
    
    
    
//    curFileInfo = ".dae";
    
    light.setPosition(lx, ly, lz);
    cam.setDistance(500);
    
    cout << "n_mdls count: " << n_mdls.size() << endl;
    cout << "ss_mdls count: " << ss_mdls.size() << endl;
    cout << "bs_mdls count: " << bs_mdls.size() << endl;
    cout << "o_mdls count: " << o_mdls.size() << endl;
    
    // Load image
    img.load("grid.jpg");
    
    // Load Font
    font.load("BABYK___.TTF", 80);
    
    // Set up Kalman
    kalman.init(1/10000., 1/10.);
    
    // Particle setup
    setupParticles();

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
    
    updateParticles();
    
    switchState();
}

//--------------------------------------------------------------
void ofApp::draw(){
    
    // Draw background
    ofSetColor(255);
    img.draw(0, 0, ofGetWidth(), ofGetHeight());
    
//    grabber.draw(0, 0);
//    texture.draw(0,0);
//    grabber.getTexture().drawSubsection(grabber_w/3, 0, 720, 720, grabber_w/3, 0);
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
    
    
    // --------Particles draw-----------
    ofEnableLighting();
    light.enable();
    for (Particle& p: particles)
    {
        p.draw();
    }
    light.disable();
    ofDisableLighting();
    
    
    // ------3D models draw---------
    ofEnableDepthTest();
    
        ofEnableLighting();
        light.enable();
        light.setPosition(lx, ly, lz);
        light.setAmbientColor(ofColor::pink);
    
        ofPushMatrix();
        addModelToFace();
        ofPopMatrix();
    
        light.disable();
        ofDisableLighting();
    
    ofDisableDepthTest();
    
    
    // -----font draw--------------
    ofPushStyle();
    ofSetColor(ofColor::black);
    ofPushMatrix();
    
    ofTranslate(ofGetWidth()/5, ofGetHeight() * 0.9);
    
    float time = ofGetElapsedTimef();
    float scale = 0.5 + ofNoise(time/2)/4;
    
    float x = sin(time / 2) * 50;
    float y = ofNoise(time / 2) * 50;
    
    ofTranslate(x, y);
    ofScale(scale, scale);
    
    font.drawString("Meet your imaginary friend", 10, 10);
    
    ofSetColor(ofColor::red);
    font.drawString("Meet your imaginary friend", 0, 0);
    
    ofPopMatrix();
    ofPopStyle();
}


void ofApp::addModelToFace()
{
    ofPushStyle();
    
    tracker.drawDebugPose();
    
    for(auto face : tracker.getInstances())
    {
        // Boundinhg Box
        glm::vec3 pBoundingBox = face.getBoundingBox().getPosition();
        float w_b = face.getBoundingBox().getWidth();
        
        kalman.update(pBoundingBox);
        glm::vec3 s_position = kalman.getEstimation();
//        cout << s_position << endl;
//        cout << "Box width: " << w_b << endl;
        
        // Matrix
//        ofMatrix4x4 matrix = face.getPoseMatrix();
//        glm::vec3 m_p = matrix.getTranslation();
//        glm::vec3 m_r = matrix.getRotate().asVec3();
//        cout << "Current matrix: " << endl;
//        cout << matrix << endl;
//        cout << "Rotation: " << endl;
//        cout << m_r << endl;
//        cout << "Translation: " << endl;
//        cout << m_p << endl;
        
        
        // Smooth function
//        float xSmoothCorrection = 0.80;
//        float ySmoothCorrection = 0.80;
//        float zSmoothCorrection = 0.80;
//        float s_x = xSmoothCorrection * s_x + ( 1 - xSmoothCorrection) * pBoundingBox.x;
//        float s_y = ySmoothCorrection * s_y + ( 1 - ySmoothCorrection) * pBoundingBox.y;
//        float s_z = zSmoothCorrection * s_z + ( 1 - zSmoothCorrection) * pBoundingBox.z;
        
        ofPushView();
//        face.loadPoseMatrix();
        ofPushMatrix();

        
        // Draw with kalman-smoothed bounding box
        float scl = w_b/ 290;
        
        ofTranslate(s_position.x + 120 * scl,
                    s_position.y - 70 * scl,
                    s_position.z - 100 * scl);
        
        float time = ofGetElapsedTimef();
        float speed = 2;
        float x = sin(time * speed) * 100;
        float y = ofNoise(time) * 150;
        
        float r_x = 0;
        float r_y = sin(time*speed) * 200;
        float r_z = 0;
        
        ofTranslate(x,y);
        ofScale(scl, scl, scl);
        
//        mdl.setRotation(0, 180, 0, r_y, 0);
        
        mdl.drawFaces();
        
        // Draw based on matrix position // broken
//        ofPushMatrix();
//        ofTranslate(200, 300);
//        ofSetColor(255, 0, 0);
//        ofRectangle(m_p.x, m_p.y, 100, 100);
//        ofRectangle(100, 100, 100, 100);
//        ofDrawBox(m_p.x, m_p.y, m_p.z, size);
//        ofPopMatrix();
        
        ofPopMatrix();
        
        ofPopView();
    }
    
    ofPopStyle();

//    ofDrawBitmapStringHighlight("Tracker fps: " + ofToString(tracker.getThreadFps()), 10, ofGetHeight() - 40);
}


void ofApp::switchModel(int currentState)
{
    switch(currentState)
    {
        case null:
            break;
            
        case NEUTRAL:
            mdl = n_mdls[ofRandom(n_mdls.size())];
            break;
            
        case SMALLSMILE:
            mdl = ss_mdls[ofRandom(ss_mdls.size())];
            break;
            
        case BIGSMILE:
            mdl = bs_mdls[ofRandom(bs_mdls.size())];
            break;
            
        case OMOUTH:
            mdl = o_mdls[ofRandom(o_mdls.size())];
            break;
    }
}


void ofApp::switchState()
{
    float timeCounter = 0;
    
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
            timeCounter += ofGetElapsedTimef();
            if(timeCounter > 5.0f)
            {
                changeState = true;
                lastState = NEUTRAL;
                timeCounter = 0;
            }
        }
    }
    
    if (smallSmileValue.value() > 0.4)
    {
        currentState = SMALLSMILE;
        
        if(lastState != SMALLSMILE)
        {
            timeCounter += ofGetElapsedTimef();
            if(timeCounter > 5.0f)
            {
                changeState = true;
                lastState = SMALLSMILE;
                timeCounter = 0;
            }
        }
    }
    
    if (bigSmileValue.value() > 0.5)
    {
        currentState = BIGSMILE;
        
        if(lastState != BIGSMILE)
        {
            timeCounter += ofGetElapsedTimef();
            if(timeCounter > 5.0f)
            {
                changeState = true;
                lastState = BIGSMILE;
                timeCounter = 0;
            }
        }
    }
    
    if (oValue.value() > 0.5)
    {
        currentState = OMOUTH;
        
        if(lastState != OMOUTH)
        {
            timeCounter += ofGetElapsedTimef();
            if(timeCounter > 5.0f)
            {
                changeState = true;
                lastState = OMOUTH;
                timeCounter = 0;
            }
        }
    }
    
    if(neutralValue.value() < 0.5 && smallSmileValue.value() < 0.4 && bigSmileValue.value() < 0.5 && oValue.value() < 0.5)
    {
        currentState = NEUTRAL;
    }
//    cout << "current state is : " << currentState << endl;
}

void ofApp::setupStrings()
{
    n_strings.push_back("hi");
    
    ss_strings.push_back("hehe");
    
    bs_strings.push_back("haha");
    
    o_strings.push_back("oooo");
}


void ofApp::setupParticles()
{
    
    // The width, height and depth of our bounding cube.
    boundingSize = { 3000, 2000, 400 };
    
    int numParticles = 150;
    
    for (int i = 0; i < numParticles; i++)
    {
        Particle p;
        
        // Set initial positions.
        p.position.x = ofGetWidth()/2;
        p.position.y = ofGetHeight()/2;
        p.position.z = 0;
        
        // Set initial velocities.
        p.velocity.x = ofRandom(-5, 5);
        p.velocity.y = ofRandom(-5, 5);
        p.velocity.z = ofRandom(-5, 5);
        
        // Set inital accelerations.
        p.acceleration.x = 0;
        p.acceleration.y = 0;
        p.acceleration.z = 0;
        
        // Set angular velocity.
        p.angularVelocity.x = ofRandom(-2, 2);
        p.angularVelocity.y = ofRandom(-2, 2);
        p.angularVelocity.z = ofRandom(-2, 2);
        
        // Set radius.
        p.radius = ofRandom(10, 50);
        
        // Set color.
        p.color = ofColor::fromHsb(ofRandom(255), 255, 255);
        
        // Add a copy to our vector.
        particles.push_back(p);
    }
    
}


void ofApp::updateParticles()
{
    for (Particle& p: particles)
    {
        p.update();

        // Check to see if our position in inside our bounding cube.
        // Check x
        if (p.position.x + p.radius >  boundingSize.x / 2)
        {
            p.velocity.x *= -1;
            p.position.x = boundingSize.x / 2 - p.radius;
        }
        else if (p.position.x - p.radius < -boundingSize.x / 2)
        {
            p.velocity.x *= -1;
            p.position.x = - boundingSize.x / 2 + p.radius;
        }
        
        // Check Y
        if (p.position.y + p.radius >  boundingSize.y / 2)
        {
            p.velocity.y *= -1;
            p.position.y = boundingSize.y / 2 - p.radius;
        }
        else if (p.position.y - p.radius < -boundingSize.y / 2)
        {
            p.velocity.y *= -1;
            p.position.y = - boundingSize.y / 2 + p.radius;
        }
        
        // Check z
        if (p.position.z + p.radius >  boundingSize.z / 2)
        {
            p.velocity.z *= -1;
            p.position.z = boundingSize.z / 2 - p.radius;
        }
        else if (p.position.z - p.radius < -boundingSize.z / 2)
        {
            p.velocity.z *= -1;
            p.position.z = - boundingSize.z / 2 + p.radius;
        }
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
