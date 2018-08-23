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
//    grabber.listDevices();
//    grabber.setDeviceID(1);
    grabber.setup(grabber_w,grabber_h);

    // Setup tracker
    tracker.setup();
    
    // Setup GUI
    gui.setup();
    gui.add(lx.setup("light x", 1200, 0, 5000));
    gui.add(ly.setup("light y", 200, 0, 5000));
    gui.add(lz.setup("light z", 1000, 0, 5000));
    
    gui.add(px.setup("model trans x", 0, -ofGetWidth(), ofGetWidth()));
    gui.add(py.setup("model trans y", 500, -ofGetHeight(), ofGetHeight()));
    gui.add(pz.setup("model trabs z", 350, -1000, 1000));

    
    // Setup 3D models
    setUpModels();
    
    // Load image
    img.load("grid.jpg");
    bubble.load("Bubble.png");
    bubble.setImageType(OF_IMAGE_COLOR_ALPHA);    
    
    bs.load("bs.png");
    ss.load("ss.png");
    o.load("o.png");
    n.load("n.png");
    
    emojis.push_back(bs);
    emojis.push_back(ss);
    emojis.push_back(o);
    emojis.push_back(n);
    
    
    // Load Font
    font.load("BABYK___.TTF", 140);
    font2.load("Sketch 3D.otf", 100);
    font3.load("Sketch 3D.otf", 25);
    
    // Set up Kalman
    kalman.init(1/10000., 1/10.);
    
    // Particle setup
    setupParticles();
    
    // Strings setup
    setupStrings();
    
    ofBackground(0);
    
}


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
    
    cout << "instance size: " << instances.size() << endl;
    
    for(auto face : tracker.getInstances())
    {
        instances.push_back(face);
    }
    
}


void ofApp::draw(){
    
    // Draw background
    ofSetColor(255);
    ofPushMatrix();
    ofScale(3, 3);
    ofScale(img.getWidth()/w, (img.getHeight()/h)/3);
//    img.draw(0, 0, 1080, 1920);
    ofPopMatrix();
    
//    grabber.draw(0, 0);
//    texture.draw(0,0);
    
//    ofPushMatrix();
//    ofScale(2, 2, 1);
//    grabber.getTexture().drawSubsection(0, 0, 720, 1080, x_offset, 0);
//    ofPopMatrix();
    
//    tracker.drawDebug();
    
#ifndef __OPTIMIZE__
    ofSetColor(ofColor::red);
    ofDrawBitmapString("Warning! Run this app in release mode to get proper performance!",10,60);
    ofSetColor(ofColor::white);
#endif
    
    
    // Draw GUI
//    gui.draw();
    
    
    // Draw particles
    ofPushMatrix();
    ofScale(3, 3, 3);
    
    ofEnableLighting();
    light.enable();
    
    for (Particle& p: particles)
    {
        p.draw();
    }
    
    light.disable();
    ofDisableLighting();
    
    ofPopMatrix();
    
    
    // Draw 3D models
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
    
    
    // Draw font
    ofPushStyle();
    ofPushMatrix();
    
//    ofScale(2, 2);
    ofTranslate(ofGetWidth()/10, ofGetHeight() * 0.95);
    
    float time = ofGetElapsedTimef();
    float scale = 0.5 + ofNoise(time/2)/4;
    
    float x = sin(time / 2) * 50;
    float y = ofNoise(time / 2) * 50;
    
    ofTranslate(x, y);
    ofScale(scale, scale);

    ofSetColor(ofColor::purple);
    font.drawString("Meet your imaginary friend", 10, 10);
    
    ofSetColor(ofColor::red);
    font.drawString("Meet your imaginary friend", 0, 0);
    
    ofPopMatrix();
    ofPopStyle();
    
    
    // Draw value bars
    ofPushMatrix();
    ofTranslate(ofGetWidth() - 400, 100);
    
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
        
        //        ofDrawBitmapStringHighlight(str, 20, 0);
        //        ofDrawBitmapStringHighlight(ofToString(val), 20, 15);
        
        ofSetColor(ofColor::fromHsb((i+1) * 70, 255, 255));
        font3.drawString(str, 20, -30);
        font3.drawString(ofToString(val), 20, 10);
        
        ofSetColor(ofColor::fromHsb((i+1) * 70, 255, 255));
        ofDrawRectangle(20, 20, 350 * val, 80);
        
        ofSetColor(255);
        ofImageType(OF_IMAGE_COLOR_ALPHA);
        emojis[i].draw(350 * val - imgSize/2, 15, imgSize, imgSize);
        
        ofNoFill();
        ofSetColor(ofColor::yellow);
        ofDrawRectangle(20, 20, 350, 80);
        ofFill();
        
        ofTranslate(0, 200);
    }
    
    ofPopMatrix();
}


