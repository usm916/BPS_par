#include "testApp.h"
/*
 These functions are for adding quads and triangles to an ofMesh -- either
 vertices, or texture coordinates.
 */
//--------------------------------------------------------------
void addFace(ofVboMesh& mesh, ofVec3f a, ofVec3f b, ofVec3f c) {
	mesh.addVertex(a);
	mesh.addVertex(b);
	mesh.addVertex(c);
}

//--------------------------------------------------------------
void addFace(ofVboMesh& mesh, ofVec3f a, ofVec3f b, ofVec3f c, ofVec3f d) {
	addFace(mesh, a, b, c);
	addFace(mesh, a, c, d);
}

//--------------------------------------------------------------
void addTexCoords(ofVboMesh& mesh, ofVec2f a, ofVec2f b, ofVec2f c) {
	mesh.addTexCoord(a);
	mesh.addTexCoord(b);
	mesh.addTexCoord(c);
}

//--------------------------------------------------------------
void addTexCoords(ofVboMesh& mesh, ofVec2f a, ofVec2f b, ofVec2f c, ofVec2f d) {
	addTexCoords(mesh, a, b, c);
	addTexCoords(mesh, a, c, d);
}

void testApp::setup(){
    ///////////////////////////////////////////////////
    fluidSetup();
    ofSetVerticalSync(true);
    ofEnableNormalizedTexCoords(); //This lets you do 0-1 range instead of 0-640 (in pixels)
    ofHideCursor();
    
    setupScene();
    
    cloud.loadImage("images/exCloud.png");
    for(int i=0;i<2;i++){
        clouds[i] = CloudsImage(&cloud, &fluidFboOpacity);
    }
    ///////////////////////////////////////////////////
	// this number describes how many bins are used
	// on my machine, 2 is the ideal number (2^2 = 4x4 pixel bins)
	// if this number is too high, binning is not effective
	// because the screen is not subdivided enough. if
	// it's too low, the bins take up so much memory as to
	// become inefficient.
	int binPower = 5;
    
    behaviaMode = 2;
    mouseCheck = false;
    bInvert = false;
    bInvertBG = false;
    bForest = false;
    bParticle = true;
    bFluid = false;
    bHideCur = true;
    
    starMap.loadImage("");
    
	particleSystem.setup(ofGetWidth(), ofGetHeight(), binPower);

	kParticles = 3;
    float paddingFrame = 100;
	float padding = 0;
	float maxVelocity = .5;
	for(int i = 0; i < kParticles * 1024; i++) {
		float x = ofRandom(padding, ofGetWidth() - padding);
		float y = ofRandom(padding, ofGetHeight() - padding);
		float xv = ofRandom(-maxVelocity, maxVelocity);
		float yv = ofRandom(-maxVelocity, maxVelocity);
		Particle particle(x, y, xv, yv);
		particleSystem.add(particle);
	}

	ofBackground(0);
    ra = ga = ba = 0;
    
	timeStep = 0.5;
	lineOpacity = 128;
	pointOpacity = 255;
	isMousePressed = false;
	slowMotion = false;
	particleNeighborhood = 4;
	particleRepulsion = 0.01;
	centerAttraction = 1.5;//.01
    
    ofSetFrameRate(45);
    
    // ---------------------------- FBO
    scrFbo.allocate(ofGetWidth(), ofGetHeight(), GL_RGBA32F_ARB); // with alpha, 32 bits red, 32 bits green, 32 bits blue, 32 bits alpha, from 0 to 1 in 'infinite' steps
	scrFbo.begin();
	ofClear(255,255,255, 0);
    scrFbo.end();
    fluidFbo.allocate(width, height, GL_RGBA32F_ARB); // with alpha, 32 bits red, 32 bits green, 32 bits blue, 32 bits alpha, from 0 to 1 in 'infinite' steps
	fluidFbo.begin();
	ofClear(255,0,0, 0);
    fluidFbo.end();
    
    //----------------------- OSC setup
    cout << "listening for osc messages on port " << PORT << "\n";
	receiver.setup(PORT);
    
	current_msg_string = 0;
    for(int i=0;i<IDnum;i++){
        handX[i] = 0.0;
        handY[i] = 0.0;
        legX[i] = .0;
        legY[i] = .0;
        shoulderX[i] = .0;
        shoulderY[i] = .0;
        elbowX[i] = .0;
        elbowY[i] = .0;
        userCenterX[i] = .0;
        userCenterY[i] = .0;
        for(int j=0;j<MEM;j++){
            pHandX[i][j]=0.0;
            pHandY[i][j]=0.0;
            pLegX[i][j]=0.0;
            pLegY[i][j]=0.0;
            pShoulderX[i][j]=0.0;
            pShoulderY[i][j]=0.0;
            pElbowX[i][j]=0.0;
            pElbowY[i][j]=0.0;
            puCenterX[i][j]=.0;
            puCenterY[i][j]=.0;
            
            pMouseX[j]=0;
            pMouseY[j]=0;
        }
    }
    // end of OSC setup
    backToNormal = false;
    ofSetCircleResolution(48);
    
    filterFactorHandPos = 0.1;
    cam3D.setScale(1,-1,1);
    
    // randomly add a point on a sphere
	int   num = 1000;
	float radius = 1000;
	for(int i = 0; i<num; i++ ) {
		
		float theta1 = ofRandom(0, TWO_PI);
		float theta2 = ofRandom(0, TWO_PI);
		
		ofVec3f p;
		p.x = cos( theta1 ) * cos( theta2 );
		p.y = sin( theta1 );
		p.z = cos( theta1 ) * sin( theta2 );
		p *= radius;
		
		addPoint(p.x, p.y, p.z);
        
	}
	// upload the data to the vbo
	int total = (int)points.size();
	vbo.setVertexData(&points[0], total, GL_STATIC_DRAW);
	vbo.setNormalData(&sizes[0], total, GL_STATIC_DRAW);
    //load texture
    ofDisableArbTex();
//    site.setTextureWrap(GL_REPEAT, GL_REPEAT);
//    site.allocate(ofGetWidth(), ofGetHeight(), GL_RGB, GL_RGB);
    ofLoadImage(tex, "x.png");
    ofLoadImage(site, "site.png");
//    site.loadImage("site.jpg");
    
    // load the shader
	if(shader.load("shader")) {
		printf("Shader is loaded\n");
	}
    
    cout<<"Load BLOOM"<<endl;
    bloomShader.load( "shaders/bloom_GLSL"); //Loads both frag and vert if they are named correctly
    cout<<"Load BRCOSA"<<endl;
    brcosaShader.load( "shaders/brcosa_GLSL");
    cout<<"Load CHROMA"<<endl;
    //    chromaShader.load("shaders/ChromaAb_GLSL");
    chromaShader.load("shaders/ChromaAb_GLSLcstm");
    cout<<"Load BLUR"<<endl;
    blurShader.load("shaders/Gauss_GLSL");
    cout<<"Load FISHEYE"<<endl;
    fishShader.load("shaders/Fisheye_GLSL");
//    cout<<"Load ROTA"<<endl;
//    rotaShader.load("shaders/Rota_GLSL");
//    cout<<"Load CARTOPOL"<<endl;
//    carToPolShader.load("shaders/CarToPol_GLSL");
//    cout<<"Load WOBBLE"<<endl;
//    wobbleShader.load("shaders/Wobble_GLSL");
//    cout<<"Load LUMAKEY"<<endl;
//    lumaShader.load("shaders/LumaKey_GLSL");
//    cout<<"Load RADIAL"<<endl;
//    radialShader.load("shaders/Radial_GLSL");
    chromeNoise = 0.0;
    chromeMove = 0.0;

    
    soundCount = 0;
    
    float width = 10, height = 3;
    
	// OF_PRIMITIVE_TRIANGLES means every three vertices create a triangle
	scrVboMesh.setMode(OF_PRIMITIVE_TRIANGLES);
    float gridSizeX, gridSizeY;
    gridSizeX = ofGetWidth()/width;
    gridSizeY = ofGetHeight()/height;
	int skip = 1;	// this controls the resolution of the mesh
	ofVec3f zero(0, 0, 0);
	for(int y = 0; y < height; y++) {
		for(int x = 0; x < width; x++) {
			/*
			 To construct a mesh, we have to build a collection of quads made up of
			 the current pixel, the one to the right, to the bottom right, and
			 beneath. These are called nw, ne, se and sw. To get the texture coords
			 we need to use the actual image indices.
			 */
			ofVec3f nw = ofVec3f( x*gridSizeX, y*gridSizeY, 0);
			ofVec3f ne = ofVec3f( x*gridSizeX+gridSizeX, y*gridSizeY, 0);
			ofVec3f se = ofVec3f( x*gridSizeX+gridSizeX, y*gridSizeY+gridSizeY, 0);
			ofVec3f sw = ofVec3f( x*gridSizeX, y*gridSizeY+gridSizeY, 0);
			ofVec2f nwi((float)x/width, (float)y/height);
			ofVec2f nei(float((x+1.0)/width), (float)y/height);
			ofVec2f swi((float)x/width, (float)(y+1)/height);
			ofVec2f sei(float((x+1.0)/width), (float)(y+1)/height);
//            cout << nwi <<" | "<< nei <<" | "<< swi <<" | "<< sei << endl;
			
			// ignore any zero-data (where there is no depth info)
//			if(nw != zero && ne != zero && sw != zero && se != zero) {
            addFace(scrVboMesh, nw, ne, se, sw);
            addTexCoords(scrVboMesh, nwi, nei, sei, swi);
//			}
		}
	}
    // FLUID MESH
    ofVec3f nw = ofVec3f( -ofGetWidth()/2, -ofGetHeight()/2, 0);
    ofVec3f ne = ofVec3f( ofGetWidth()/2, -ofGetHeight()/2, 0);
    ofVec3f se = ofVec3f( ofGetWidth()/2, ofGetHeight()/2, 0);
    ofVec3f sw = ofVec3f( -ofGetWidth()/2, ofGetHeight()/2, 0);
    ofVec2f nwi( 0, 0);
    ofVec2f nei( 1, 0);
    ofVec2f swi( 1, 1);
    ofVec2f sei( 0, 1);
    addFace(fluidScrMesh, nw, ne, se, sw);
    addTexCoords(fluidScrMesh, nwi, nei, swi, sei);
    
    bSetVertex = false;
    scr = &scrFbo.getTextureReference();
    scr->setTextureWrap(GL_REPEAT, GL_REPEAT);
    fluidTexture = &fluidFbo.getTextureReference();
    fluidTexture->setTextureWrap(GL_REPEAT, GL_REPEAT);

}

