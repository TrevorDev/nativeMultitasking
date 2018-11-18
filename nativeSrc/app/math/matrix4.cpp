#pragma once

#include "quaternion.cpp";
#include "vector3.cpp";

class Matrix4{
    public:
    float m[4][4] = {
        1,0,0,0,
        0,1,0,0,
        0,0,1,0,
        0,0,0,1
    };
    Matrix4(){
    }
    
    
    void static FromValuesToRef(float m00, float m01, float m02, float m03, float m10, float m11, float m12, float m13, float m20, float m21, float m22, float m23, float m30, float m31, float m32, float m33, Matrix4& result){
            result.m[0][0] = m00;
            result.m[0][1] = m01;
            result.m[0][2] = m02;
            result.m[0][3] = m03;
            result.m[1][0] = m10;
            result.m[1][1] = m11;
            result.m[1][2] = m12;
            result.m[1][3] = m13;
            result.m[2][0] = m20;
            result.m[2][1] = m21;
            result.m[2][2] = m22;
            result.m[2][3] = m23;
            result.m[3][0] = m30;
            result.m[3][1] = m31;
            result.m[3][2] = m32;
            result.m[3][3] = m33;
    }
    void static ScaleToRef(Vector3& scale, Matrix4& result){
        FromValuesToRef(scale.x, 0.0, 0.0, 0.0,
                        0.0, scale.y, 0.0, 0.0,
                        0.0, 0.0, scale.z, 0.0,
                        0.0, 0.0, 0.0, 1.0,result);
    }
    void static RotationToRef(Quaternion& rotation, Matrix4& result){
        float xx = rotation.x * rotation.x;
        float yy = rotation.y * rotation.y;
        float zz = rotation.z * rotation.z;
        float xy = rotation.x * rotation.y;
        float zw = rotation.z * rotation.w;
        float zx = rotation.z * rotation.x;
        float yw = rotation.y * rotation.w;
        float yz = rotation.y * rotation.z;
        float xw = rotation.x * rotation.w;

        FromValuesToRef(
            1.0 - (2.0 * (yy + zz)), 2.0 * (xy + zw), 2.0 * (zx - yw), 0.0,
            2.0 * (xy - zw), 1.0 - (2.0 * (zz + xx)), 2.0 * (yz + xw), 0.0,
            2.0 * (zx + yw), 2.0 * (yz - xw), 1.0 - (2.0 * (yy + xx)), 0.0,
            0.0, 0.0, 0.0, 1.0, 
            result
        );
    }
    void static PositionToRef(Vector3& position, Matrix4& result){
        FromValuesToRef(1.0, 0.0, 0.0, 0.0,
                        0.0, 1.0, 0.0, 0.0,
                        0.0, 0.0, 1.0, 0.0,
                        position.x, position.y, position.z, 1.0,result);
    }
    void static MultiplyToRef(Matrix4& a, Matrix4& b, Matrix4& result){
        float a00=a.m[0][0];
        float a01=a.m[0][1];
        float a02=a.m[0][2];
        float a03=a.m[0][3];

        float a10=a.m[1][0];
        float a11=a.m[1][1];
        float a12=a.m[1][2];
        float a13=a.m[1][3];

        float a20=a.m[2][0];
        float a21=a.m[2][1];
        float a22=a.m[2][2];
        float a23=a.m[2][3];

        float a30=a.m[3][0];
        float a31=a.m[3][1];
        float a32=a.m[3][2];
        float a33=a.m[3][3];

        float b00=b.m[0][0];
        float b01=b.m[0][1];
        float b02=b.m[0][2];
        float b03=b.m[0][3];

        float b10=b.m[1][0];
        float b11=b.m[1][1];
        float b12=b.m[1][2];
        float b13=b.m[1][3];

        float b20=b.m[2][0];
        float b21=b.m[2][1];
        float b22=b.m[2][2];
        float b23=b.m[2][3];

        float b30=b.m[3][0];
        float b31=b.m[3][1];
        float b32=b.m[3][2];
        float b33=b.m[3][3];

        result.m[0][0] = a00*b00+a10*b01+a20*b02+a30*b03;
        result.m[0][1] = a01*b00+a11*b01+a21*b02+a31*b03;
        result.m[0][2] = a02*b00+a12*b01+a22*b02+a32*b03;
        result.m[0][3] = a03*b00+a13*b01+a23*b02+a33*b03;

        result.m[1][0] = a00*b10+a10*b11+a20*b12+a30*b13;
        result.m[1][1] = a01*b10+a11*b11+a21*b12+a31*b13;
        result.m[1][2] = a02*b10+a12*b11+a22*b12+a32*b13;
        result.m[1][3] = a03*b10+a13*b11+a23*b12+a33*b13;

        result.m[2][0] = a00*b20+a10*b21+a20*b22+a30*b23;
        result.m[2][1] = a01*b20+a11*b21+a21*b22+a31*b23;
        result.m[2][2] = a02*b20+a12*b21+a22*b22+a32*b23;
        result.m[2][3] = a03*b20+a13*b21+a23*b22+a33*b23;

        result.m[3][0] = a00*b30+a10*b31+a20*b32+a30*b33;
        result.m[3][1] = a01*b30+a11*b31+a21*b32+a31*b33;
        result.m[3][2] = a02*b30+a12*b31+a22*b32+a32*b33;
        result.m[3][3] = a03*b30+a13*b31+a23*b32+a33*b33;
    }
    void static ComposeToRef(Vector3& position, Quaternion& rotation, Vector3& scale, Matrix4& result){
        ScaleToRef(scale, TmpMatrix1);
        RotationToRef(rotation, TmpMatrix2);
        MultiplyToRef(TmpMatrix1, TmpMatrix2, result);
        result.m[3][0] = position.x;
        result.m[3][1] = position.y;
        result.m[3][2] = position.z;
    }

    private:
        static Matrix4 TmpMatrix1;
        static Matrix4 TmpMatrix2;
        static Matrix4 TmpMatrix3;
};

Matrix4 Matrix4::TmpMatrix1 = Matrix4();
Matrix4 Matrix4::TmpMatrix2 = Matrix4();
Matrix4 Matrix4::TmpMatrix3 = Matrix4();