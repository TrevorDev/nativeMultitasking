#pragma once

#include "../math/quaternion.cpp";
#include "../math/vector3.cpp";
#include "../math/matrix4.cpp";

class Node {
    public:
    Vector3 scale = Vector3(1,1,1);
    Vector3 position;
    Quaternion rotation;
    Node(){
        
    }

    void computeWorldMatrix(){
        Matrix4::ComposeToRef(position, rotation, scale, _worldMatrix);
    }
    Matrix4 _worldMatrix;
    private:
    
};