void testApp::update(){
    while(receiver.hasWaitingMessages()){
		// get the Ïnext message
		ofxOscMessage m;
		receiver.getNextMessage(&m);
        
        ///////////////////////////////////////////////////
        char nameOsc[20];
        char numOsc[20], adrA[20], adrB[20];
        char ssnumOsc[20], c[50];
        float fadrA, fadrB;
        
        char oibf[100];// = m.getAddress().c_str();
        strcpy(oibf, m.getAddress().c_str());
        sscanf( oibf, "/%[^/]/%[^/]/%[^/]/%[^/]", numOsc, nameOsc, adrA, adrB);
        stringstream getMessage;
        getMessage << m.getAddress().c_str();
        getMessage >> c >> fadrA >> c >> fadrB;
        string checkStr = "";
        checkStr = checkStr + nameOsc;
        string checkStrG = "";
        checkStrG = checkStrG + numOsc;
        ///////////////////////////////////////////////////
        float const xyscale = 1.0;//when you use VGA 1.0
        float width=640*xyscale, height=480*xyscale, scaleX, scaleY;
		// check for mouse moved message
        if(!mouseCheck){
            if(m.getAddress() == "/joint/position"){
                // both the arguments are int32's
                idNumber = m.getArgAsFloat(0);
                handX[0] = 1-(m.getArgAsFloat(1)/width);
                handY[0] = 1-(m.getArgAsFloat(2)/height);
                handX[1] = 1-(m.getArgAsFloat(3)/width);
                handY[1] = 1-(m.getArgAsFloat(4)/height);
                legX[0] = 1-(m.getArgAsFloat(5)/width);
                legY[0] = 1-(m.getArgAsFloat(6)/height);
                legX[1] = 1-(m.getArgAsFloat(7)/width);
                legY[1] = 1-(m.getArgAsFloat(8)/height);
                shoulderX[0] = 1-(m.getArgAsFloat(9)/width);
                shoulderY[0] = 1-(m.getArgAsFloat(10)/height);
                shoulderX[1] = 1-(m.getArgAsFloat(11)/width);
                shoulderY[1] = 1-(m.getArgAsFloat(12)/height);
                elbowX[0] = 1-(m.getArgAsFloat(13)/width);
                elbowY[0] = 1-(m.getArgAsFloat(14)/height);
                elbowX[1] = 1-(m.getArgAsFloat(15)/width);
                elbowY[1] = 1-(m.getArgAsFloat(16)/height);
                userCenterX[0] = 1-(m.getArgAsFloat(17)/height);
                userCenterY[1] = 1-(m.getArgAsFloat(18)/width);
                neck[0] = 1-(m.getArgAsFloat(19)/width);
                neck[1] = 1-(m.getArgAsFloat(20)/height);
                rotUser[0] = m.getArgAsFloat(21);
//                fmsx = m.getArgAsFloat(22);
//                fmsy = m.getArgAsFloat(23);
//                behaviour = m.getArgAsFloat(24);
//                bInvert = valueToBool(m.getArgAsFloat(25));
//                bParticle = valueToBool(m.getArgAsFloat(26));
//                bTulou = valueToBool(m.getArgAsFloat(27));
//                bForest = valueToBool(m.getArgAsFloat(28));
//                bFluid = valueToBool(m.getArgAsFloat(29));
            }else
            if(m.getAddress() == "/soundT"){
                soundT = m.getArgAsFloat(0);
            }else
            if(m.getAddress() == "/soundL"){
                soundL = m.getArgAsFloat(0);
            }
        }
    }
}

