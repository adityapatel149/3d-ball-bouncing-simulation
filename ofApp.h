#pragma once

#include "ofMain.h"
#include "ofxGui.h"
#include "Particle.h"
#include "ParticleSystem.h"


class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);

		tuple<bool, float, vector<glm::vec3>, vector<float>> checkCollisions(ParticleState state, ParticleState new_state);
		glm::vec3 minBounds;   
		glm::vec3 maxBounds;
		void rotateObject(of3dPrimitive& object, float degree, glm::vec3 rotationAxis);
		void translateObject(of3dPrimitive& object, glm::vec3 translation);

		ofEasyCam mainCam;
		ofLight light;

		ParticleSystem sys;
		Particle ball;
		GravityForce g;
		ResponseForce response;
		AirResistance air_res;
		WindForce wind;
		

		bool bHide;
		bool play;
		bool collision;

		ofxPanel gui;
		ofxFloatSlider gravity;
		ofxFloatSlider restitution;
		ofxFloatSlider air_resistance;
		ofxFloatSlider wind_coeff;
		ofxFloatSlider wind_x;
		ofxFloatSlider wind_y;
		ofxFloatSlider wind_z;
		ofxFloatSlider ball_velocity_x;
		ofxFloatSlider ball_velocity_y;
		ofxFloatSlider ball_velocity_z;
		ofxFloatSlider mass;
		ofxFloatSlider radius;
		ofxFloatSlider friction;

		ofPlanePrimitive floor, wall1, wall2, wall3, wall4;
		vector<ofPlanePrimitive*> boxplanes;

		float time_rem, h, framerate, n, timestep;
};
