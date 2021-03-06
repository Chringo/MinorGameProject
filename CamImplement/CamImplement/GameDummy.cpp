#include "GameDummy.h"

using namespace DirectX;

/**********************************************************************************/
/*********************************** Game Stuff ***********************************/

GameDummy::GameDummy()
{
	player = nullptr;

	enemyArr = nullptr;
	enemyMatrixArr = nullptr;
	pfMap = nullptr;

	map = nullptr;
}

GameDummy::~GameDummy()
{
	delete player;

	for (size_t i = 0; i < (size_t)enemyArrSize; i++)
	{
		delete enemyArr[i];
	}
	delete[] enemyArr;

	//levels.~LinkedList();
	if (map != nullptr)
		delete map;

	delete pfMap;

	delete[] hitData[0];
	delete[] hitData[1];
}

void GameDummy::InitLevels()
{
	currentLevel = 0;
	map = levels.elementAt(currentLevel);
}
void GameDummy::spawnEnemies(int amount, int type)
{
	Ent::Enemy* base = new Ent::Enemy(regular);
	switch (type)
	{
	case 0:
		//base = new Ent::Enemy(regular);
		break;
	case 1:
		base = new Ent::Enemy(runner);
		break;
	case 2:
		base = new Ent::Enemy(elite);
		break;
	case 3:
		base = new Ent::Enemy(giant);
		break;
	}

	/**********************************************************************************/
	/************************************* Enemy  *************************************/

	if (enemyArr != nullptr)
	{
		for (size_t i = 0; i < (size_t)enemyArrSize; i++)
		{
			delete enemyArr[i];
		}
		delete[] enemyArr;
	}

	LQueue<int> pathUpdate = LQueue<int>();

	enemyArrSize = amount;
	enemyMatrixArr = new XMMATRIX[enemyArrSize];
	hitData[0] = new bool[enemyArrSize];
	hitData[1] = new bool[enemyArrSize];
	enemyArr = new Ent::Enemy*[enemyArrSize];
	for (size_t i = 0; i < (size_t)enemyArrSize; i++)
	{
		enemyArr[i] = new Ent::Enemy(*base);

		int corner = i % 4;
		int max = map->getChunkSize() - 3;
		switch (corner)
		{
		case 0:
			enemyArr[i]->SetPosition(map->getBaseTiles()[2][2].worldpos);
			break;
		case 1:
			enemyArr[i]->SetPosition(map->getBaseTiles()[2][max].worldpos);
			break;
		case 2:
			enemyArr[i]->SetPosition(map->getBaseTiles()[max][2].worldpos);
			break;
		case 3:
			enemyArr[i]->SetPosition(map->getBaseTiles()[max][max].worldpos);
			break;
		}

		enemyMatrixArr[i] = XMMatrixIdentity();
		hitData[0][i] = false;
		hitData[1][i] = false;
	}

	delete base;
}

void GameDummy::NewGame()
{
	gameState = gOngoing;

	if (player != nullptr)
	{
		delete player;
	}

	player = new Ent::Player(XMVectorSet(64.f, 0.f, 64.f, 1.f), SCALE_MEDIUM, 9.0f);

	lastX = -1;
	lastZ = -1;

	/************************************ Obstacle ************************************/
	if (obsArr != nullptr)
	{
		for (size_t i = 0; i < (size_t)enemyArrSize; i++)
		{
			delete obsArr[i];
		}
		delete[] obsArr;
		delete[] obsMatrixArr;
	}

	obsArrSize = map->getObstacles();
	obsMatrixArr = new XMMATRIX[obsArrSize];
	obsArr = new Ent::Obstacle*[obsArrSize];
	int cSize = map->getChunkSize();
	int index = 0;
	for (int h = 0; h < cSize; h++)
	{
		for (int w = 0; w < cSize; w++)
		{
			if (map->getBaseTiles()[h][w].obstacle)
			{
				obsArr[index] = new Ent::Obstacle(map->getBaseTiles()[h][w].worldpos.x, map->getBaseTiles()[h][w].worldpos.z, 2.f, 2.f, map->getRandom());
				obsMatrixArr[index] = XMMatrixIdentity();
				index++;
			}
		}
	}
	/**********************************************************************************/
	/************************************* Pathfinding *************************************/

	if (pfMap != nullptr) delete pfMap;

	// Allocates 2D bool array used for marking tiles as blocked
	bool** disable = new bool*[map->getChunkSize()];
	for (int i = 0; i < map->getChunkSize(); i++)
	{
		disable[i] = new bool[map->getChunkSize()];
		for (int j = 0; j < map->getChunkSize(); j++)
		{
			disable[i][j] = false;
		}
	}

	for (size_t i = 0; i < (size_t)obsArrSize; i++)
	{
		disable[obsArr[i]->getXTileSpace(map->TILESIZE)][obsArr[i]->getZTileSpace(map->TILESIZE)] = true;
	}

	// Makes handling of A* easier. Deallocates the 2D bool array
	pfMap = new PF::Map(disable, map->getChunkSize());

	/**********************************************************************************/
	switch (currentLevel)
	{
	case 0:
		spawnEnemies(4, 1);
		break;
	case 1:
		spawnEnemies(4, 3);
		break;
	case 2:
		spawnEnemies(4, 2);
		break;
	default:
		spawnEnemies(5, 0);
		break;
	}
}

