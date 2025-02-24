#pragma once
#include "ofMain.h"

struct ParticleState {
	glm::vec3 position;
	glm::vec3 velocity;
	glm::vec3 acceleration;
	glm::vec3 forces;
	float mass;
	float radius;
};

class Particle{
public:
	Particle();

	glm::vec3 position;
	glm::vec3 velocity;
	glm::vec3 acceleration;
	glm::vec3 forces;

	float mass;
	float radius;
	float damping;

	ofColor color;
;
	void integrate(float timestep, float framerate);
	void draw();
	void reset();
	void setVelocity(float x, float y, float z);
	void setMass(float m);
	void setRadius(float r);
	
	void setState(ParticleState state);
	ParticleState getState();
};