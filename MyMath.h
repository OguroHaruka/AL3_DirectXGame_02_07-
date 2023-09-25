#pragma once
#include "Matrix4x4.h"
#include "Vector3.h"
#include <assert.h>
#include <math.h>
#define _USE_MATH_DEFINES

/// <summary>
/// 3次元ベクトル加算
/// </summary>
/// <param name="v1"></param>
/// <param name="v2"></param>
/// <returns></returns>
Vector3 Add(const Vector3& v1, const Vector3& v2);
Vector3 Subtract(const Vector3& v1, const Vector3& v2);
Vector3 Multiply(float scalar, const Vector3& v1);
Vector3 Normalize(const Vector3& v1);
Vector3 Multiply(Vector3 vector, Matrix4x4 matrix);
Matrix4x4 Multiply(const Matrix4x4& m1, const Matrix4x4& m2);
Matrix4x4 MakeRotateXMatrix(float radian);
Matrix4x4 MakeRotateYMatrix(float radian);
Matrix4x4 MakeRotateZMatrix(float radian);
Matrix4x4 MakeAffineMatrix(const Vector3& scale, Vector3& rotate, const Vector3& translate);
Vector3 TransformNormal(const Vector3 v, const Matrix4x4& m);
Matrix4x4 Inverse(const Matrix4x4& m);
Vector3 Transform(const Vector3& vector, const Matrix4x4& matrix);
Matrix4x4 MakeViewPortMatrix(
    float left, float top, float width, float height, float minDepth, float maxDepth);
