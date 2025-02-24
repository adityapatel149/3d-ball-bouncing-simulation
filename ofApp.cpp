#include "ofApp.h"
#include "ofxGui.h"

//--------------------------------------------------------------
void ofApp::setup(){
	ofSetBackgroundColor(ofColor(25, 25, 25));

	mainCam.setDistance(100);
	mainCam.setNearClip(.1);
	ofSetVerticalSync(true);

	// Setup GUI
gui.setup();
gui.add(gravity.setup("Gravity", 10, 0, 100));
gui.add(restitution.setup("Restitution", 1, 0, 1));
gui.add(air_resistance.setup("Air Resistance", 0.1, 0, 0.5));
gui.add(wind_coeff.setup("Wind Coefficient", 0.0, 0, 0.5));
gui.add(wind_x.setup("Wind X", 0, -100, 100));
gui.add(wind_y.setup("Wind Y", 0, -100, 100));
gui.add(wind_z.setup("Wind Z", 0, -100, 100));
gui.add(friction.setup("Friction", 0, 0, 1));
gui.add(ball_velocity_x.setup("Velocity X", 0, -1000, 1000));
gui.add(ball_velocity_y.setup("Velocity Y", 0, -1000, 1000));
gui.add(ball_velocity_z.setup("Velocity Z", 0, -1000, 1000));
gui.add(mass.setup("Mass", 1, 1, 10));
gui.add(radius.setup("Radius", 2, 1, 10));

bHide = false;
play = false;
collision = false;

// Setup light	
light.setup();
light.setAreaLight(5, 5);
light.setPosition(0, 125, 0);
light.setDiffuseColor(ofColor(255, 192, 128));



// Setup plane width, height, columns, rows
floor.set(100, 100, 20, 20);
floor.setPosition(0, 0, 0);
// Rotating plane only changes rendering not underlying mesh normals
//floor.rotate(-90, 1, 0, 0);
rotateObject(floor, -90, glm::vec3(1, 0, 0));

wall1.set(100, 100, 20, 20);
translateObject(wall1, glm::vec3(0, 50, -50));
rotateObject(wall1, 90, glm::vec3(0, 1, 0));

wall2.set(100, 100, 20, 20);
translateObject(wall2, glm::vec3(0, 50, -50));

wall3.set(100, 100, 20, 20);
translateObject(wall3, glm::vec3(0, 50, -50));
rotateObject(wall3, -90, glm::vec3(0, 1, 0));

wall4.set(100, 100, 20, 20);
translateObject(wall4, glm::vec3(0, 50, -50));
rotateObject(wall4, -180, glm::vec3(0, 1, 0));

minBounds = glm::vec3(-50, 0, -50);   // Minimum corner (bottom-left-back)
maxBounds = glm::vec3(50, 200, 50); // Maximum corner (top[-right-front)


boxplanes.push_back(&floor);
boxplanes.push_back(&wall1);
boxplanes.push_back(&wall2);
boxplanes.push_back(&wall3);
boxplanes.push_back(&wall4);



// Setup particle system
sys.add(ball);
sys.addForce(&g);
sys.addForce(&response);
sys.addForce(&air_res);
sys.addForce(&wind);

}

//--------------------------------------------------------------
void ofApp::update() {

	if (play) {
		
		// check collisions for all particles 
		for (int i = 0; i < sys.particles.size(); i++) {

			//check for no particles
			if (sys.particles.size() == 0) return;

			// check for 0 framerate
			framerate = ofGetFrameRate();
			framerate = 60;
			if (framerate < 1.0) return;

			// timestep h
			h = 1.0 / framerate;
			time_rem = h;
			timestep = time_rem;

			Particle& particle = sys.particles[i];
			float r = particle.radius;
			minBounds = glm::vec3(-50+r, r, -50+r);   // Minimum corner (bottom-left-back)
			maxBounds = glm::vec3(50-r, 200, 50-r);
			while (time_rem > 1e-6f) {
				ParticleState state = particle.getState();
				ParticleState new_state = sys.update(&particle, timestep, framerate);
				auto [collision, f, normals, dist] = checkCollisions(state, new_state);
				
				if (collision) {
					
					timestep = max(f * timestep, 1e-6f);  // Ensure timestep is not too small
					
					// update response from all planes that particle collided with
					for (int i = 0; i < normals.size(); i++){
						response.apply(&particle, normals[i]);
						new_state = sys.update(&particle, timestep, framerate);
					}	
					new_state.velocity *= 0.99;
				}
				
				time_rem -= timestep;	
				new_state.position = glm::clamp(new_state.position, minBounds, maxBounds);
				particle.setState(new_state);				
			}
			
		}
				
		
		// update values from sliders
		g.set(glm::vec3(0, -gravity, 0));
		response.set_restitution(restitution);
		response.set_friction(friction);
		air_res.set(air_resistance); 
		wind.set(wind_coeff, wind_x, wind_y, wind_z);		
	}
}

