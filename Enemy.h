#pragma once
#include "Input.h"
#include "Model.h"
#include "WorldTransform.h"
#include"EnemyBullet.h"
#include <list>

class Enemy {
public:
	~Enemy();
	void Initialize(Model* model, uint32_t textureHandle);
	void Update();
	void Draw(ViewProjection& viewProjection);
	void Approach(Vector3 move);
	void Leave(Vector3 move);
	void Attack();
	void PhaseReset();
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
	std::list<EnemyBullet*> bullets_;
	static const int kFireInterval = 60;
	int32_t countdown_ = 0;
	float kCharacterSpeed;
};