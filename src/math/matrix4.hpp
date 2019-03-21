#pragma once

#include "quaternion.hpp"
#include "vector3.hpp"

class Matrix4{
    public:
    float m[4][4] = {
        {1,0,0,0},
        {0,1,0,0},
        {0,0,1,0},
        {0,0,0,1}
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
            1 - (2 * (yy + zz)), 2 * (xy + zw), 2 * (zx - yw), 0,
            2 * (xy - zw), 1 - (2 * (zz + xx)), 2 * (yz + xw), 0,
            2 * (zx + yw), 2 * (yz - xw), 1 - (2 * (yy + xx)), 0,
            0, 0, 0, 1, 
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

    void static InvertToRef(Matrix4& input, Matrix4& result){
        float m00 = input.m[0][0];
        float m01 = input.m[0][1];
        float m02 = input.m[0][2];
        float m03 = input.m[0][3];
        float m10 = input.m[1][0];
        float m11 = input.m[1][1];
        float m12 = input.m[1][2];
        float m13 = input.m[1][3];
        float m20 = input.m[2][0];
        float m21 = input.m[2][1];
        float m22 = input.m[2][2];
        float m23 = input.m[2][3];
        float m30 = input.m[3][0];
        float m31 = input.m[3][1];
        float m32 = input.m[3][2];
        float m33 = input.m[3][3];
        // https://en.wikipedia.org/wiki/Laplace_expansion
        // to compute the deterrminant of a 4x4 Matrix we compute the cofactors of any row or column,
        // then we multiply each Cofactor by its corresponding matrix value and sum them all to get the determinant
        // Cofactor(i, j) = sign(i,j) * det(Minor(i, j))
        // where
        //  - sign(i,j) = (i+j) % 2 === 0 ? 1 : -1
        //  - Minor(i, j) is the 3x3 matrix we get by removing row i and column j from current Matrix
        //
        // Here we do that for the 1st row.

        float det_22_33 = m22 * m33 - m32 * m23;
        float det_21_33 = m21 * m33 - m31 * m23;
        float det_21_32 = m21 * m32 - m31 * m22;
        float det_20_33 = m20 * m33 - m30 * m23;
        float det_20_32 = m20 * m32 - m22 * m30;
        float det_20_31 = m20 * m31 - m30 * m21;
        float cofact_00 = +(m11 * det_22_33 - m12 * det_21_33 + m13 * det_21_32);
        float cofact_01 = -(m10 * det_22_33 - m12 * det_20_33 + m13 * det_20_32);
        float cofact_02 = +(m10 * det_21_33 - m11 * det_20_33 + m13 * det_20_31);
        float cofact_03 = -(m10 * det_21_32 - m11 * det_20_32 + m12 * det_20_31);

        float det = m00 * cofact_00 + m01 * cofact_01 + m02 * cofact_02 + m03 * cofact_03;
        
        float det_12_33 = m12 * m33 - m32 * m13;
        float det_11_33 = m11 * m33 - m31 * m13;
        float det_11_32 = m11 * m32 - m31 * m12;
        float det_10_33 = m10 * m33 - m30 * m13;
        float det_10_32 = m10 * m32 - m30 * m12;
        float det_10_31 = m10 * m31 - m30 * m11;
        float det_12_23 = m12 * m23 - m22 * m13;
        float det_11_23 = m11 * m23 - m21 * m13;
        float det_11_22 = m11 * m22 - m21 * m12;
        float det_10_23 = m10 * m23 - m20 * m13;
        float det_10_22 = m10 * m22 - m20 * m12;
        float det_10_21 = m10 * m21 - m20 * m11;

        float cofact_10 = -(m01 * det_22_33 - m02 * det_21_33 + m03 * det_21_32);
        float cofact_11 = +(m00 * det_22_33 - m02 * det_20_33 + m03 * det_20_32);
        float cofact_12 = -(m00 * det_21_33 - m01 * det_20_33 + m03 * det_20_31);
        float cofact_13 = +(m00 * det_21_32 - m01 * det_20_32 + m02 * det_20_31);

        float cofact_20 = +(m01 * det_12_33 - m02 * det_11_33 + m03 * det_11_32);
        float cofact_21 = -(m00 * det_12_33 - m02 * det_10_33 + m03 * det_10_32);
        float cofact_22 = +(m00 * det_11_33 - m01 * det_10_33 + m03 * det_10_31);
        float cofact_23 = -(m00 * det_11_32 - m01 * det_10_32 + m02 * det_10_31);

        float cofact_30 = -(m01 * det_12_23 - m02 * det_11_23 + m03 * det_11_22);
        float cofact_31 = +(m00 * det_12_23 - m02 * det_10_23 + m03 * det_10_22);
        float cofact_32 = -(m00 * det_11_23 - m01 * det_10_23 + m03 * det_10_21);
        float cofact_33 = +(m00 * det_11_22 - m01 * det_10_22 + m02 * det_10_21);

        if(det == 0){
            jlog("bad det");
            return;
        }

        Matrix4::FromValuesToRef(
                cofact_00 / det, cofact_10 / det, cofact_20 / det, cofact_30 / det,
                cofact_01 / det, cofact_11 / det, cofact_21 / det, cofact_31 / det,
                cofact_02 / det, cofact_12 / det, cofact_22 / det, cofact_32 / det,
                cofact_03 / det, cofact_13 / det, cofact_23 / det, cofact_33 / det,
                result
        );
    }

    float determinant() {
        float m00 = m[0][0];
        float m01 = m[0][1];
        float m02 = m[0][2];
        float m03 = m[0][3];
        float m10 = m[1][0];
        float m11 = m[1][1];
        float m12 = m[1][2];
        float m13 = m[1][3];
        float m20 = m[2][0];
        float m21 = m[2][1];
        float m22 = m[2][2];
        float m23 = m[2][3];
        float m30 = m[3][0];
        float m31 = m[3][1];
        float m32 = m[3][2];
        float m33 = m[3][3];
        // https://en.wikipedia.org/wiki/Laplace_expansion
        // to compute the deterrminant of a 4x4 Matrix we compute the cofactors of any row or column,
        // then we multiply each Cofactor by its corresponding matrix value and sum them all to get the determinant
        // Cofactor(i, j) = sign(i,j) * det(Minor(i, j))
        // where
        //  - sign(i,j) = (i+j) % 2 === 0 ? 1 : -1
        //  - Minor(i, j) is the 3x3 matrix we get by removing row i and column j from current Matrix
        //
        // Here we do that for the 1st row.

        float det_22_33 = m22 * m33 - m32 * m23;
        float det_21_33 = m21 * m33 - m31 * m23;
        float det_21_32 = m21 * m32 - m31 * m22;
        float det_20_33 = m20 * m33 - m30 * m23;
        float det_20_32 = m20 * m32 - m22 * m30;
        float det_20_31 = m20 * m31 - m30 * m21;
        float cofact_00 = +(m11 * det_22_33 - m12 * det_21_33 + m13 * det_21_32);
        float cofact_01 = -(m10 * det_22_33 - m12 * det_20_33 + m13 * det_20_32);
        float cofact_02 = +(m10 * det_21_33 - m11 * det_20_33 + m13 * det_20_31);
        float cofact_03 = -(m10 * det_21_32 - m11 * det_20_32 + m12 * det_20_31);
        return m00 * cofact_00 + m01 * cofact_01 + m02 * cofact_02 + m03 * cofact_03;
    }

    private:
        static Matrix4 TmpMatrix1;
        static Matrix4 TmpMatrix2;
        static Matrix4 TmpMatrix3;
};

Matrix4 Matrix4::TmpMatrix1 = Matrix4();
Matrix4 Matrix4::TmpMatrix2 = Matrix4();
Matrix4 Matrix4::TmpMatrix3 = Matrix4();