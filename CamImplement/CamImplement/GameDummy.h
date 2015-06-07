#ifndef GAMEDUMMY_H
#define GAMEDUMMY_H

#include <DirectXMath.h>
#include <sstream>

#include "Map.h"
#include "Entity.h"
#include "Input.h"
#include "PathfindingLib.h"

#define SCALE_SMALL		0.8f
#define SCALE_MEDIUM	1.0f
#define SCALE_LARGE		1.2f

enum GameState
{
	gOngoing,
	gWon,
	gLost,
	gNextLevel
};

class GameDummy
{
private:
	Map* map;

	unsigned int obsArrSize;
	Ent::Obstacle** obsArr;
	DirectX::XMMATRIX* obsMatrixArr;
	Ent::Obstacle* worldBounds;

	//- Attack data ------
	bool* hitData[2];
	void CheckPlayerAttack();
	void CheckEnemyAttack(int index);
	//--------------------

	Ent::Player* player;
	// Stores the last tile coords of the player necessary for updating path of enemies
	int lastX;
	int lastZ;

	unsigned int enemyArrSize;
	// Stores the last tile coords of the enemies (necessary for updating path of enemies)
	PF::Pathfinding::Coordinate* lastEnemyCoord;
	// The array of enemies
	Ent::Enemy** enemyArr;
	DirectX::XMMATRIX* enemyMatrixArr;

	POINT clientSize;
	HWND windowHandle;
	float elapsedTime = 0.f;
	int frames = 0;

	// Determinates if the player or the enemies won and marks the game for termination 
	GameState gameState;

public:
	GameDummy();
	~GameDummy();

	void NewGame();

	HRESULT Initialize(HWND &wndHandle, HINSTANCE &hInstance, const D3D11_VIEWPORT &viewport);

	void Update(float deltaTime);

	DirectX::XMMATRIX GetPlayerMatrix();
	DirectX::XMVECTOR GetPlayerPosition();
	DirectX::XMFLOAT4 GetPlayerColor() const;
	Ent::Action GetPlayerAction();

	float GetPlayerHitPoints();

	int GetEnemyArrSize();
	DirectX::XMMATRIX* GetEnemyMatrices();
	DirectX::XMVECTOR GetEnemyPosition(int index);
	DirectX::XMFLOAT4 GetEnemyColor(int index) const;
	Ent::Action GetEnemyAction(int index);
	float GetEnemyHitPoints(int index);

	DirectX::XMMATRIX* GetObsMatrices();
	int GetObsArrSize() const;

	DirectX::XMMATRIX GetMapMatrix() const;
	int GetNrOfTiles() const;

	bool IsPlayerHit();
	bool IsEnemyHit(int index);

	GameState GetGameState() const;
	
	int GetPlayerFrame () const;
	int GetEnemyFrame (int index) const;
};

#endif