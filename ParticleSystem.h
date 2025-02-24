#pragma once
#include "ofMain.h"
#include "Particle.h"

class ParticleForce {
public:
    bool applyOnce = false;
    bool applied = false;
    virtual void updateForce(Particle*) = 0;
};

class ParticleSystem {
public:
    void add(const Particle&);
    void addForce(ParticleForce*);
    void remove(int);
    ParticleState update(Particle* , float, float );
    void reset();
    void draw();

    std::vector<Particle> particles;
    std::vector<ParticleForce*> forces;
};

// Force classes

class GravityForce : public ParticleForce {
    glm::vec3 gravity;
public:
    GravityForce(const glm::vec3& g);
    GravityForce();
    void set(const glm::vec3& g);
    void updateForce(Particle* particle) override;
};

class AirResistance : public ParticleForce {
    float d;
    
public:
    AirResistance();
    AirResistance(const float d);
    void set(const float d);
    void updateForce(Particle* particle) override;
};

class WindForce : public ParticleForce {
    glm::vec3 v;
    float d;
public:
    WindForce();
    WindForce(const float d, float x, float y, float z);
    void set(const float d, float x, float y, float z);
    void updateForce(Particle* particle) override;
};


class ResponseForce : public ParticleForce {
    glm::vec3 vn;
    glm::vec3 vt;
    float cr;
    float cf;
public:
    ResponseForce();
    void ResponseForce::set_restitution(const float cr);
    void ResponseForce::set_friction(const float cf);
    void apply(Particle* particle, glm::vec3 normal);
    void updateForce(Particle* particle) override;
};
