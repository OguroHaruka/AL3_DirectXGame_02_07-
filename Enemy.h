#pragma once
#include "Input.h"
#include "Model.h"
#include "WorldTransform.h"

class Enemy {
public:
	void Initialize(Model* model, uint32_t textureHandle);
	void Update();
	void Draw(ViewProjection& viewProjection);
	void Approach(Vector3 move);
	void Leave(Vector3 move);
	enum class Phase {
		Approach,
		Leave,
	};

private:
	WorldTransform worldTransform_;
	Model* model_ = nullptr;
	uint32_t textureHandle_ = 0u;
	Enemy* enemy_ = nullptr;
	Phase phase_ = Phase::Approach;
	float kCharacterSpeed;
};