#pragma once
#include "Input.h"
#include "Model.h"
#include "PlayerBullet.h"
#include "WorldTransform.h"
#include <list>

class Player {
public:
	~Player();
	void Initialize(Model* model, uint32_t textureHandle);
	void Rotate();
	void Update();
	void Draw(ViewProjection& viewProjection);
	void Attack();
	Vector3 GetWorldPosition();
	void OnCollision();
	const std::list<PlayerBullet*>& GetBullets() const { return bullets_; }
	bool GetIsDead() { return isDead_; }

private:
	WorldTransform worldTransform_;
	Model* model_ = nullptr;
	uint32_t textureHandle_ = 0u;
	Player* player_ = nullptr;
	Input* input_ = nullptr;
	// PlayerBullet* bullet_ = nullptr;
	std::list<PlayerBullet*> bullets_;
	float HP = 5;
	bool isDead_ = false;
	
};