HRESULT GameDummy::Initialize(HWND &wndHandle, HINSTANCE &hInstance, const D3D11_VIEWPORT &viewport)
{
	RECT r;
	windowHandle = wndHandle;
	GetWindowRect(wndHandle, &r);
	clientSize.x = r.right - r.left;
	clientSize.y = r.bottom - r.top;

	/************************************** Map  **************************************/

	levels.insertLast(new Map(5, 5, 68.f));
	levels.insertLast(new Map(8, 5, 80.f));
	//levels.insertLast(new Map(3, 5, 73.f));
	levels.insertLast(new Map(21, 5, 85.f));
	//levels.insertLast(new Map(9, 5, 70.f));
	//levels.insertLast(new Map(25, 5, 70.f));

	InitLevels();

	/**********************************************************************************/
	/************************************ Obstacle ************************************/

	// Static Obstacles - Dynamic Obstacles which is updated after each level is in NewGame()
	worldBounds = new Ent::Obstacle(62.f, 62.f, 64.f, 64.f, 0.f);

	/**********************************************************************************/
	NewGame();

	return S_OK;
}

void GameDummy::Update(float deltaTime)
{
	// Output (total average) fps.
	elapsedTime += deltaTime;
	frames++;

	std::wstringstream wss;
	wss << "FPS: " << (INT)((float)frames / elapsedTime) << " Timer: " << (INT)(elapsedTime);
	SetWindowText(windowHandle, wss.str().c_str());

	// Get cursor position.
	POINT cursor;
	GetCursorPos(&cursor);
	ScreenToClient(windowHandle, &cursor);

	// Adjust to client center.
	cursor.x -= (LONG)(clientSize.x * 0.5f - 8);
	cursor.y -= (LONG)(clientSize.y * 0.5f - 16);

	bool gameWon = true;
	for (int i = 0; i < enemyArrSize; i++)
	{
		if (!enemyArr[i]->IsDead())
		{
			gameWon = false;
		}
	}

	if (gameWon && levels.size() == currentLevel + 1)// Player won
	{
		currentLevel = 0;
		gameState = gWon;
	}
	else if (gameWon)// Next level
	{
		currentLevel++;
		map = levels.elementAt(currentLevel);
		gameState = gNextLevel;
	}

	/************************************* Pathfinding *************************************/

	const float ts = map->TILESIZE;
	const int cs = map->getChunkSize();

	// Update if player moves
	if (lastX != player->getXTileSpace(ts, cs) || lastZ != player->getZTileSpace(ts, cs))
	{
		// Save current player pos for comparison
		lastX = player->getXTileSpace(ts, cs);
		lastZ = player->getZTileSpace(ts, cs);
		
		// Enqueue paths that needs to be updated
		for (int i = 0; i < enemyArrSize; i++)
		{
			pathUpdate.Enqueue(i);
		}
	}

	int maxPath = 2;
	int pathCount = 0;

	if (pathUpdate.Size() > 0 && pathCount < maxPath)
	{
		enemyArr[pathUpdate.Dequeue()]->setPathfinding
			(
			map,
			pfMap,
			player->GetPosition().m128_f32[0],
			player->GetPosition().m128_f32[2]
			);
		pathCount++;
	}

	

	/***************************************************************************************/
	/************************************** Collision **************************************/

	for (UINT obstacleId = 0; obstacleId < obsArrSize; obstacleId++)
		player->Intersect(obsArr[obstacleId]);
	player->Intersect(worldBounds);

	if (!player->IsDead())
	{
		player->Update(deltaTime);
		player->SetAttackDirection(cursor);
		CheckPlayerAttack();
	}
	else // player lose
	{
		gameState = gLost;
	}

	// Update game objects.
	for (size_t i = 0; i < (size_t)enemyArrSize; i++)
	{
		if (!enemyArr[i]->IsDead())
		{
			enemyArr[i]->updateMoveOrder();
			if (XMVector3LengthEst(player->GetPosition() - enemyArr[i]->GetPosition()).m128_f32[0] < 6.f)
			{
				enemyArr[i]->SetAttackDirection(player->GetPosition());
				enemyArr[i]->PerformAction(Ent::Attack1);
			}
			CheckEnemyAttack(i);

			enemyArr[i]->Update(deltaTime);
			enemyArr[i]->Intersect(worldBounds);
			player->Intersect(enemyArr[i]);
			for (size_t j = i + 1; j < (size_t)enemyArrSize; j++)
			{
				if (i != j) enemyArr[i]->Intersect(enemyArr[j]);
			}
		}
	}
}

