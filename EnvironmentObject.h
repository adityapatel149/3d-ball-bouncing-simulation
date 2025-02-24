#pragma once
#include "ofMain.h"

class EnvironmentObject {
	vector<float*> prev_d;
public:
	EnvironmentObject(of3dPrimitive primitive);
	void rotateObject(of3dPrimitive& object, float degree, glm::vec3 rotationAxis);
};