void ofApp::addModelToFace()
{
    ofPushStyle();
    ofPushMatrix();
    
    //Move the models down when scaled for full screen
    ofTranslate(-x_offset, ofGetHeight()/5);
    ofScale(2.5, 2.5);
    
//    ofTranslate(- x_offset, 0, 0);
//    ofScale(2, 2);
    
    // Draw 3D coordinate debug pose
//    tracker.drawDebugPose();
    
    
//    int numFaces = tracker.size();
//    cout << "numFaces: " << numFaces << endl;
//    cout << "isTracking : " << isTracking << endl;

//        for(auto face : tracker.getInstances())
//        {
//            glm::vec3 pBoundingBox = face.getBoundingBox().getPosition();
//            float w_b = face.getBoundingBox().getWidth();
//            //cout << w_b << endl;
//
//            kalman.update(pBoundingBox);
//            glm::vec3 s_position = kalman.getEstimation();
//
//            float s_w_b = 0.70 * s_w_b + 0.3 * w_b;
//            //cout<<"smoothed boundingbox width: " << s_w_b <<endl;
//
//            ofPushView();
//            ofPushMatrix();
//
//            // Draw with kalman-smoothed bounding box
//            float scl = s_w_b/ 69;
//
//            float px = grabber_w - (s_position.x + 120 * scl);
//            float py = s_position.y - 70 * scl;
//            float pz = s_position.z - 100 * scl;
//            ofTranslate(px, py, pz);
//
//            cout << "xpos: " << px << endl;
//            cout << "ypos: " << py << endl;
//
//            float time = ofGetElapsedTimef();
//            float speed = 2;
//            float x = sin(time * speed) * 100;
//            float y = ofNoise(time) * 150;
//
//            ofTranslate(x,y);
//
//            ofScale(1, 1, 1);
//            // ofScale(scl, scl, scl);
//
//            mdl.drawFaces();
//            drawStrings();
//
//            ofPopMatrix();
//            ofPopView();
//        }
    
    
    for (int i = 0; i < instances.size(); i ++)
    {
        if ( i == 0)
        {
            glm::vec3 pBoundingBox = instances[i].getBoundingBox().getPosition();
            float w_b = instances[i].getBoundingBox().getWidth();
            //cout << w_b << endl;

            kalman.update(pBoundingBox);
            glm::vec3 s_position = kalman.getEstimation();
//            cout << "sposition: " << s_position << endl;

            float s_w_b = 0.70 * s_w_b + 0.3 * w_b;
            //cout<<"smoothed boundingbox width: " << s_w_b <<endl;

            ofPushView();
            ofPushMatrix();

            // Draw with kalman-smoothed bounding box
            float scl = s_w_b/ 69;

//            float px = 500; //grabber_w - (s_position.x + 120 * scl);
//            float py = 500; //s_position.y - 70 * scl;
//            float pz = s_position.z - 100 * scl;
            ofTranslate(690, 500, 100);
//            ofTranslate(px,py,pz);

            cout << "xpos: " << grabber_w - (s_position.x + 120 * scl) << endl;
            cout << "ypos: " << s_position.y - 70 * scl << endl;

            float time = ofGetElapsedTimef();
            float speed = 2;
            float x = sin(time * speed) * 100;
            float y = ofNoise(time) * 150;

            ofTranslate(x,y);

            ofScale(1, 1, 1);
            //ofScale(scl, scl, scl);

            mdl.drawFaces();
            drawStrings();

            ofPopMatrix();
            ofPopView();
        }
    }
    
    ofPopMatrix();
    ofPopStyle();

//    ofDrawBitmapStringHighlight("Tracker fps: " + ofToString(tracker.getThreadFps()), 10, ofGetHeight() - 40);
}


