#pragma once
#include "Input.h"
#include "Model.h"
#include "PlayerBullet.h"
#include "WorldTransform.h"
#include <list>

class Player {
public:
	~Player();
	void Initialize(Model* model, uint32_t textureHandle,Vector3 playerPosition);
	void Rotate();
	void Update();
	void Draw(ViewProjection& viewProjection);
	void Attack();
	void SetParent(const WorldTransform* parent);
	Vector3 GetWorldPosition();
	void OnCollision();
	const std::list<PlayerBullet*>& GetBullets() const { return bullets_; }

private:
	WorldTransform worldTransform_;
	Model* model_ = nullptr;
	uint32_t textureHandle_ = 0u;
	Player* player_ = nullptr;
	Input* input_ = nullptr;
	// PlayerBullet* bullet_ = nullptr;
	std::list<PlayerBullet*> bullets_;

	
};