void testApp::draw(){
    // draw BG
//    ofBackgroundGradient(ofColor(60,0,100), ofColor(10,0,10), OF_GRADIENT_CIRCULAR);
    if(fluidFboOpacity>0.1)fluidUpdate();
    
    fluidFbo.begin();
    ofClear(0);
    fluidDraw();
    fluidFbo.end();
    
    updateValue();
    
    scrFbo.begin();
    ofClear(255);
    int xxxx =rtnAdrs((float)mouseX, (float)mouseY);

    drawParticles();
    scrFbo.end();
    
    if(!mouseCheck)behaviaMode = behaviour;
    setMode(behaviaMode);
    
    soundCount++;

    scr->setTextureWrap(GL_REPEAT, GL_REPEAT);
    scr->bind();
    // draw fbo
//    brcosaShader.begin();
//    brcosaShader.setUniformTexture("tex0", *scr, 0);
//    brcosaShader.setUniform1f("contrast", 1.0);
//    brcosaShader.setUniform1f("brightness", 1.0);
//    brcosaShader.setUniform1f("saturation", 5*sin(ofGetElapsedTimef()));
//    brcosaShader.setUniform1f("alpha", 1.0);
    
    chromaShader.begin();
    chromaShader.setUniformTexture("tex0", *scr, 0);
    chromaShader.setUniform2f("windowSize", ofGetHeight(), ofGetHeight());
    float line = ofRandom(0,ofGetWidth());
    chromaShader.setUniform2f("roiSize", ofRandom(-2,0)+line, ofRandom(0,2)+line );

        chromeNoise = ofNoise( (float)ofGetElapsedTimeMillis()/100.0);
    changeBG();
    if(chromeNoise>0.65){
//        chromeMove = ofRandom(-30,30);
        chromaShader.setUniform1f("offsetALL", chromeMove);//10*sin(ofGetElapsedTimef()));
    }else{
        chromaShader.setUniform1f("offsetALL", 0);
    }
    if(!bInvertBG){
//    if(!bBlack){
        chromaShader.setUniform1i("bInvert", 0);
    }else{
        chromaShader.setUniform1i("bInvert", 1);
    }
    scrVboMesh.draw();
//    brcosaShader.end();
    chromaShader.end();
    scr->unbind();
    
    if(bSetVertex)setScreenMeshVertex();
    
    if(mouseCheck){
        ofSetColor(255);
        ofDrawBitmapString(ofToString(kParticles) + "k particles", 32, 30);
        ofDrawBitmapString(ofToString((int) ofGetFrameRate()) + " [fps]", 32, 45);
        stringstream ss;
        ss << pMouseX[5] << ", " << mouseX << ", ";
        ofDrawBitmapString(ss.str(), 32,60);
        ss.str("");
        ss << pMouseX[0] << ", " << pMouseX[1] << ", " << pMouseX[2] << ", " <<pMouseX[3] << ", " <<pMouseX[4] << ", " <<pMouseX[5];
        ofDrawBitmapString(ss.str(), 32,75);
    }
    
    ofSetColor(255);
    ofDrawBitmapString( ofToString(scrHandX[0]) + ", " + ofToString(scrHandY[0]), 50,50);
    
    ofDisableAlphaBlending();
    ofDisableBlendMode();
    
    
}

//--------------------------------------------------------------------------------------------
void testApp::changeBG(){
    if(bPInvert!=bInvert){
        invertCount = 0;
    }
    if(invertCount>10){
        bInvertBG = bInvert;
        chromeMove = ofRandom(-30,30);
    }else{
        if( (invertCount%2)==1 ){
            bInvertBG = valueToBool((int)ofRandom(1.7));
            chromeMove = ofRandom(-60,60);
            chromeNoise = 0.9;
        }
    }
    invertCount ++;
    bPInvert = bInvert;
}