GameState GameDummy::GetGameState() const
{
	return gameState;
}

/**********************************************************************************/
/************************************* Attack *************************************/

void GameDummy::CheckPlayerAttack()
{
	if (player->GetAttackValue() > 0)
	{
		for (size_t i = 0; i < (size_t)enemyArrSize; i++)
		{
			hitData[0][i] = false;
		}
	}
	else return;

	XMVECTOR atkPos = player->GetAttackPosition();
	float d = 0.f;
	for (size_t i = 0; i < (size_t)enemyArrSize; i++)
	{
		d = XMVector3Length(enemyArr[i]->GetPosition() - atkPos).m128_f32[0];
		if (d < 5.f && !hitData[0][i])
		{
			hitData[0][i] = true;
			enemyArr[i]->DecreaseHealth(player->GetAttackValue());
		}
	}
}

void GameDummy::CheckEnemyAttack(int index)
{
	Ent::Action action = enemyArr[index]->GetCurrentAction();
	int frame = enemyArr[index]->GetCurrentActionFrame();

	if (enemyArr[index]->GetAttackValue() > 0)
		hitData[1][index] = false;
	else return;

	XMVECTOR atkPos = enemyArr[index]->GetAttackPosition();
	float d = XMVector3LengthEst(player->GetPosition() - atkPos).m128_f32[0];
	if (d < 5.f && !hitData[1][index])
	{
		hitData[1][index] = true;
		player->DecreaseHealth(enemyArr[index]->GetAttackValue());
	}
}

/**********************************************************************************/
/*********************************** Get Player ***********************************/

XMMATRIX GameDummy::GetPlayerMatrix()
{
	return player->GetTransform();
}

XMVECTOR GameDummy::GetPlayerPosition()
{
	return player->GetPosition();
}

XMFLOAT4 GameDummy::GetPlayerColor() const
{
	return player->GetColor();
}

Ent::Action GameDummy::GetPlayerAction()
{
	return player->GetCurrentAction();
}

float GameDummy::GetPlayerHitPoints()
{
	return player->GetHitPoints();
}

bool GameDummy::IsPlayerHit()
{
	return player->GetHitFrameCount() || player->IsDead();
}

int GameDummy::GetPlayerFrame () const
{
	return player->GetFrame ();
}

/**********************************************************************************/
/*********************************** Get Enemy  ***********************************/

int GameDummy::GetEnemyArrSize()
{
	return enemyArrSize;
}

DirectX::XMMATRIX* GameDummy::GetEnemyMatrices()
{
	for (size_t i = 0; i < (size_t)enemyArrSize; i++)
	{
		enemyMatrixArr[i] = enemyArr[i]->GetTransform();
	}
	return enemyMatrixArr;
}

DirectX::XMVECTOR GameDummy::GetEnemyPosition(int index)
{
	return enemyArr[index]->GetPosition();
}

DirectX::XMFLOAT4 GameDummy::GetEnemyColor(int index) const
{
	return enemyArr[index]->GetColor();
}

Ent::Action GameDummy::GetEnemyAction(int index)
{
	return enemyArr[index]->GetCurrentAction();
}

float GameDummy::GetEnemyHitPoints(int index)
{
	return enemyArr[index]->GetHitPoints();
}

bool GameDummy::IsEnemyHit(int index)
{
	return enemyArr[index]->GetHitFrameCount() || enemyArr[index]->IsDead();
}

int GameDummy::GetEnemyFrame (int index) const
{
	return enemyArr[index]->GetFrame ();
}

/**********************************************************************************/
/********************************** Get Obstacle **********************************/
DirectX::XMMATRIX* GameDummy::GetObsMatrices()
{
	for (size_t i = 0; i < (size_t)obsArrSize; i++)
	{
		obsMatrixArr[i] = obsArr[i]->GetTransform();
	}
	return obsMatrixArr;
}

DirectX::BoundingOrientedBox GameDummy::GetObsBoundingBox(int index)
{
	return obsArr[index]->GetBoundingBox();
}

int GameDummy::GetObsArrSize() const
{
	return this->obsArrSize;
}

/**********************************************************************************/
/************************************ Get Map  ************************************/
DirectX::XMMATRIX GameDummy::GetMapMatrix() const
{
	return map->setMapPlane();
}
int GameDummy::GetNrOfTiles() const
{
	return map->getNrOfTiles();
}