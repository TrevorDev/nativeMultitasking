#pragma once

class Quaternion{
    public:
        float x = 0;
        float y = 0;
        float z = 0;
        float w = 1;
    void static FromEuler(float x, float y, float z, Quaternion& result){
        // Produces a quaternion from Euler angles in the z-y-x orientation (Tait-Bryan angles)
        float halfRoll = z * 0.5;
        float halfPitch = x * 0.5;
        float halfYaw = y * 0.5;
        
        float sinRoll = std::sin(halfRoll);
        float cosRoll = std::cos(halfRoll);
        float sinPitch = std::sin(halfPitch);
        float cosPitch = std::cos(halfPitch);
        float sinYaw = std::sin(halfYaw);
        float cosYaw = std::cos(halfYaw);

        result.x = (cosYaw * sinPitch * cosRoll) + (sinYaw * cosPitch * sinRoll);
        result.y = (sinYaw * cosPitch * cosRoll) - (cosYaw * sinPitch * sinRoll);
        result.z = (cosYaw * cosPitch * sinRoll) - (sinYaw * sinPitch * cosRoll);
        result.w = (cosYaw * cosPitch * cosRoll) + (sinYaw * sinPitch * sinRoll);
    }
};