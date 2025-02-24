#include "Particle.h"

Particle::Particle() {
	position = glm::vec3(0, 100, 0);
	velocity = glm::vec3(0, 0, 0);
	acceleration = glm::vec3(0, 0, 0);
	forces = glm::vec3(0, 0, 0);
	mass = 1;
	radius = 2;
	damping = 0.99;
	color = ofColor::red;
}

void Particle::draw() {
	ofSetColor(color);
	ofDrawSphere(position, radius);
}

void Particle::setVelocity(float x, float y, float z) {
	velocity = glm::vec3(x, y, z);
}

void Particle::setMass(float m) {
	mass = m;
}

void Particle::setRadius(float r) {
	radius = r;
}

void Particle::reset() {
	position = glm::vec3(0, radius, 0);
	velocity = glm::vec3(0, 0, 0);
	acceleration = glm::vec3(0, 0, 0);
	forces = glm::vec3(0, 0, 0);
	mass = 1;
	radius = 2;
	damping = 0.99;
	color = ofColor::red;
}

ParticleState Particle::getState() {
	return {position, velocity, acceleration, forces, mass, radius};
}

void Particle::setState(ParticleState state) {
	this->position = state.position;
	this->velocity = state.velocity;
	this->acceleration = state.acceleration;
	this->forces = state.forces;
	this->mass = state.mass;
	this->radius = state.radius;
}

void Particle::integrate(float timestep, float framerate) {

	// timestep h
	float h = timestep;
	
	// update acceleration, velocity and position for next timestep
	// using Improved Euler
	acceleration = forces * (1.0 / mass) * framerate;

	glm::vec3 predicted_velocity = (velocity + (acceleration * h));
	position += (0.5 * (velocity + predicted_velocity) *  h) ;
	
	velocity =  predicted_velocity;

	// add damping
	//velocity *= damping;
	// clear forces on particle. They get re-added each step
	forces = glm::vec3(0, 0, 0);
}