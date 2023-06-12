#include "WorldTransform.h"
#include "MyMath.h"
#include <Math.h>

void WorldTransform::UpdateMatrix() {
	matWorld_ = MakeAffineMatrix(scale_, rotation_, translation_);
	TransferMatrix();
}