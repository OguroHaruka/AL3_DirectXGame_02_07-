#include "GameScene.h"
#include "AxisIndicator.h"
#include "TextureManager.h"
#include <cassert>

GameScene::GameScene() {}

GameScene::~GameScene() {
	delete model_;
	delete player_;
	delete enemy_;
	delete skydome_;
	delete skydomeModel_;
	delete debugCamera_;
	delete gameClearSprite_;
	delete gameOverSprite_;
}

void GameScene::Initialize() {

	gameClearTexture_ = TextureManager::Load("GameClear.png");
	gameClearSprite_ = Sprite::Create(gameClearTexture_, {150, 200});

	gameOverTexture_ = TextureManager::Load("GameOver.png");
	gameOverSprite_ = Sprite::Create(gameOverTexture_, {150, 200});

	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();

	textureHandle_ = TextureManager::Load("uvChecker.png");

	EnemyTexture_ = TextureManager::Load("sample.png");
	enemy_ = new Enemy();

	worldTransform_.Initialize();
	viewProjection_.Initialize();
	model_ = Model::Create();

	player_ = new Player();

	player_->Initialize(model_, textureHandle_);

	Enemymodel_ = Model::Create();
	enemy_->Initialize(Enemymodel_, EnemyTexture_);
	enemy_->SetPlayer(player_);

	skydome_ = new Skydome();

	skydomeModel_ = Model::CreateFromOBJ("skydome", true);
	skydomeTexture_ = TextureManager::Load("uvChecker.png");
	skydome_->Initialize(skydomeModel_,skydomeTexture_);

	debugCamera_ = new DebugCamera(1280, 720);

	AxisIndicator::GetInstance()->SetVisible(true);
	AxisIndicator::GetInstance()->SetTargetViewProjection(&viewProjection_);
}

void GameScene::Update() {
	player_->Update();
	enemy_->Update();
	debugCamera_->Update();
	CheckAllCollisions();
#ifdef _DEBUG
	if (input_->TriggerKey(DIK_RETURN)) {
		isDebugCameraActive_ = true;
	} else if (input_->TriggerKey(DIK_BACKSPACE)) {
		isDebugCameraActive_ = false;
	}
#endif
	if (isDebugCameraActive_) {
		viewProjection_.matView = debugCamera_->GetViewProjection().matView;
		viewProjection_.matProjection = debugCamera_->GetViewProjection().matProjection;
		viewProjection_.TransferMatrix();
	} else {
		viewProjection_.UpdateMatrix();
	}
}

void GameScene::Draw() {

	// コマンドリストの取得
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

#pragma region 背景スプライト描画
	// 背景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに背景スプライトの描画処理を追加できる
	/// </summary>

	// スプライト描画後処理
	Sprite::PostDraw();
	// 深度バッファクリア
	dxCommon_->ClearDepthBuffer();
#pragma endregion

#pragma region 3Dオブジェクト描画
	// 3Dオブジェクト描画前処理
	Model::PreDraw(commandList);

	/// <summary>
	/// ここに3Dオブジェクトの描画処理を追加できる

	player_->Draw(viewProjection_);
	
	enemy_->Draw(viewProjection_);
	skydome_->Draw(viewProjection_);

	/// </summary>

	// 3Dオブジェクト描画後処理
	Model::PostDraw();
#pragma endregion

#pragma region 前景スプライト描画
	// 前景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに前景スプライトの描画処理を追加できる
	/// </summary>

	if (enemy_->GetIsDead() == true) {
		gameClearSprite_->Draw();
	}

	if (player_->GetIsDead() == true) {
		gameOverSprite_->Draw();
	}

	// スプライト描画後処理
	Sprite::PostDraw();

#pragma endregion
}

void GameScene::CheckAllCollisions() {
	Vector3 posA, posB, posC, posD;
	const std::list<PlayerBullet*>& playerBullets = player_->GetBullets();
	const std::list<EnemyBullet*>& enemyBullets = enemy_->GetBullets();
	const float PLAYER_R = 1.0f;
	const float ENEMY_R = 1.0f;
	const float BULLET_R = 1.0f;

	#pragma region 自キャラと敵弾の当たり判定
	posA = player_->GetWorldPosition();
	for (EnemyBullet* bullet: enemyBullets) {
		posB = bullet->GetWorldPosition();
		float P = (posB.x - posA.x) * (posB.x - posA.x) +
			      (posB.y - posA.y) * (posB.y - posA.y) +
		          (posB.z - posA.z) * (posB.z - posA.z);
		float R = (PLAYER_R + ENEMY_R) * (PLAYER_R + ENEMY_R);
		if (P <= R) {
			player_->OnCollision();
			bullet->OnCollision();
		}
	}
	#pragma endregion


	#pragma region 自弾と敵キャラの当たり判定
	posC = enemy_->GetWorldPosition();
	for (PlayerBullet* bullet : playerBullets) {
		posB = bullet->GetWorldPosition();
		float P = (posB.x - posC.x) * (posB.x - posC.x) +
			      (posB.y - posC.y) * (posB.y - posC.y) +
		          (posB.z - posC.z) * (posB.z - posC.z);
		float R = (BULLET_R + ENEMY_R) * (BULLET_R + ENEMY_R);
		if (P <= R) {
			enemy_->OnCollision();
			bullet->OnCollision();
		}
	}
    #pragma endregion


	#pragma region 自弾と敵弾の当たり判定
	for (PlayerBullet* playerBullet : playerBullets) {
		for (EnemyBullet* enemyBullet : enemyBullets) {
			posB = playerBullet->GetWorldPosition();
			posD = enemyBullet->GetWorldPosition();
			float P = (posB.x - posD.x) * (posB.x - posD.x) +
			          (posB.y - posD.y) * (posB.y - posD.y) + 
				      (posB.z - posD.z) * (posB.z - posD.z);
			float R = (BULLET_R + BULLET_R) * (BULLET_R + BULLET_R);
			if (P <= R) {
				playerBullet->OnCollision();
				enemyBullet->OnCollision();
			}
		}
	}
    #pragma endregion
}