void testApp::drawParticles(){
    
    glSetting();
//    glDepthMask(GL_FALSE);
	ofEnableAlphaBlending();
    ofEnableBlendMode(OF_BLENDMODE_ALPHA);
    ofEnablePointSprites();
    if(!bInvert){
        ofBackgroundGradient(ofColor(60,0,100), ofColor(10,0,10), OF_GRADIENT_CIRCULAR);
    }if(bInvert){
        ofBackgroundGradient(ofColor(r,g,b), ofColor(75,65,95), OF_GRADIENT_CIRCULAR);
    }
    

    cam3D.begin();
    ofTranslate(-ofGetWidth()/2, -ofGetHeight()/2, 70);

    if(lineOpacity>0.1){
        if(!bInvert)
            ofSetColor(245, 255, 220, lineOpacity);
        else
            ofSetColor((255-20), (255), (255-73), lineOpacity);
        
        particleSystem.setTimeStep(timeStep);
        particleSystem.setupForces();
        // apply per-particle forces
        int cntLine = 0;
        glBegin(GL_LINES);
        for(int i = 0; i < particleSystem.size(); i++) {
            Particle& cur = particleSystem[i];
            // global force on other particles
            particleSystem.addRepulsionForce(cur, particleNeighborhood, particleRepulsion);
            // forces on this particle
            cur.bounceOffWalls(0, 0, ofGetWidth(), ofGetHeight());
            cur.addDampingForce();
        }
        glEnd();
    }

    // draw SCENE in particle space
    ofPushMatrix();
    ofTranslate(ofGetWidth()/2, ofGetWidth()/5+userCenterY[0]);
    ofRotateY(-rotUser[0]/2);
    if(forestOpacity>0) {
        drawSceneTrees();
    }
    if(tulouOpacity>0) {
        drawSceneTulou();
    }

    ofPopMatrix();

    drawParameters();
    setForces();
	particleSystem.update();

    /////////////////////////////////////////////////////////////////////////////// hand Trace
    
    float msx, msy, imsx, imsy, scrX, scrY;
    msx = mouseX;
    msy = mouseY;
    imsx = ofGetWidth()-msx;
    imsy = ofGetHeight()-msy;
    if(!mouseCheck){
        for(int i=0;i<2;i++){
            ofVec2f h = ofVec2f(scrHandX[i],scrHandY[i]);
            ofVec2f ph = ofVec2f(pHandX[i][5],pHandY[i][5]);
            if( !h.match(ph, 15) ){
                clouds[i].draw( &scrHandX[i], &scrHandY[i]);
            }
        }
    }else{
        ofVec2f msp = ofVec2f(mouseX, mouseY);
        ofVec2f pmsp = ofVec2f(pMouseX[5], pMouseY[5]);
        if( !msp.match(pmsp, 5.0) ){
            clouds[0].draw( &msx, &msy );
            clouds[1].draw( &imsx, &imsy );
        }
    }
    /////////////////////////////////////////////////////////////////////////////// hand Trace End

    if(pointOpacity>0.1){
        ofSetColor(255, pointOpacity);
        particleSystem.draw();
    }

    ///////////////////////////////////////////////////// draw Fluid image in particles
//    if(bFluid){
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
        //        ofEnableBlendMode(OF_BLENDMODE_ADD);
        ofPushMatrix();
        //        ofTranslate(ofGetWidth()/2, ofGetHeight()/2);
        ofSetColor(255,255,255,fluidFboOpacity);
        fluidFbo.draw(0,0,ofGetWidth(), ofGetHeight());
        ofPopMatrix();
        ofDisableBlendMode();
//    }

    cam3D.end();
    
    ofDisableAlphaBlending();
    ofDisablePointSprites();
	ofDisableBlendMode();
}

void testApp::drawParameters(){
    // single global forces
    ofNoFill();
    for(int i=0;i<2;i++){
        if(!bInvert)
            ofSetColor(255,192);
        else
            ofSetColor(40,192);

        scrHandX[i] = ofGetWidth() * handX[i];
        scrHandY[i] = ofGetHeight() * handY[i];
        scrLegX[i] = ofGetWidth() * legX[i];
        scrLegY[i] = ofGetHeight() * legY[i];
        scrElbowX[i] = ofGetWidth() * elbowX[i];
        scrElbowY[i] = ofGetHeight() * elbowY[i];
        scrShoulderX[i] = ofGetWidth() * shoulderX[i];
        scrShoulderY[i] = ofGetHeight() * shoulderY[i];
        scrUserCenterX[i] = ofGetWidth() * userCenterX[i];
        scrUserCenterY[i] = ofGetHeight() * userCenterY[i];
        
        neckPos = ofPoint(ofGetWidth()*neck[0], ofGetHeight()*neck[1]);

        drawNodeCursor(scrHandX[i], scrHandY[i], pHandX[i][0], pHandY[i][0], 45);
//        drawNodeCursor(scrShoulderX[i], scrShoulderY[i], pShoulderX[i][0], pShoulderY[i][0], 25);
//        drawNodeCursor(scElbowX[i], scrElbowY[i], pElbowX[i][0], pElbowY[i][0], 25);
//        drawNodeCursor(scrLegX[i], scrLegY[i], pLegX[i][0], pLegY[i][0], 45);
//        drawNodeCursor(scrUserCenterX[i], scrUserCenterY[i], puCenterX[i][0], puCenterY[i][0], 45);
        
//        drawControlWire(scrHandX[i], scrHandY[i]);
//        drawControlWire(scrElbowX[i], scrElbowY[i]);
//        drawControlWire(scrHandX[i], scrHandY[i]);
//        drawControlWire(scrLegX[i], scrLegY[i]);
//        // draw line Neck to Hand
//        ofLine(neckPos.x, neckPos.y, scrHandX[0], scrHandY[0]);
//        ofLine(neckPos.x, neckPos.y, scrHandX[1], scrHandY[1]);
//        ofCircle(neck[0], neck[1], 5);
        
        //Display Paramater
//        stringstream ss;
//        ss <<"[id:"<<i<<"-"<<scrHandX[i]<<", "<<scrHandY[i]<<" ]";
//        ofDrawBitmapString(ss.str(), scrHandX[i]+20, scrHandY[i]-30);
    }
//    drawNodeCursor(neck[0], scrUserCenterY[i], puCenterX[i][0], puCenterY[i][0], 45);
//    drawNodeCursor(mouseX, (ofGetHeight()-mouseY), pMouseX[0], pMouseY[0], 40);
//    drawControlWire(mouseX, (ofGetHeight()-mouseY));
//    drawControlWire(scrUserCenterY[0], scrUserCenterY[0]);

}

void testApp::drawNodeCursor(float _x, float _y, float _px, float _py, float _size){
    
    ofColor(255);
    ofPushMatrix();
    ofTranslate( _x, _y);
    ofLine(-20, 0, 20, 0);
    ofLine(0, -20, 0, 20);
    ofCircle(0, 0, _size);
    ofCircle(0, 0, _size*1.1);
    
    ofVec2f rote = ofVec2f(_x-_px, _y-_py);
    float scalar = rote.distance(ofVec2f(.0,.0));
    
    rote.normalize();
    if(rote.y<0){
        ofRotateZ( -acos(rote.x)*180/PI );
    }else{
        ofRotateZ( acos(rote.x)*180/PI );
    }
    
    if(scalar>4.0){
        ofFill();
        ofRect(40-15, -5, 30, 10);
        ofNoFill();
    }
    ofPopMatrix();
}

void testApp::drawControlWire(float _x, float _y){
    int blur = 30;
    ofSetColor(255);
    ofPushMatrix();
    
    ofTranslate( _x, _y);
    ofLine( 0,0, ofRandom(-blur,blur), ofGetHeight()*2);
    ofSetColor(255, 128);
    ofLine( 0,0, ofRandom(-blur,blur), ofGetHeight()*2);
    ofLine( 0,0, ofRandom(-blur,blur), ofGetHeight()*2);
    
    ofPopMatrix();
}

