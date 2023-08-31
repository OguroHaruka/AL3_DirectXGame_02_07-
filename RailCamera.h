#pragma once
#include "WorldTransform.h"
#include "ViewProjection.h"

class RailCamera {
public:
	void Initialize(WorldTransform worldTransform);
	void Update();

private:
	WorldTransform worldTransform_;
	ViewProjection viewProjection_;
};