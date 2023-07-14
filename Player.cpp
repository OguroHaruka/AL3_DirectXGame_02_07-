#include "Player.h"
#include "ImGuiManager.h"
#include "MyMath.h"
#include <cassert>
#include "Enemy.h"

void Player::Initialize(Model* model, uint32_t textureHandle) {
	assert(model);
	model_ = model;
	textureHandle_ = textureHandle;
	input_ = Input::GetInstance();
	worldTransform_.scale_ = {1.0f, 1.0f, 1.0f};
	worldTransform_.rotation_ = {0.0f, 0.0f, 0.0f};
	worldTransform_.translation_ = {0.0f, 0.0f, 0.0f};
	worldTransform_.Initialize();
}

void Player::Rotate() {
	const float kRotSpeed = 0.02f;

	if (input_->PushKey(DIK_A)) {
		worldTransform_.rotation_.y -= kRotSpeed;
	} else if (input_->PushKey(DIK_D)) {
		worldTransform_.rotation_.y += kRotSpeed;
	}
}

void Player::Update() {

	Vector3 move = {0.0f, 0.0f, 0.0f};

	// Matrix4x4 matScale;
	// matScale.m[0][0] = worldTransform_.scale_.x;
	// matScale.m[1][1] = worldTransform_.scale_.y;
	// matScale.m[2][2] = worldTransform_.scale_.z;
	// matScale.m[3][3] = 1;

	// Matrix4x4 matRotX;
	// matRotX.m[0][0] = 1;
	// matRotX.m[1][1] = cosf(worldTransform_.rotation_.x);
	// matRotX.m[2][1] = -sinf(worldTransform_.rotation_.x);
	// matRotX.m[1][2] = sinf(worldTransform_.rotation_.x);
	// matRotX.m[2][2] = cosf(worldTransform_.rotation_.x);
	// matRotX.m[3][3] = 1;

	// Matrix4x4 matRotY;
	// matRotY.m[0][0] = cosf(worldTransform_.rotation_.y);
	// matRotY.m[1][1] = 1;
	// matRotY.m[0][2] = -sinf(worldTransform_.rotation_.y);
	// matRotY.m[2][0] = sinf(worldTransform_.rotation_.y);
	// matRotY.m[2][2] = cosf(worldTransform_.rotation_.y);
	// matRotY.m[3][3] = 1;

	// Matrix4x4 matRotZ;
	// matRotZ.m[0][0] = cosf(worldTransform_.rotation_.z);
	// matRotZ.m[1][0] = sinf(worldTransform_.rotation_.z);
	// matRotZ.m[0][1] = -sinf(worldTransform_.rotation_.z);
	// matRotZ.m[1][1] = cosf(worldTransform_.rotation_.z);
	// matRotZ.m[2][2] = 1;
	// matRotZ.m[3][3] = 1;

	// Matrix4x4 matTrans;
	// matTrans.m[0][0] = 1;
	// matTrans.m[1][1] = 1;
	// matTrans.m[2][2] = 1;
	// matTrans.m[3][3] = 1;
	// matTrans.m[3][0] = worldTransform_.translation_.x;
	// matTrans.m[3][1] = worldTransform_.translation_.y;
	// matTrans.m[3][2] = worldTransform_.translation_.z;

	// worldTransform_.TransferMatrix();

	// void UpdateMatrix();

	// Matrix4x4 a = Multiply(matRotZ, matRotX);
	// Matrix4x4 b = Multiply(a, matRotY);
	// Matrix4x4 matRot = b;

	// Matrix4x4 c = Multiply(matScale, matRot);
	// Matrix4x4 d = Multiply(c, matTrans);
	// worldTransform_.matWorld_ = d;

	// worldTransform_.matWorld_ = MakeAffineMatrix(
	//     worldTransform_.scale_, worldTransform_.rotation_, worldTransform_.translation_);


	const float kCharacterSpeed = 0.2f;

	if (input_->PushKey(DIK_LEFT)) {
		move.x -= kCharacterSpeed;
	} else if (input_->PushKey(DIK_RIGHT)) {
		move.x += kCharacterSpeed;
	} else if (input_->PushKey(DIK_UP)) {
		move.y += kCharacterSpeed;
	} else if (input_->PushKey(DIK_DOWN)) {
		move.y -= kCharacterSpeed;
	}

	const float kMoveLimitX = 33.0f;
	const float kMoveLimitY = 18.0f;

	worldTransform_.translation_.x = max(worldTransform_.translation_.x, -kMoveLimitX);
	worldTransform_.translation_.x = min(worldTransform_.translation_.x, +kMoveLimitX);
	worldTransform_.translation_.y = max(worldTransform_.translation_.y, -kMoveLimitY);
	worldTransform_.translation_.y = min(worldTransform_.translation_.y, +kMoveLimitY);

	worldTransform_.translation_ = Add(worldTransform_.translation_, move);

	worldTransform_.UpdateMatrix();

	Player::Rotate();
	Player::Attack();

	for (PlayerBullet* bullet : bullets_) {
		bullet->Update();
	}

	bullets_.remove_if([](PlayerBullet* bullet) {
		if (bullet->IsDead()) {
			delete bullet;
			return true;
		}
		return false;
	});
	ImGui::Begin("Debug");
	float playerPos[] = {
	    worldTransform_.translation_.x, worldTransform_.translation_.y,
	    worldTransform_.translation_.z};
	ImGui::SliderFloat3("PlayerPos", playerPos, -30.0f, 30.0f);
	worldTransform_.translation_.x = playerPos[0];
	worldTransform_.translation_.y = playerPos[1];
	worldTransform_.translation_.z = playerPos[2];
	ImGui::End();
}

void Player::Attack() {
	if (input_->TriggerKey(DIK_SPACE)) {
		const float kBulletSpeed = 1.0f;
		Vector3 velocity(0, 0, kBulletSpeed);
		velocity = TransformNormal(velocity, worldTransform_.matWorld_);
		PlayerBullet* newBullet = new PlayerBullet();
		newBullet->Initialize(model_, worldTransform_.translation_, velocity);
		bullets_.push_back(newBullet);
	}
}

Player::~Player() {
	for (PlayerBullet* bullet : bullets_) {
		delete bullet;
	}
}

void Player::Draw(ViewProjection& viewProjection) {
	model_->Draw(worldTransform_, viewProjection, textureHandle_);
	for (PlayerBullet* bullet : bullets_) {
		bullet->Draw(viewProjection);
	}
}

Vector3 Player::GetWorldPosition() {
	Vector3 worldPos;
	worldPos.x = worldTransform_.matWorld_.m[3][0];
	worldPos.y = worldTransform_.matWorld_.m[3][1];
	worldPos.z = worldTransform_.matWorld_.m[3][2];
	return worldPos;
}  

void Player::OnCollision() {}