void testApp::setScreenMeshVertex(){
    for(int i=0;i<scrVboMesh.getNumVertices();i++){
        ofSetColor(255);
        ofPoint vi = scrVboMesh.getVertex(i);
        ofCircle(vi, 10);
        if(isMouseDragged && checkAreaRect(mouseX, mouseY, vi, ofVec2f(50,50) ) ){
            scrVboMesh.setVertex(i, ofPoint(mouseX, mouseY, 0.0));
        }
    }
    scrVboMesh.disableTextures();
    scrVboMesh.drawWireframe();
    scrVboMesh.enableTextures();
}

void testApp::setForces(){
    int trg = MEM-1;
    if(mouseCheck){
        if(pMouseX[trg]!=mouseX && pMouseY[trg]!=mouseY ){
            particleSystem.addAttractionForce(mouseX, ofGetHeight()-mouseY, ofGetWidth()/2, centerAttraction);
//            particleSystem.addAttractionForce(ofGetWidth()-mouseX, ofGetHeight()-mouseY, ofGetWidth()/2, centerAttraction);
            
        }else{
            particleSystem.addAttractionForce(ofGetWidth()/2, ofGetHeight()/2, ofGetWidth()/2, centerAttraction);
            particleSystem.addRepulsionForce(ofGetWidth()/2, ofGetHeight()/2, 100, 20);
        }
    }else{
        if(pHandX[0][trg]!=scrHandX[0] && pHandY[0][trg]!=scrHandY[0] && pHandX[1][trg]!=scrHandX[1] && pHandY[1][trg]!=scrHandY[1]){
            particleSystem.addAttractionForce(scrHandX[0], scrHandY[0], ofGetWidth()/2, centerAttraction);
            particleSystem.addAttractionForce(scrHandX[1], scrHandY[1], ofGetWidth()/2, centerAttraction);
        }else{
            particleSystem.addAttractionForce(ofGetWidth()/2, ofGetHeight()/2, ofGetWidth()/2, centerAttraction);
            particleSystem.addRepulsionForce(ofGetWidth()/2, ofGetHeight()/2, 100, 25);
        }
    }
    
    for(int i=0;i<IDnum;i++){
        pHandX[i][0]=scrHandX[i]*filterFactorHandPos + pHandX[i][0]*(1-filterFactorHandPos);
        pHandY[i][0]=scrHandY[i]*filterFactorHandPos + pHandY[i][0]*(1-filterFactorHandPos);
        pElbowX[i][0]=scrElbowX[i]*filterFactorHandPos + pElbowX[i][0]*(1-filterFactorHandPos);
        pElbowY[i][0]=scrElbowY[i]*filterFactorHandPos + pElbowY[i][0]*(1-filterFactorHandPos);
        pShoulderX[i][0]=scrShoulderX[i]*filterFactorHandPos + pShoulderX[i][0]*(1-filterFactorHandPos);
        pShoulderY[i][0]=scrShoulderY[i]*filterFactorHandPos + pShoulderY[i][0]*(1-filterFactorHandPos);
        pLegX[i][0]=scrLegX[i]*filterFactorHandPos + pLegX[i][0]*(1-filterFactorHandPos);
        pLegY[i][0]=scrLegY[i]*filterFactorHandPos + pLegY[i][0]*(1-filterFactorHandPos);
        puCenterX[i][0]=scrUserCenterX[i]*filterFactorHandPos + puCenterX[i][0]*(1-filterFactorHandPos);
        puCenterY[i][0]=scrUserCenterY[i]*filterFactorHandPos + puCenterY[i][0]*(1-filterFactorHandPos);
        pRotUser[i][0]=rotUser[i]*filterFactorHandPos + pRotUser[i][0]*(1-filterFactorHandPos);
        for(int j=MEM-2;j>-1;j--){
            pHandX[i][j+1] = pHandX[i][j];
            pHandY[i][j+1] = pHandY[i][j];
            pLegX[i][j+1] = pLegX[i][j];
            pLegY[i][j+1] = pLegY[i][j];
            pShoulderX[i][j+1] = pShoulderX[i][j];
            pShoulderY[i][j+1] = pShoulderY[i][j];
            pElbowX[i][j+1] = pElbowX[i][j];
            pElbowY[i][j+1] = pElbowY[i][j];
            puCenterX[i][j+1] = puCenterX[i][j];
            puCenterY[i][j+1] = puCenterY[i][j];
            pRotUser[i][j+1] = pRotUser[i][j];
        }
    }
    pMouseX[0]=mouseX;
    pMouseY[0]=mouseY;
    for(int i=MEM-2;i>-1;i--){
        pMouseX[i+1] = pMouseX[i];
        pMouseY[i+1] = pMouseY[i];
    }
//	particleSystem.addAttractionForce(ofGetWidth() / 2, ofGetHeight() / 2, ofGetWidth(), centerAttraction);
	if(isMousePressed){particleSystem.addRepulsionForce(mouseX, mouseY, 100, 20);}
}

void testApp::keyPressed(int key){
	if(key == 'p'){
		ofSaveScreen(ofToString(ofGetMinutes()) + "_" + ofToString(ofGetFrameNum()) + ".png");
        bPaint = !bPaint;
    }
    if(key == '1')behaviaMode = 1;
    if(key == '2')behaviaMode = 2;
    if(key == '3')behaviaMode = 3;
    if(key == '4')behaviaMode = 4;
    if(key == '5')behaviaMode = 5;
    if(key == '|')ofToggleFullscreen();
    if(key == 'm')mouseCheck = !mouseCheck;
    if(key == 'i'){
        bInvert = !bInvert;
//        bInvertBG = !bInvertBG;
    }
    if(key == 'e')bSetVertex = !bSetVertex;
    if(key == 'o')bObstacle = !bObstacle;
    if(key == 'b'){
        bBounding = !bBounding;
    }
    if(key == 'c')bClear = !bClear;
    if(key == 'd')bForest = !bForest;
    if(key == 's')bTulou = !bTulou;
    if(key == 'a')bParticle = !bParticle;
    if(key == 'f')bFluid = !bFluid;
    if(key == 'S')ofSaveFrame(ofGetElapsedTimeMillis()+".png");
    if(key == 'h')ofShowCursor();
}


