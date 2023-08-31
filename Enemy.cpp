#include "Enemy.h"
#include "Player.h"
#include "MyMath.h"
#include <cassert>

Enemy::~Enemy() {
	for (EnemyBullet* bullet : bullets_) {
		delete bullet;
	}
}

void Enemy::Initialize(Model* model, uint32_t textureHandle) {
	assert(model);
	model_ = model;
	textureHandle_ = textureHandle;
	kCharacterSpeed = 0.2f;
	worldTransform_.scale_ = {1.0f, 1.0f, 1.0f};
	worldTransform_.rotation_ = {0.0f, 0.0f, 0.0f};
	worldTransform_.translation_ = {0.0f, 5.0f, 60.0f};
	Enemy::PhaseReset();
	worldTransform_.Initialize();
}

void Enemy::Update() {
	if (HP <= 0) {
		isDead_ = true;
	}
	
	Vector3 move = {0.0f, 0.0f, 0.0f};

	switch (phase_) {
	case Phase::Approach:
	default:
		if (moveFlagX == false) {
			worldTransform_.translation_.x -= kCharacterSpeed;
		} else if (moveFlagX == true) {
			worldTransform_.translation_.x += kCharacterSpeed;
		}
		if (worldTransform_.translation_.x <= -50.0f) {
			moveFlagX = true;
		} else if (worldTransform_.translation_.x >= 50.0f) {
			moveFlagX = false;
		}
		if (moveFlagY == false) {
			worldTransform_.translation_.y -= kCharacterSpeed;
		} else if (moveFlagY == true) {
			worldTransform_.translation_.y += kCharacterSpeed;
		}
		if (worldTransform_.translation_.y <= -10.0f) {
			moveFlagY = true;
		} else if (worldTransform_.translation_.y >= 10.0f) {
			moveFlagY = false;
		}
		Enemy::Approach(move);

		break;
	case Phase::Leave:
		Enemy::Leave(move);
	}

	worldTransform_.UpdateMatrix();


	for (EnemyBullet* bullet : bullets_) {
		bullet->Update();
	}
	bullets_.remove_if([](EnemyBullet* bullet) {
		if (bullet->IsDead()) {
			delete bullet;
			return true;
		}
		return false;
	});
}

void Enemy::Fire() {
	if (isDead_ == false) {

		assert(player_);

		const float kBulletSpeed = 1.0f;

		Vector3 A = player_->GetWorldPosition();
		Vector3 B = Enemy::GetWorldPosition();
		Vector3 C = Subtract(A, B);
		Vector3 vector = Normalize(C);
		vector = Multiply(kBulletSpeed, vector);

		Vector3 velocity(vector);
		velocity = TransformNormal(velocity, worldTransform_.matWorld_);

		EnemyBullet* newBullet = new EnemyBullet();
		newBullet->Initialize(model_, worldTransform_.translation_, velocity);
		bullets_.push_back(newBullet);
	}
}

void Enemy::Draw(ViewProjection& viewProjection) {
	if (isDead_ == false) {
		model_->Draw(worldTransform_, viewProjection, textureHandle_);
		for (EnemyBullet* bullet : bullets_) {
			bullet->Draw(viewProjection);
		}
	}
}

void Enemy::PhaseReset() { countdown_ = 30; }

void Enemy::Approach(Vector3 move) {
	
	worldTransform_.translation_ = Add(worldTransform_.translation_, move);

	countdown_--;
	if (countdown_ <= 0) {
		Enemy::Fire();
		countdown_ = kFireInterval;
	}
	

	if (worldTransform_.translation_.z < -10.0f) {
		phase_ = Phase::Leave;
	}
}
void Enemy::Leave(Vector3 move) {
	move.y += kCharacterSpeed;
	move.x -= kCharacterSpeed - 0.3f;
	worldTransform_.translation_ = Add(worldTransform_.translation_, move);
};

Vector3 Enemy::GetWorldPosition() {
	Vector3 worldPos;
	worldPos.x = worldTransform_.matWorld_.m[3][0];
	worldPos.y = worldTransform_.matWorld_.m[3][1];
	worldPos.z = worldTransform_.matWorld_.m[3][2];
	return worldPos;
}

void Enemy::OnCollision() { HP -= 1; }