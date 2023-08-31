#include "RailCamera.h"

void RailCamera::Initialize(WorldTransform worldTransform) { 
	worldTransform_ = worldTransform;

	viewProjection_.Initialize(); }

void RailCamera::Update() {

}