void testApp::updateValue(){
    if(bParticle){
        tarPointOpacity = 255;
        tarLineOpacity = 64;
    } else {
        tarPointOpacity = 0;
        tarLineOpacity = 0;
    }
    if(bForest){ tarForestOpacity = 64; } else { tarForestOpacity=0;}
    if(bTulou){ tarTulouOpacity = 64; } else { tarTulouOpacity=0;}
    if(bFluid){ tarFluidFboOpacity = 255; } else { tarFluidFboOpacity=0;}
    if(bInvert){
        tarR = 0;
        tarG = 0;
        tarB = 0;
        tarEdgeR = 75;
        tarEdgeG = 65;
        tarEdgeB = 95;
    } else {
        tarR = 60;
        tarG = 0;
        tarR = 100;
        tarEdgeR = 10;
        tarEdgeG = 0;
        tarEdgeB = 10;
    }
    linearTrasition(&forestOpacity, 1, &tarForestOpacity);
    linearTrasition(&tulouOpacity, 1, &tarTulouOpacity);

    linearTrasition(&timeStep, 0.01, &tarTimeStep);
    linearTrasition(&lineOpacity, 2, &tarLineOpacity);
    linearTrasition(&pointOpacity, 2, &tarPointOpacity);
    linearTrasition(&fluidFboOpacity, 2, &tarFluidFboOpacity);

    linearTrasition(&particleNeighborhood, 0.1, &tarParticleNeighborhood);
    linearTrasition(&particleRepulsion, 0.03, &tarParticleRepulsion);
    linearTrasition(&centerAttraction, 0.03, &tarCenterAttraction);
    int colorStep = 128;
    linearTrasition(&r, colorStep, &tarR);
    linearTrasition(&g, colorStep, &tarG);
    linearTrasition(&b, colorStep, &tarB);
    linearTrasition(&edgeR, colorStep, &tarEdgeR);
    linearTrasition(&edgeG, colorStep, &tarEdgeG);
    linearTrasition(&edgeB, colorStep, &tarEdgeB);
}

void testApp::setMode(int _mode){
    switch (_mode) {
        case 1:
            tarTimeStep = 0.9;
            tarParticleNeighborhood = 18;//18
            tarParticleRepulsion = 0.01;
            tarCenterAttraction = 3;//.01
            break;
        case 2:
            tarTimeStep = 0.05;
            tarParticleNeighborhood = 18;//18
            tarParticleRepulsion = 0.01;
            tarCenterAttraction = 0.9;//.01
            break;
        case 3:
            tarTimeStep = 0.5;
            tarParticleNeighborhood = 18;//22
            tarParticleRepulsion = 0.7;
            tarCenterAttraction = 0.2;//.01
            break;
        case 4://default
            tarTimeStep = 0.4;
            tarParticleNeighborhood = 18;//18
            tarParticleRepulsion = 0.02;
            tarCenterAttraction = 1.4;//.01
            break;
        case 5:
            tarTimeStep = 0.6;
            tarParticleNeighborhood = 12;//12
            tarParticleRepulsion = 0.01;
            tarCenterAttraction = 3.9;//.01
            break;
        default:
            break;
    }
}

void testApp::mouseDragged(int x, int y, int button){
    isMouseDragged = true;
}

void testApp::mousePressed(int x, int y, int button){
	isMousePressed = true;
    isMouseDragged = true;
}

void testApp::mouseReleased(int x, int y, int button){
	isMousePressed = false;
    isMouseDragged = false;
}

//--------------------------------------------------------------
void testApp::glSetting(){
    /*
    //--------------------------- for the Background
    //    ofSetBackgroundAuto(true);
    //    glBlendFunc(GL_ONE, GL_ONE);
    //    ofSetColor(0, 0, 0, 50);
    //    ofRect(ofGetWidth()/2, ofGetHeight()/2, ofGetWidth(), ofGetHeight());

    //-----------------------------------------GL setting
    ofEnableAlphaBlending();
    glEnable(GL_BLEND);
    int bgStep = 16;
    //glBlendEquationOES(GL_ADD);
    if(!bInvert){//PSSCREEN
        if(ra>60){
            ra-=bgStep;
            ga-=bgStep;
            ba-=bgStep;
        }
        if(ra<60){
            ra=60;
        }
        if(ga<0){
            ga=0;
        }
        if(ba<100){
            ba=100;
        }
        
        ofBackground(ra,ga,ba);
        ofBackgroundGradient(ofColor(ra,ga,ba), ofColor(10,0,10), OF_GRADIENT_CIRCULAR);

//        ofBackground(r*255+ra, g*255+ga, b*255+ba);
        if(ra<170){
            glBlendFunc(GL_SRC_ALPHA, GL_ONE);
        }else{
            glBlendFunc(GL_ZERO, GL_ONE_MINUS_SRC_ALPHA);
        }
    } else {
        if(ra == 60 )ra = 0;
        if(ra<255){
            ra+=bgStep;
            ga+=bgStep;
            ba+=bgStep;
        }
        if(ra>255){
            ra=255;
        }
        if(ga>255){
            ga=250;
        }
        if(ba>230){
            ba=230;
        }
        ofBackgroundGradient(ofColor(ra,ga,ba), ofColor(ra-70,ga-70,ba-70), OF_GRADIENT_CIRCULAR);
        if(ra<170){
            glBlendFunc(GL_SRC_ALPHA, GL_ONE);
        }else{
            glBlendFunc(GL_ZERO, GL_ONE_MINUS_SRC_ALPHA);
        }
    }
     */
}

//--------------------------------------------------------------
void testApp::addPoint(float x, float y, float z) {
	ofVec3f p(x, y, z);
	points.push_back(p);
	
	// we are passing the size in as a normal x position
	float size = ofRandom(5, 50);
	sizes.push_back(ofVec3f(size));
}

//------------------------------------------------------------------
bool testApp::checkAreaRect(float _x, float _y, ofPoint _centerPoint, ofVec2f _size){
    float xmin, xmax, ymin, ymax;
    xmin = _centerPoint.x - _size.x/2;
    xmax = _centerPoint.x + _size.x/2;
    ymin = _centerPoint.y - _size.y/2;
    ymax = _centerPoint.y + _size.y/2;
    if( xmin < _x && xmax > _x && ymin < _y && ymax > _y ){
        ofRect(_centerPoint, _size.x, _size.y);
        return true;
    }else{
        return false;
    }
}

//------------------------------------------------------------------
int testApp::rtnAdrs(float _x, float _y){
    int x = (int) _x / ofGetWidth() * GridNumX -1;
    int y = (int) _y / ofGetHeight() * GridNumY -1;
    ofSetColor(255);
    ofRect(x*GridNumX, y*GridNumY, ofGetWidth()/GridNumX, ofGetHeight()/GridNumY);
    return x + y * GridNumX;
}