void ofApp::drawStrings()
{
    ofPushMatrix();
    ofPushStyle();
    
    ofTranslate(-300, 300);
    ofScale(.5, .5);
    font2.drawString(str, 0, 0);
    
//    bubble.draw(0, 0, 500, 500);
    
    ofPopStyle();
    ofPopMatrix();
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


void ofApp::switchString(int currentState)
{
    switch(currentState)
    {
        case null:
            break;
            
        case NEUTRAL:
            str = n_strings[ofRandom(n_strings.size())];
            break;
            
        case SMALLSMILE:
            str = ss_strings[ofRandom(ss_strings.size())];
            break;
            
        case BIGSMILE:
            str = bs_strings[ofRandom(bs_strings.size())];
            break;
            
        case OMOUTH:
            str = o_strings[ofRandom(o_strings.size())];
            break;
    }
}


void ofApp::switchState()
{
    
    if (changeState == true)
    {
        switchModel(currentState);
        switchString(currentState);
        changeState = false;
    }
    
    if (neutralValue.value() > 0.5 && smallSmileValue.value() < 0.4 && bigSmileValue.value() < 0.5 && oValue.value() < 0.5)
    {
        currentState = NEUTRAL;
        timeCounter += ofGetElapsedTimef();
        
        if(lastState != NEUTRAL && timeCounter > threshold)
        {
            changeState = true;
            lastState = NEUTRAL;
            timeCounter = 0;
        }
    }
    
    if (neutralValue.value() < 0.5 && smallSmileValue.value() > 0.4 && bigSmileValue.value() < 0.5 && oValue.value() < 0.5)
    {
        currentState = SMALLSMILE;
        timeCounter += ofGetElapsedTimef();
        
        if(lastState != SMALLSMILE && timeCounter > threshold)
        {
            changeState = true;
            lastState = SMALLSMILE;
            timeCounter = 0;
        }
    }
    
    if (neutralValue.value() < 0.5 && smallSmileValue.value() < 0.4 && bigSmileValue.value() > 0.5 && oValue.value() < 0.5)
    {
        currentState = BIGSMILE;
        timeCounter += ofGetElapsedTimef();
        
        if(lastState != BIGSMILE && timeCounter > threshold)
        {
            changeState = true;
            lastState = BIGSMILE;
            timeCounter = 0;
        }
    }
    
    if (neutralValue.value() < 0.5 && smallSmileValue.value() < 0.4 && bigSmileValue.value() < 0.5 && oValue.value() > 0.5)
    {
        currentState = OMOUTH;
        timeCounter += ofGetElapsedTimef();
        
        if(lastState != OMOUTH && timeCounter > threshold)
        {
            changeState = true;
            lastState = OMOUTH;
            timeCounter = 0;
        }
    }
    
    if(neutralValue.value() < 0.5 && smallSmileValue.value() < 0.4 && bigSmileValue.value() < 0.5 && oValue.value() < 0.5)
    {
        currentState = NEUTRAL;
    }
//    cout << "current state is : " << currentState << endl;
}


void ofApp::setupParticles()
{
    // The width, height and depth of our bounding cube.
    boundingSize = { 1500, 2500, 400 };
    
    int numParticles = 200;
    
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


void ofApp::setupStrings()
{
    n_strings.push_back("Zzzzzzzzzzzzzzz");
    n_strings.push_back("What are you???");
    n_strings.push_back("Nap time!");
    n_strings.push_back("Play with me");
    n_strings.push_back("Smile! Smile!");
    n_strings.push_back("Are you real?");
    n_strings.push_back("Show me your teeth");
    n_strings.push_back("There goes my sanity");
    n_strings.push_back("Show me your moves");
    n_strings.push_back("Dance with me");
    n_strings.push_back("What are you?");
    n_strings.push_back("Lalalalalalalala la.");
    n_strings.push_back("ha ha ha...");
    n_strings.push_back("hello world");
    n_strings.push_back("Meow?");
    n_strings.push_back("Blow your cheeks!");
    n_strings.push_back("You are a good robot");
    n_strings.push_back("Did you wash your face?");
    
    
    ss_strings.push_back("Whatever.");
    ss_strings.push_back("Gimme your eyebrow");
    ss_strings.push_back("How are you?");
    ss_strings.push_back("Tell me something fun");
    ss_strings.push_back("Show me how to laugh");
    ss_strings.push_back("Show me your teeth");
    ss_strings.push_back("Let's talk with face");
    ss_strings.push_back("I love daydreaming");
    ss_strings.push_back("g'night");
    
    
    bs_strings.push_back("What's sanity?");
    bs_strings.push_back("Blah blah blah blah blah.");
    bs_strings.push_back("What did you say?");
    bs_strings.push_back("I like nightmares!");
    bs_strings.push_back("Look at you!");
    bs_strings.push_back("Cool");
    bs_strings.push_back("Shhhhhhhhhhh");
    bs_strings.push_back("yay...");
    bs_strings.push_back("Let's scream together");
    
    
    o_strings.push_back("What surprises you?");
    o_strings.push_back("What do you like?");
    o_strings.push_back("What intrigues you?");
    o_strings.push_back("You are weird");
    o_strings.push_back("AHHHHHHHHHHHHHH");
    o_strings.push_back("You are funny");
    o_strings.push_back("Blah blah.");
    o_strings.push_back("OH WOWW");
    o_strings.push_back("BAM BAM BAM");
    o_strings.push_back("Gimme your nose");
    o_strings.push_back("You are robot");
    o_strings.push_back("You need face massage");
    o_strings.push_back("Do you bring me booze?");
    o_strings.push_back("I need a party");
    
    
    cout << "n_strings size: " << n_strings.size() << endl;
    cout << "ss_strings size: " << ss_strings.size() << endl;
    cout << "bs_strings size: " << bs_strings.size() << endl;
    cout << "o_strings size: " << o_strings.size() << endl;
}


void ofApp::setUpModels()
{
    ofBackground(50);
    ofSetVerticalSync(true);
    
    mdl1.loadModel("Bird_LP.dae", 20);
    mdl1.setPosition(0, 0, 0);
    mdl1.setScale(1, 1, 1);
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
    mdl7.setScale(.7, .7, .7);
    bs_mdls.push_back(mdl7);
    
    mdl8.loadModel("UFO.dae");
    mdl8.setPosition(0, - 300, 0);
    mdl8.setRotation(0, 180, 0, 1, 0);
    mdl8.setRotation(0, 20, 1, 0, 0);
    mdl8.setScale(1, 1, 1);
    o_mdls.push_back(mdl8);
    
    mdl9.loadModel("Jellyfish.dae");
    mdl9.setPosition(0, 0, 0);
    mdl9.setRotation(0, 180, 0, 1, 0);
    mdl9.setRotation(0, 20, 1, 0, 0);
    mdl9.setScale(.8, .8, .8);
    n_mdls.push_back(mdl9);
    
    mdl10.loadModel("Hammer.dae");
    mdl10.setPosition(0, 0, 0);
    mdl10.setRotation(0, 100, 0, 1, 0);
    mdl10.setRotation(0, 20, 1, 0, 0);
    mdl10.setScale(1, 1, 1);
    o_mdls.push_back(mdl10);
    
    mdl11.loadModel("Donut.dae");
    mdl11.setPosition(0, 0, 0);
//    mdl11.setRotation(0, 100, 0, 1, 0);
//    mdl11.setRotation(0, 20, 1, 0, 0);
    mdl11.setScale(1, 1, 1);
    bs_mdls.push_back(mdl11);
    
    mdl12.loadModel("Brain.dae");
    mdl12.setPosition(0, 200, 0);
    mdl12.setRotation(0, 100, 0, 1, 0);
//    mdl12.setRotation(0, 20, 1, 0, 0);
    mdl12.setScale(1, 1, 1);
    n_mdls.push_back(mdl12);
    
    mdl13.loadModel("thmsfrndfc low.dae");
    mdl13.setPosition(-300, 1000, 0);
    mdl13.setRotation(0, 270, 0, 1, 0);
    //    mdl13.setRotation(0, 20, 1, 0, 0);
    mdl13.setScale(1, 1, 1);
    o_mdls.push_back(mdl13);
    
    mdl14.loadModel("Pilow_HP.dae");
    mdl14.setPosition(0, 0, 0);
    //mdl14.setRotation(0, 100, 0, 1, 0);
    //    mdl14.setRotation(0, 20, 1, 0, 0);
    mdl14.setScale(1, 1, 1);
    n_mdls.push_back(mdl14);
    
    mdl15.loadModel("Apple.dae");
    mdl15.setPosition(0, 0, 0);
//    mdl15.setRotation(0, 100, 0, 1, 0);
    //    mdl15.setRotation(0, 20, 1, 0, 0);
    mdl15.setScale(1, 1, 1);
    n_mdls.push_back(mdl15);
    
    mdl16.loadModel("Dagger.dae");
    mdl16.setPosition(0, 0, 0);
//    mdl16.setRotation(0, 100, 0, 1, 0);
    //    mdl16.setRotation(0, 20, 1, 0, 0);
    mdl16.setScale(1, 1, 1);
    n_mdls.push_back(mdl16);
    
    mdl17.loadModel("Bone.dae");
    mdl17.setPosition(0, 0, 0);
//    mdl17.setRotation(0, 100, 0, 1, 0);
//    mdl17.setRotation(0, 20, 1, 0, 0);
    mdl17.setScale(2, 2, 2);
    n_mdls.push_back(mdl17);
    
    mdl18.loadModel("Skull.dae");
    mdl18.setPosition(0, 0, 0);
    //    mdl18.setRotation(0, 100, 0, 1, 0);
    //    mdl18.setRotation(0, 20, 1, 0, 0);
    mdl18.setScale(1, 1, 1);
    n_mdls.push_back(mdl18);
    
    
    light.setPosition(lx, ly, lz);
    cam.setDistance(500);
    
    cout << "n_mdls count: " << n_mdls.size() << endl;
    cout << "ss_mdls count: " << ss_mdls.size() << endl;
    cout << "bs_mdls count: " << bs_mdls.size() << endl;
    cout << "o_mdls count: " << o_mdls.size() << endl;
    
}
