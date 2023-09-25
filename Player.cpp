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

	// 02_15の24ぺージ
	 XINPUT_STATE joyState;

	Vector2 spritePosition = sprite2DReticle_->GetPosition();

	if (Input::GetInstance()->GetJoystickState(0, joyState)) {
		spritePosition.x += (float)joyState.Gamepad.sThumbRX / SHRT_MAX * 7.0f;
		spritePosition.y -= (float)joyState.Gamepad.sThumbRY / SHRT_MAX * 7.0f;
		sprite2DReticle_->SetPosition(spritePosition);
	}
	//////////

	// 02_15の23ぺージ
	if (Input::GetInstance()->GetJoystickState(0, joyState)) {
		move.x += (float)joyState.Gamepad.sThumbLX / SHRT_MAX * kCharacterSpeed;
		move.y += (float)joyState.Gamepad.sThumbLY / SHRT_MAX * kCharacterSpeed;
	}
	///////////

	const float kMoveLimitX = 18.0f;
	const float kMoveLimitY = 10.0f;

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

	 Vector3 positionReticle = {
	     worldTransform3DReticle_.matWorld_.m[3][0], worldTransform3DReticle_.matWorld_.m[3][1],
	     worldTransform3DReticle_.matWorld_.m[3][2]};
	 Matrix4x4 matViewport =
	     MakeViewPortMatrix(0, 0, WinApp::kWindowWidth, WinApp::kWindowHeight, 0, 1);
	 Matrix4x4 matViewProjectionViewport =
	     Multiply(viewProjection.matView, Multiply(viewProjection.matProjection,
	     matViewport));
	 positionReticle = Transform(positionReticle, matViewProjectionViewport);
	 //sprite2DReticle_->SetPosition(Vector2(positionReticle.x, positionReticle.y));
	 
	 // 02_15の14ページ
	 //あとで消す
	 /* POINT mousePosition;
	  
	  GetCursorPos(&mousePosition);

	  HWND hwnd = WinApp::GetInstance()->GetHwnd();
	  ScreenToClient(hwnd, &mousePosition);
	 
	  sprite2DReticle_->SetPosition(Vector2((float)mousePosition.x, (float)mousePosition.y));*/
	 /////////////////

	// 02_15の15ページ
	  Matrix4x4 matVPV =
	      Multiply(Multiply(viewProjection.matView, viewProjection.matProjection), matViewport);

	  Matrix4x4 matInverseVPV = Inverse(matVPV);
	  //////////////////

	  // 02_15の16ページ
	  Vector3 posNear = {
	      (float)sprite2DReticle_->GetPosition().x, (float)sprite2DReticle_->GetPosition().y, 0};

	  Vector3 posFar = {
	      (float)sprite2DReticle_->GetPosition().x, (float)sprite2DReticle_->GetPosition().y, 1};

	  posNear = Transform(posNear, matInverseVPV);
	  posFar = Transform(posFar, matInverseVPV);
	  ///////////////////

	  // 02_15の17ページ
	  Vector3 mouseDirection = Subtract(posFar, posNear);
	  mouseDirection = Normalize(mouseDirection);

	  const float kDistanceTestObject = 100.0f;

	  worldTransform3DReticle_.translation_ =
	      Add(posNear, Multiply(kDistanceTestObject, mouseDirection));

	  worldTransform3DReticle_.UpdateMatrix();
	  worldTransform3DReticle_.TransferMatrix();
	  ////////////////////

	  // 02_15の18ページ
	  ImGui::Begin("Player");
	  //↓無視していい
	  //ImGui::Text("2DReeticle:(%f,%f)", sprite);
	  ImGui::Text("Near:(%+.2f,%+.2f,%+.2f)", posNear.x, posNear.y, posNear.z);
	  ImGui::Text("Far:(%+.2f,%+.2f,%+.2f)", posFar.x, posFar.y, posFar.z);
	  ImGui::Text(
	      "3Deticle:(%+.2f,%+.2f,%+.2f)", worldTransform3DReticle_.translation_.x,
	      worldTransform3DReticle_.translation_.y, worldTransform3DReticle_.translation_.z);
	  ImGui::End();
	  ////////////////////

	  worldTransform_.translation_ = Add(worldTransform_.translation_, move);
	  worldTransform_.UpdateMatrix();
	
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

	// 02_15の25ぺージ
	XINPUT_STATE joyState;

	if (!Input::GetInstance()->GetJoystickState(0, joyState)) {
		return;
	}

	
	if (joyState.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER) {
		
		const float kBulletSpeed = 1.0f;
		Vector3 velocity(0, 0, kBulletSpeed);

		
		velocity = TransformNormal(velocity, worldTransform_.matWorld_);

		velocity = Subtract(
		    {worldTransform3DReticle_.matWorld_.m[3][0], worldTransform3DReticle_.matWorld_.m[3][1],
		     worldTransform3DReticle_.matWorld_.m[3][2]},
		    GetWorldPosition());

		velocity = Multiply(kBulletSpeed, Normalize(velocity));
		
		PlayerBullet* newBullet = new PlayerBullet();
		newBullet->Initialize(model_, GetWorldPosition(), velocity);

		
		bullets_.push_back(newBullet);
	}
	////////////////////
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