//------------------------------------------------------------------
void testApp::callSound(bool _check, int _time){
    if(_check){
        ofSetColor(255);
        unsigned int num = 0;
        num = ofRandom(4);
        ofRect(ofGetWidth()/2, ofGetHeight()/2, 50, 50);
        if(soundCount > _time){
//            cout << num << endl;
            sound[num].play();
            soundCount = 0;
        }
    }
}

//--------------------------------------------------------------
void testApp::drawSceneSpace(){
    space01.drawObj(ofVec3f(0,0,0), 0, 0, 0, 255);
}

//--------------------------------------------------------------
void testApp::drawSceneTulou(){
//    ofEnableAlphaBlending();
    room.drawObj(ofVec3f(0,0,0), 0, 0, 0, tulouOpacity);
//    ofDisableAlphaBlending();
}

//--------------------------------------------------------------
void testApp::drawSceneTrees(){
//    ofEnableAlphaBlending();
    NodeForWhl.calMoveF(nwSpd);
    NodeForBird.calMoveF(nbSpd);
    
    for(int i=0;i<treePos1.size();i++){
        tree1.drawObj( treePos1[i] , 0, treeRot1[i], 0, forestOpacity);
        tree2.drawObj( treePos2[i] , 0, treeRot2[i], 0, forestOpacity);
        treeN1.drawObj( treePos1[i] , 0, treeRot1[i], 0, forestOpacity);
        treeN2.drawObj( treePos2[i] , 0, treeRot2[i], 0, forestOpacity);
    }
    NodeForFishes.calMoveF(nfSpd);
    for(int i=0;i<NodeForFishes.stPos.size();i++){
        fish.drawObj( ofVec3f(NodeForFishes.stPos[i].x,
                              NodeForFishes.stPos[i].y,
                              NodeForFishes.stPos[i].z),
                     NodeForFishes.stPos[i].rx/PI*180,
                     NodeForFishes.stPos[i].ry/PI*180 + 90,
                     NodeForFishes.stPos[i].rz/PI*180,
                     forestOpacity);
    }
    for(int i=0;i<NodeForWhl.stPos.size();i++){
        whl.drawObj( ofVec3f(NodeForWhl.stPos[i].x,
                             NodeForWhl.stPos[i].y,
                             NodeForWhl.stPos[i].z),
                    NodeForWhl.stPos[i].rx/PI*180,
                    NodeForWhl.stPos[i].ry/PI*180 + 90,
                    NodeForWhl.stPos[i].rz/PI*180,
                    forestOpacity);
    }
    for(int i=0;i<NodeForBird.stPos.size();i++){
        bird.drawObj( ofVec3f(NodeForBird.stPos[i].x,
                              NodeForBird.stPos[i].y,
                              NodeForBird.stPos[i].z),
                     NodeForBird.stPos[i].rx/PI*180,
                     NodeForBird.stPos[i].ry/PI*180 + 90,
                     NodeForBird.stPos[i].rz/PI*180,
                     forestOpacity);
    }

//    ofDisableAlphaBlending();
}

//--------------------------------------------------------------
void testApp::fluidSetup(){
    // set window size
    fluidScrScale = 2;
    width = ofGetWidth()/fluidScrScale;
    height = ofGetHeight()/fluidScrScale;
    
    // Initial Allocation
    //
    fluid.allocate(width, height, 0.5);
    
    //  Set obstacle
    //
    fluid.begin();
    fluid.end();
    
    // Adding constant forces
    //
    setDelta = 2;
    color = 0.1;
    bBlack = false;
    fluid.setGravity(ofVec2f(0.1,0.006));
//    fluid.addConstantForce(ofPoint(500,500), ofPoint(1,0.5), ofFloatColor(255));

}

//--------------------------------------------------------------
void testApp::fluidDraw(){
    // Seting the gravity set up & injecting the background image
    //
    fluid.setDissipation(0.98);
    fluid.setVelocityDissipation(0.995);
    fluid.draw();
}

//--------------------------------------------------------------
void testApp::fluidUpdate(){
    if(!bInvert){
        fluidColor = ofFloatColor(.70, 1.0, .62, 1.0);
        fluidColorScale = 0.5f;
        fluidForce = 5.1f;
    }else{
        fluidColor = ofFloatColor(.82, 1.0, .72, 1.0);
        fluidColorScale = 0.5f;
        fluidForce = 5.9f;
    }

    float margin = 20;
    for(int i=0;i<2;i++){
        if(scrHandX[i]<margin) scrHandX[i] = margin;
        if(scrHandX[i]>(ofGetWidth()-margin)) scrHandX[i] = ofGetWidth()-margin;
        if(scrHandY[i]<margin) scrHandY[i] = margin;
        if(scrHandY[i]>(ofGetHeight()-margin)) scrHandY[i] = ofGetWidth()-margin;
    }
    if(!mouseCheck){
        for(int i=0;i<2;i++){
            ofVec2f h = ofVec2f(scrHandX[i],scrHandY[i]);
            ofVec2f ph = ofVec2f(pHandX[i][5],pHandY[i][5]);
            if( !h.match(ph, 15) ){
                fluidDrawForce( scrHandX[i]/fluidScrScale, scrHandY[i]/fluidScrScale, fluidColor, fluidColorScale, fluidForce);
            }
        }
    }else{
        ofVec2f msp = ofVec2f(mouseX, mouseY);
        ofVec2f pmsp = ofVec2f(pMouseX[5], pMouseY[5]);
        if( !msp.match(pmsp, 5.0) ){
            fluidDrawForce( mouseX/fluidScrScale, mouseY/fluidScrScale, fluidColor, fluidColorScale, fluidForce);
            fluidDrawForce( (ofGetWidth()-mouseX)/fluidScrScale, (ofGetHeight()-mouseY)/fluidScrScale, fluidColor, fluidColorScale, fluidForce);
        }
    }
    fluid.setGravity(ofVec2f(-(rotUser[0]-pRotUser[0][0])/13, 0.2));
    //  Update
    //
    fluid.update();
}

