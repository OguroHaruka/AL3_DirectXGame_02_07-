#include "GameScene.h"
#include "AxisIndicator.h"
#include "TextureManager.h"
#include <fstream>
#include <cassert>

GameScene::GameScene() {}

GameScene::~GameScene() {
	delete model_;
	delete player_;
	delete enemy_;
	delete skydome_;
	delete skydomeModel_;
	delete debugCamera_;
	delete railCamera_;
	for (EnemyBullet* bullet : enemyBullets_) {
		delete bullet;
	}

	for (Enemy* enemy : enemys_) {
		delete enemy;
	}
}

void GameScene::Initialize() {

	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();

	textureHandle_ = TextureManager::Load("uvChecker.png");

	EnemyTexture_ = TextureManager::Load("sample.png");
	enemy_ = new Enemy();

	worldTransform_.Initialize();
	viewProjection_.Initialize();
	model_ = Model::Create();

	railCamera_ = new RailCamera();

	railCamera_->Initialize(worldPos, rotation);

	player_ = new Player();
	Vector3 playerPosition(0, 0, 30.0f);
	player_->Initialize(model_, textureHandle_,playerPosition);
	player_->SetParent(&railCamera_->GetWorldTransform());

	Enemymodel_ = Model::Create();

	skydome_ = new Skydome();

	skydomeModel_ = Model::CreateFromOBJ("skydome", true);
	skydome_->Initialize(skydomeModel_);

	debugCamera_ = new DebugCamera(1280, 720);

	LoadEnemyPopData();

	// 02_14の31ページ
	TextureManager::Load("Reticle.png");
	////////////

	AxisIndicator::GetInstance()->SetVisible(true);
	AxisIndicator::GetInstance()->SetTargetViewProjection(&viewProjection_);
	AxisIndicator::GetInstance()->SetTargetViewProjection(&viewProjection_);
}


void GameScene::Update() {
	railCamera_->Update();
	player_->Update(viewProjection_);
	debugCamera_->Update();
	CheckAllCollisions();
	
	UpdateEnemyPopCommands();

	for (Enemy* enemy : enemys_) {
		enemy->Update();
	}
	for (EnemyBullet* bullet : enemyBullets_) {
		bullet->Update();
	}
	enemyBullets_.remove_if([](EnemyBullet* bullet) {
		if (bullet->IsDead()) {
			delete bullet;
			return true;
		}
		return false;
	});
	enemys_.remove_if([](Enemy* enemy) {
		if (enemy->IsDead()) {
			delete enemy;
			return true;
		}
		return false;
	});
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
		viewProjection_.matView = railCamera_->GetViewProjection().matView;
		viewProjection_.matProjection = railCamera_->GetViewProjection().matProjection;
		viewProjection_.TransferMatrix();
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
	skydome_->Draw(viewProjection_);
	for (Enemy* enemy : enemys_) {
		enemy->Draw(viewProjection_);
	}
	for (EnemyBullet* bullet : enemyBullets_) {
		bullet->Draw(viewProjection_);
	}
	/// </summary>

	// 3Dオブジェクト描画後処理
	Model::PostDraw();
#pragma endregion

#pragma region 前景スプライト描画
	// 前景スプライト描画前処理
	Sprite::PreDraw(commandList);

	// 02_14の33ページ
	player_->DrawUI();
	//////////

	/// <summary>
	/// ここに前景スプライトの描画処理を追加できる
	/// </summary>

	// スプライト描画後処理
	Sprite::PostDraw();

#pragma endregion
}

void GameScene::CheckAllCollisions() {
	Vector3 posA, posB, posC, posD;
	const std::list<PlayerBullet*>& playerBullets = player_->GetBullets();
	//const std::list<EnemyBullet*>& enemyBullets = enemy_->GetBullets();
	const float PLAYER_R = 1.0f;
	const float ENEMY_R = 1.0f;
	const float BULLET_R = 1.0f;

	#pragma region 自キャラと敵弾の当たり判定
	posA = player_->GetWorldPosition();
	for (EnemyBullet* bullet: enemyBullets_) {
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
	
	for (PlayerBullet* bullet : playerBullets) {
		for (Enemy* enemy : enemys_) {
			posC = enemy->GetWorldPosition();
			posB = bullet->GetWorldPosition();
		float P = (posB.x - posC.x) * (posB.x - posC.x) +
			      (posB.y - posC.y) * (posB.y - posC.y) +
		          (posB.z - posC.z) * (posB.z - posC.z);
		float R = (BULLET_R + ENEMY_R) * (BULLET_R + ENEMY_R);
		if (P <= R) {
			enemy->OnCollision();
			bullet->OnCollision();
		}
		}
	}
    #pragma endregion


	#pragma region 自弾と敵弾の当たり判定
	for (PlayerBullet* playerBullet : playerBullets) {
		for (EnemyBullet* enemyBullet : enemyBullets_) {
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

void GameScene::AddEnemyBullet(EnemyBullet* enemyBullet) { 
	enemyBullets_.push_back(enemyBullet); 
}

void GameScene::AddEnemy(Vector3 position) {
	Enemy* enemy = new Enemy();
	enemy->Initialize(Enemymodel_, EnemyTexture_,position);
	enemy->SetPlayer(player_);
	enemy->SetGameScene(this);
	enemys_.push_back(enemy);
}

void GameScene::LoadEnemyPopData() {
	std::ifstream file;
	file.open("Resources/enemyPop.csv");
	assert(file.is_open());

	enemyPopCommands << file.rdbuf();
	file.close();
}

void GameScene::UpdateEnemyPopCommands() { 
	if (standFlag) {
		standTime--;
		if (standTime <= 0) {
			standFlag = false;
		}
	    return;
	}
	std::string line;
	while (getline(enemyPopCommands, line)) {
		std::istringstream line_stream(line);
		std::string word;
		getline(line_stream, word, ',');
		if (word.find("//") == 0) {
			continue;
		}
		if (word.find("POP") == 0) {
			getline(line_stream, word, ',');
			float x = (float)std::atof(word.c_str());

			getline(line_stream, word, ',');
			float y = (float)std::atof(word.c_str());

			getline(line_stream, word, ',');
			float z = (float)std::atof(word.c_str());

			AddEnemy(Vector3(x,y,z));
		} else if (word.find("WAIT") == 0) {
			getline(line_stream, word, ',');
			int32_t waitTime = atoi(word.c_str());
			standFlag = true;
			standTime = waitTime;
			break;
		}
	}
}