﻿#pragma once
#include "Matrix4x4.h"
#include "Vector3.h"
#include <math.h>

/// <summary>
/// 3次元ベクトル加算
/// </summary>
/// <param name="v1"></param>
/// <param name="v2"></param>
/// <returns></returns>
Vector3 Add(const Vector3& v1, const Vector3& v2);
Matrix4x4 Multiply(Matrix4x4 m1, Matrix4x4 m2);
Matrix4x4 MakeRotateXMatrix(float radian);
Matrix4x4 MakeRotateYMatrix(float radian);
Matrix4x4 MakeRotateZMatrix(float radian);
Matrix4x4 MakeAffineMatrix(const Vector3& scale, Vector3& rotate, const Vector3& translate);
Vector3 TransformNormal(const Vector3 v, const Matrix4x4& m);