//--------------------------------------------------------------
void testApp::fluidDrawForce(float _x, float _y, ofFloatColor _color, float _scale, float _force){
    // Adding temporal Force
    //
    float msX, msY;
    msX = _x;
    msY = _y;
    ofPoint m[8] = {
        ofPoint(msX, msY),
        ofPoint(msX+setDelta*ofNoise(0.5,0.4,ofGetElapsedTimeMillis()/500.0), msY+setDelta*ofNoise(0.9,0.8,ofGetElapsedTimeMillis()/500.0)),
        ofPoint(msX-setDelta*ofNoise(0.6,0.5,ofGetElapsedTimeMillis()/500.0), msY-setDelta*ofNoise(1.0,0.9,ofGetElapsedTimeMillis()/500.0)),
        ofPoint(msX+setDelta*ofNoise(0.7,0.6,ofGetElapsedTimeMillis()/500.0), msY-setDelta*ofNoise(1.1,1.0,ofGetElapsedTimeMillis()/500.0)),
//        ofPoint(msX-setDelta*ofNoise(0.8,0.7,ofGetElapsedTimeMillis()/500.0), msY+setDelta*ofNoise(1.2,1.1,ofGetElapsedTimeMillis()/500.0)),
//        ofPoint(msX+setDelta*ofNoise(1.5,0.9,ofGetElapsedTimeMillis()/500.0), msY+setDelta*ofNoise(1.9,0.1,ofGetElapsedTimeMillis()/500.0)),
//        ofPoint(msX-setDelta*ofNoise(0.3,0.2,ofGetElapsedTimeMillis()/500.0), msY+setDelta*ofNoise(1.7,3.0,ofGetElapsedTimeMillis()/500.0)),
//        ofPoint(msX+setDelta*ofNoise(1.9,0.3,ofGetElapsedTimeMillis()/500.0), msY+setDelta*ofNoise(1.8,0.7,ofGetElapsedTimeMillis()/500.0))
    };
    ofPoint d = (m[0] - oldM)*10.0;
    ofPoint drev = ofPoint(d.y,d.x);
    ofPoint mus = ofPoint(msX, msY);
    ofPoint c = ofPoint(width*0.5, height*0.5) - m[0];
    c.normalize();
    
    float scale = _scale;
    ofFloatColor color = ofFloatColor(_color.r * scale, _color.g * scale, _color.b * scale, _color.a);
    float force = _force;
    
    if(mus!=pmus){
//    fluid.addTemporalForce(m2, d, ofFloatColor(0.7,0.7,1),5.0f);
        fluid.addTemporalForce(m[0], d*ofRandom(0.5,1.0), color, force*ofNoise(0.5,0.4,ofGetElapsedTimeMillis()/500.0)+0.3);
        fluid.addTemporalForce(m[1], -d*ofRandom(0.5,1.0), color, force*ofNoise(0.6,0.5,ofGetElapsedTimeMillis()/500.0)+0.3);
        fluid.addTemporalForce(m[2], drev*ofRandom(0.5,1.0), color, force*ofNoise(0.8,0.7,ofGetElapsedTimeMillis()/500.0)+0.3);
        fluid.addTemporalForce(m[3], -drev*ofRandom(0.5,1.0), color, force*ofNoise(1.9,0.3,ofGetElapsedTimeMillis()/500.0)+0.3);
//        fluid.addTemporalForce(m[4], ofPoint(drev.x,0), color, force*ofNoise(1.1,1.0,ofGetElapsedTimeMillis()/500.0)+0.3);
//        fluid.addTemporalForce(m[5], ofPoint(0,drev.y), color, force*ofNoise(1.7,3.0,ofGetElapsedTimeMillis()/500.0)+0.3);
//        fluid.addTemporalForce(m[6], ofPoint(-drev.x,0), color, force*ofNoise(0.9,0.8,ofGetElapsedTimeMillis()/500.0)+0.3);
//        fluid.addTemporalForce(m[7], ofPoint(0,-drev.y), color, force*ofNoise(1.9,0.3,ofGetElapsedTimeMillis()/500.0)+0.3);
//    fluid.addTemporalForce(m, d, ofFloatColor(c.x,c.y,0.5),8.0f);
    }
    oldM = m[0];
    pmus = mus;
}

//--------------------------------------------------------------
void testApp::setupScene(){
    bird = ObjLoader("bird.txt", 20.0);
    whl = ObjLoader("whl.txt", 20.0);
    tree1 = ObjLoader("tree1.txt", 60.0);
    tree2 = ObjLoader("tree2.txt", 63.0);
    treeN1 = ObjLoader("treeN1.txt", 60.0);
    treeN2 = ObjLoader("treeN2.txt", 63.0);
    fish = ObjLoader("fish.txt", 20.0);
    taiji = ObjLoader("taiji.txt", 130.0);
    logo = ObjLoader("logo.txt", 800.0, 800.0, 30.0);
    space01 = ObjLoader("space01.obj", 0.4);
    room = ObjLoader("tulou.txt", 2.0);
    bird.loadObjFile();
    whl.loadObjFile();
    tree1.loadObjFile();
    tree2.loadObjFile();
    treeN1.loadObjFile();
    treeN2.loadObjFile();
    taiji.loadObjFile();
    fish.loadObjFile();
    logo.loadObjFile();
    space01.loadObjFile();
    room.loadObjFile();
    
    for(int i=0;i<3;i++){
        spdSE.push_back(1);
    }
    for(int i=0;i<3;i++){
        nfSpd.push_back(0.01);
    }
    for(int i=0;i<3;i++){
        nwSpd.push_back(0.0003);
    }
    for(int i=0;i<3;i++){
        nbSpd.push_back(0.0005);
    }
    
    spring0 = Spring(1600);
    spring0.setup();
    NodeForFishes = Spring(150, -70);
    NodeForFishes.setup();
    NodeForWhl = Spring(7, -160);
    NodeForWhl.setup();
    NodeForBird = Spring(7);
    NodeForBird.setup();
    psz = 30;
    rainColor = ofColor(255, 255, 255, 230);
    for(int i=0;i<150;i++){
        treePos1.push_back( ofVec3f( ofRandom( -ofGetWidth()*2, ofGetWidth()*2), 0, ofRandom( -ofGetWidth()*2, ofGetWidth()*2) ) );
        treePos2.push_back( ofVec3f( ofRandom( -ofGetWidth()*2, ofGetWidth()*2), 0, ofRandom( -ofGetWidth()*2, ofGetWidth()*2) ) );
        treeRot1.push_back( ofRandom(0,360) );
        treeRot2.push_back( ofRandom(0,360) );
    }
    forestOpacity = 0.0;
    tarForestOpacity = 0.0;
    tulouOpacity = 0.0;
    tarTulouOpacity = 0.0;
}

