#pragma once

#include "node.cpp"
#define _USE_MATH_DEFINES
#include <math.h>

class Camera : public Node {
    public:
    Matrix4 _viewMatrix;
    float nearClip = 0.01f;
    float farClip = 100.0f;
    float projectionAngleRad = M_PI / 4.0f;
    void computeViewMatrix(){
        Matrix4::InvertToRef(this->_worldMatrix, this->_viewMatrix);
    }
};