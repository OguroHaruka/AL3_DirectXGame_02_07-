#pragma once
#include "Input.h"
#include "Model.h"
#include "PlayerBullet.h"
#include "WorldTransform.h"
#include "MyMath.h"
#include <Sprite.h>
#include <list>

class Player {
public:
	~Player();
	void Initialize(Model* model, uint32_t textureHandle,Vector3 playerPosition);
	void Rotate();
	void Update(ViewProjection& viewProjection);
	void Draw(ViewProjection& viewProjection);
	void Attack();
	void SetParent(const WorldTransform* parent);
	Vector3 GetWorldPosition();
	void OnCollision();
	const std::list<PlayerBullet*>& GetBullets() const { return bullets_; }
	// 02_14の32ページ
	void DrawUI();
	///////////

private:
	WorldTransform worldTransform_;
	ViewProjection viewProjection_;
	Model* model_ = nullptr;
	uint32_t textureHandle_ = 0u;
	Player* player_ = nullptr;
	Input* input_ = nullptr;
	// PlayerBullet* bullet_ = nullptr;
	std::list<PlayerBullet*> bullets_;
	WorldTransform worldTransform3DReticle_;
	// 02_14の28ページ
	Sprite* sprite2DReticle_ = nullptr;
	//////////
};