//--------------------------------------------------------------
void ofApp::draw(){

	if (!bHide) gui.draw();

	mainCam.begin();
	ofEnableLighting();
	light.enable();



	ofSetColor(ofColor::white);
	floor.getMesh().draw();	
	ofSetColor(ofColor::coral);
	wall1.getMesh().draw();
	ofSetColor(ofColor::yellow);
	wall2.getMesh().draw();
	ofSetColor(ofColor::blue);
	wall3.getMesh().draw();
	ofSetColor(ofColor::green);
	wall4.getMesh().draw();
	sys.draw();
	


	light.disable();
	ofDisableLighting();
	mainCam.end();
}

//--------------------------------------------------------------
tuple<bool, float, vector<glm::vec3>, vector<float>> ofApp::checkCollisions(ParticleState state, ParticleState new_state) {
	
	glm::vec3 normal, vertex;
	vector<glm::vec3> normals;
	vector<float> distances;
	float f = -1, d1, d2;
	bool flag = false;

	// check against all planes
	for (int j = 0; j < boxplanes.size(); j++) {

		ofPlanePrimitive* plane = boxplanes[j];
		normal = plane->getMesh().getNormal(0);
		vertex = plane->getMesh().getVertex(0);

		// d = (x-p) . n_hat --> distance between particle and plane
		d1 = glm::dot((state.position - vertex), normal) - state.radius;
		d2 = glm::dot((new_state.position - vertex), normal) - state.radius;		

		// if particle not on same side of plane, i.e, crosses the plane
		if (d1>=0 && d2<=0) {				
			f = d1 / (d1 - d2); 
			flag = true;
			normals.push_back(normal);
			distances.push_back(d1);
		}
	}
	return make_tuple(flag, f, normals, distances);
}


void ofApp::translateObject(of3dPrimitive& object, glm::vec3 translation) {
	
	// Translate the entire mesh (vertices)
	ofMesh& mesh = object.getMesh();
	
	for (int i = 0; i < mesh.getNumVertices(); i++) {
		// Translate all vertices 
		glm::vec3 vertex = mesh.getVertex(i);
		vertex += translation;
		mesh.setVertex(i, vertex);
	}	
}

void ofApp::rotateObject(of3dPrimitive& object, float degree, glm::vec3 rotationAxis) {
	
	// Rotate the entire mesh (vertices and normals)

	ofMesh& mesh = object.getMesh();
	ofQuaternion rotation;
	rotation.makeRotate(degree, rotationAxis);

	for (int i = 0; i < mesh.getNumVertices(); i++) {
		// Rotate all vertices 
		glm::vec3 vertex = mesh.getVertex(i);
		vertex = rotation * vertex;
		mesh.setVertex(i, vertex);

		//Rotate all normals
		glm::vec3 normal = mesh.getNormal(i);
		normal = rotation * normal;
		normal = glm::normalize(normal);  // Ensure the normal is unit length
		mesh.setNormal(i, normal);
	}	
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
	switch (key) {

		// 32 is int code for spacebar
		case (32): {
			play = !play;
			break;
		};
		case ('h'): {
			bHide = !bHide;
			break;
		};
		case ('r'): {
			sys.reset();
			break;
		};
		case (OF_KEY_RETURN): {
			Particle p;
			p.setMass(mass);
			p.setRadius(radius);
			p.setVelocity(ball_velocity_x,ball_velocity_y,ball_velocity_z);
			sys.add(p);
			break;
		};
		default: break;
	}
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
