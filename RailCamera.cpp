#include "RailCamera.h"
#include "MyMath.h"
#include "ImGuiManager.h"

void RailCamera::Initialize(Vector3& worldPos,Vector3& rotation) { 
	worldTransform_.Initialize();
	worldTransform_.translation_=worldPos;
	worldTransform_.rotation_ = rotation;
	viewProjection_.Initialize();
}

void RailCamera::Update() {

	Vector3 move = {0.0f, 0.0f, -0.02f};
	worldTransform_.translation_ = Add(worldTransform_.translation_, move);

	Vector3 rotate = {0.0f, 0.0f, 0.0f};
	worldTransform_.rotation_ = Add(worldTransform_.rotation_, rotate);

	worldTransform_.UpdateMatrix();
	viewProjection_.matView = Inverse(worldTransform_.matWorld_);
	
	ImGui::Begin("Camera");
	ImGui::SliderFloat3("translate", &worldTransform_.translation_.x, -100.0f, 100.0f);
	ImGui::SliderFloat3("rotate", &worldTransform_.rotation_.x, -10.0f, 10.0f);
	ImGui::End();

}