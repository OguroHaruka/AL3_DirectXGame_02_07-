﻿#pragma once
#include "Input.h"
#include "Model.h"
#include "WorldTransform.h"
#include"EnemyBullet.h"
#include <list>

class Player;

class GameScene;

class Enemy {
public:
	~Enemy();
	void Initialize(Model* model, uint32_t textureHandle, Vector3& position);
	void Update();
	void Draw(ViewProjection& viewProjection);
	void Approach(Vector3 move);
	void Leave(Vector3 move);
	void Fire();
	void PhaseReset();
	enum class Phase {
		Approach,
		Leave,
	};
	void OnCollision();
	//const std::list<EnemyBullet*>& GetBullets() const { return bullets_; }
	void SetPlayer(Player* player) { player_ = player; }
	Vector3 GetWorldPosition();
	void SetGameScene(GameScene* gameScene) { gameScene_ = gameScene; }

	bool IsDead() const { return isDead_; }

private:
	WorldTransform worldTransform_;
	Model* model_ = nullptr;
	uint32_t textureHandle_ = 0u;
	Enemy* enemy_ = nullptr;
	Phase phase_ = Phase::Approach;
	//std::list<EnemyBullet*> bullets_;
	static const int kFireInterval = 60;
	int32_t countdown_ = 0;
	float kCharacterSpeed;
	bool isDead_ = false;

	Player* player_ = nullptr;
	GameScene* gameScene_ = nullptr;
};