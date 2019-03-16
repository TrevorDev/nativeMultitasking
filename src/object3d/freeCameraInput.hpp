#pragma once

#include "../math/quaternion.hpp"
#include "../math/vector3.hpp"
#include "../math/matrix4.hpp"
#include "../object3d/camera.hpp"
#include "../windowManager.hpp"

class FreeCameraInput {
    Camera& camera;
    WindowManager& windowManager;
    float camRotX = 0.0f;
    float camRotY = 0.0f;
    float spd = 0.01f;
    public:
    FreeCameraInput(Camera& cam, WindowManager& wm): camera(cam), windowManager(wm) {


            
    }
    void update(){
        if(windowManager.keys[262]){
            // Right
            camera.position.x += spd;
        }
        if(windowManager.keys[263]){
            // Left
            camera.position.x -= spd;
        }
        if(windowManager.keys[264]){
            // Down
            camera.position.z += spd;
        }
        if(windowManager.keys[265]){
            // Up
            camera.position.z -= spd;
        }

        if(windowManager.mouseDown){
            camRotY += (float)(-windowManager.lastCursorPosDifX/1000.0);
            camRotX += (float)(-windowManager.lastCursorPosDifY/1000.0);
        }

        Quaternion::FromEuler(camRotX,camRotY,0, camera.rotation);

        camera.computeWorldMatrix();
        camera.computeViewMatrix();
    }
    private:
    
};