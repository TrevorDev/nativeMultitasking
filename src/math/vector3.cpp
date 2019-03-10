#pragma once

class Vector3{
    public:
        float x = 0.0f;
        float y = 0.0f;
        float z = 0.0f;
        Vector3(float x,float y,float z){
            this->x = x;
            this->y = y;
            this->z = z;
        }
        Vector3(){
        }
};