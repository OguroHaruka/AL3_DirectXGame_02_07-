#include "Player.h"
#include "ImGuiManager.h"
#include "MyMath.h"
#include <cassert>
#include "Enemy.h"

void Player::Initialize(Model* model, uint32_t textureHandle,Vector3 playerPosition) {
	assert(model);
	model_ = model;
	textureHandle_ = textureHandle;
	worldTransform_.Initialize();
	input_ = Input::GetInstance();
	worldTransform_.scale_ = {1.0f, 1.0f, 1.0f};
	worldTransform_.rotation_ = {0.0f, 0.0f, 0.0f};
	worldTransform_.translation_ = {0.0f, 0.0f, 0.0f};
	worldTransform_.translation_ = Add(worldTransform_.translation_, playerPosition);
	worldTransform3DReticle_.Initialize();
	uint32_t textureReticle = TextureManager::Load("Reticle.png");
	sprite2DReticle_ =
	    Sprite::Create(textureReticle, {640.0f, 360.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.5f, 0.5f});
}

void Player::Rotate() {
	const float kRotSpeed = 0.02f;

	if (input_->PushKey(DIK_A)) {
		worldTransform_.rotation_.y -= kRotSpeed;
	} else if (input_->PushKey(DIK_D)) {
		worldTransform_.rotation_.y += kRotSpeed;
	}
}

void Player::Update(ViewProjection& viewProjection) {

	Vector3 move = {0.0f, 0.0f, 0.0f};

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

	const float kDistancePlayer3DReticle = 50.0f;
	Vector3 offset = {0, 0, 1.0f};
	offset = TransformNormal(offset, worldTransform_.matWorld_);
	offset = Normalize(offset);
	offset = Multiply(offset, worldTransform_.matWorld_);
	offset = Multiply(kDistancePlayer3DReticle, Normalize(offset));
	worldTransform3DReticle_.translation_ = Add(GetWorldPosition(), offset);
	worldTransform3DReticle_.UpdateMatrix();
	worldTransform3DReticle_.TransferMatrix();
	 worldTransform3DReticle_.UpdateMatrix();
	 worldTransform3DReticle_.TransferMatrix();

	 Vector3 positionReticle = {
	     worldTransform3DReticle_.matWorld_.m[3][0], worldTransform3DReticle_.matWorld_.m[3][1],
	     worldTransform3DReticle_.matWorld_.m[3][2]};
	 Matrix4x4 matViewport =
	     MakeViewPortMatrix(0, 0, WinApp::kWindowWidth, WinApp::kWindowHeight, 0, 1);
	 Matrix4x4 matViewProjectionViewport =
	     Multiply(viewProjection.matView, Multiply(viewProjection.matProjection,
	     matViewport));
	 positionReticle = Transform(positionReticle, matViewProjectionViewport);
	 sprite2DReticle_->SetPosition(Vector2(positionReticle.x, positionReticle.y));
	 
}

void Player::Attack() {
	if (input_->TriggerKey(DIK_SPACE)) {
		const float kBulletSpeed = 1.0f;
		Vector3 velocity(0, 0, kBulletSpeed);
		velocity = TransformNormal(velocity, worldTransform_.matWorld_);
		PlayerBullet* newBullet = new PlayerBullet();
		newBullet->Initialize(model_, GetWorldPosition(), velocity);
		bullets_.push_back(newBullet);
		velocity = Subtract(
		    {worldTransform3DReticle_.matWorld_.m[3][0], worldTransform3DReticle_.matWorld_.m[3][1],
		     worldTransform3DReticle_.matWorld_.m[3][2]},
		    GetWorldPosition());
		velocity = Multiply(kBulletSpeed, Normalize(velocity));
	}
}

Player::~Player() {
	for (PlayerBullet* bullet : bullets_) {
		delete bullet;
	}
	delete sprite2DReticle_;
}

void Player::Draw(ViewProjection& viewProjection) {
	model_->Draw(worldTransform_, viewProjection, textureHandle_);
	for (PlayerBullet* bullet : bullets_) {
		bullet->Draw(viewProjection);
	}
	//model_->Draw(worldTransform3DReticle_, viewProjection);
	
}

void Player::DrawUI() { sprite2DReticle_->Draw(); }

Vector3 Player::GetWorldPosition() {
	Vector3 worldPos;
	worldPos.x = worldTransform_.matWorld_.m[3][0];
	worldPos.y = worldTransform_.matWorld_.m[3][1];
	worldPos.z = worldTransform_.matWorld_.m[3][2];
	return worldPos;
}  

void Player::OnCollision() {}

void Player::SetParent(const WorldTransform* parent) { worldTransform_.parent_ = parent; }