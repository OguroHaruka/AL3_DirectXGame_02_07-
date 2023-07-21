#include "Skydome.h"

void Skydome::Initialize(Model* model,uint32_t textureHandle) {
	model_ = model;
	textureHandle_ = textureHandle;
	worldTransform_.scale_ = {1.0f, 1.0f, 1.0f};
	worldTransform_.rotation_ = {0.0f, 0.0f, 0.0f};
	worldTransform_.translation_ = {0.0f, 0.0f, 0.0f};
	worldTransform_.Initialize();
}

void Skydome::Update() {}

void Skydome::Draw(ViewProjection& viewProjection) {
	model_->Draw(worldTransform_, viewProjection,textureHandle_);
}