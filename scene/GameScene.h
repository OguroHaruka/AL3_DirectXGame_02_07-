#pragma once

#include "Audio.h"
#include "DebugCamera.h"
#include "DirectXCommon.h"
#include "Enemy.h"
#include "RailCamera.h"
#include "Input.h"
#include "Model.h"
#include "Player.h"
#include "Skydome.h"
#include "SafeDelete.h"
#include "Sprite.h"
#include "ViewProjection.h"
#include "WorldTransform.h"
#include <sstream>

/// <summary>
/// ゲームシーン
/// </summary>
class GameScene {

public: // メンバ関数
	/// <summary>
	/// コンストクラタ
	/// </summary>
	GameScene();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~GameScene();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 毎フレーム処理
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	void CheckAllCollisions();

	void AddEnemyBullet(EnemyBullet* enemyBullet);

	void AddEnemy(Vector3 position);

	void LoadEnemyPopData();

	void UpdateEnemyPopCommands();

private: // メンバ変数
	DirectXCommon* dxCommon_ = nullptr;
	Input* input_ = nullptr;
	Audio* audio_ = nullptr;
	uint32_t textureHandle_ = 0;
	Model* model_ = nullptr;

	WorldTransform worldTransform_;
	ViewProjection viewProjection_;

	Player* player_ = nullptr;

	DebugCamera* debugCamera_ = nullptr;

	int isDebugCameraActive_;

	Enemy* enemy_ = nullptr;
	uint32_t EnemyTexture_ = 0;
	Model* Enemymodel_ = nullptr;

	Skydome* skydome_ = nullptr;
	Model* skydomeModel_ = nullptr;

	RailCamera* railCamera_ = nullptr;

	EnemyBullet* enemyBullet_ = nullptr;
	std::list<EnemyBullet*> enemyBullets_;
	std::list<Enemy*> enemys_;

	std::stringstream enemyPopCommands;

	int standFlag = false;
	int standTime = 0;

	Vector3 worldPos;
	Vector3 rotation;

	/// <summary>
	/// ゲームシーン用
	/// </summary>
};
