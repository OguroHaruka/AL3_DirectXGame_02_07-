#include "Enemy.h"
#include "MyMath.h"
#include <cassert>

void Enemy::Initialize(Model* model, uint32_t textureHandle) {
	assert(model);
	model_ = model;
	textureHandle_ = textureHandle;
	kCharacterSpeed = 0.2f;
	worldTransform_.scale_ = {1.0f, 1.0f, 1.0f};
	worldTransform_.rotation_ = {0.0f, 0.0f, 0.0f};
	worldTransform_.translation_ = {0.0f, 2.0f, 20.0f};
	worldTransform_.Initialize();
}

void Enemy::Update() {
	Vector3 move = {0.0f, 0.0f, 0.0f};
	switch (phase_) {
	case Phase::Approach:
	default:
		Enemy::Approach(move);

		break;
	case Phase::Leave:
		Enemy::Leave(move);
	}

	worldTransform_.UpdateMatrix();
}

void Enemy::Draw(ViewProjection& viewProjection) {
	model_->Draw(worldTransform_, viewProjection, textureHandle_);
}

void Enemy::Approach(Vector3 move) {
	move.z -= kCharacterSpeed;
	worldTransform_.translation_ = Add(worldTransform_.translation_, move);

	if (worldTransform_.translation_.z < 0.0f) {
		phase_ = Phase::Leave;
	}
}
void Enemy::Leave(Vector3 move) {
	move.y += kCharacterSpeed;
	move.x -= kCharacterSpeed + 0.3f;
	worldTransform_.translation_ = Add(worldTransform_.translation_, move);
};