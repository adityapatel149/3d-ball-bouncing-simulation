#include "ParticleSystem.h"


// Particle System constructor and methods
void ParticleSystem::add(const Particle& p) {
    particles.push_back(p);
}

void ParticleSystem::addForce(ParticleForce* f) {
    forces.push_back(f);
}

void ParticleSystem::remove(int i) {
    particles.erase(particles.begin() + i);
}

void ParticleSystem::reset() {

    // Reset system forces
    for (int i = 0; i < forces.size(); i++) {
        if (forces[i]->applyOnce) {
            forces[i]->applied = true;
        }
        else {
            forces[i]->applied = false;
        }        
    }

    // Reset particles
    for (int i = 0; i < particles.size(); i++) {
        particles[i].reset();
    }
}

ParticleState ParticleSystem::update(Particle* particle, float timestep, float framerate) {
    
    // update all forces on particle    
    for (int i = 0; i < forces.size(); i++) {
        if (!forces[i]->applied) {
            forces[i]->updateForce(particle);

            // update forces only applied once to "applied"
            if (forces[i]->applyOnce) {
                forces[i]->applied = true;
            }
        }
    }
   
    // integrate particle
    particle->integrate(timestep, framerate);

    return particle->getState();   
}

void ParticleSystem::draw() {
    for (int i = 0; i < particles.size(); i++) {
        particles[i].draw();
    }
}

// GravityForce constructor and methods
GravityForce::GravityForce(const glm::vec3& g) : gravity(g) {}

GravityForce::GravityForce() : gravity(0, 0, 0) {}

void GravityForce::set(const glm::vec3& g) { gravity = g; }

void GravityForce::updateForce(Particle* particle) {
    particle->forces += gravity * particle->mass;
}


// AirResistance constructor and methods
AirResistance::AirResistance() : d(0) {}

AirResistance::AirResistance(const float d) { this->d = d; }

void AirResistance::set(const float d) { this->d = d; }

void AirResistance::updateForce(Particle* particle) {
    float speed = glm::length(particle->velocity);

    if (speed > 1e-6f) { // Avoid division by zero
        glm::vec3 res = -d * 0.1 * speed * glm::normalize(particle->velocity);        
        particle->forces += res;
    }
}


// WindForce constructor and methods
WindForce::WindForce() : v(0, 0, 0), d(0) {}

WindForce::WindForce(const float d, float x, float y, float z) {
    this->d = d;
    this->v = glm::vec3(x,y,z);
}
void WindForce::set(const float d, float x, float y, float z) {
    this->d = d;
    this->v = glm::vec3(x,y,z);
}

void WindForce::updateForce(Particle* particle) {
    glm::vec3 v_relative = v - particle->velocity;
    float speed = glm::length(v_relative);
    if (speed > 1e-6f) { // Avoid division by zero
        glm::vec3 res = d  * speed * glm::normalize(v_relative);
        particle->forces += res;
    }
}


// ResponseForce constructor and methods

ResponseForce::ResponseForce() {
    applyOnce = true;
    applied = true;
    vn = glm::vec3(0, 0, 0);
    vt = glm::vec3(0, 0, 0);
    cr = 1;
    cf = 0;
}

void ResponseForce::set_restitution(const float cr) { this->cr = cr; }
void ResponseForce::set_friction(const float cf) { this->cf = cf; }

void ResponseForce::apply(Particle* particle, glm::vec3 normal) { 
    applied = false;
    glm::vec3 vn_prev = glm::dot(particle->velocity, normal) * normal;
    vn = -cr * vn_prev;
    vt = (1 - cf) * (particle->velocity - vn_prev);
}

void ResponseForce::updateForce(Particle* particle) {
    particle->velocity = vn + vt;
}

