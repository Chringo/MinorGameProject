#ifndef GAMEDUMMY_H
#define GAMEDUMMY_H

#include <DirectXMath.h>
#include <sstream>

#include "Map.h"
#include "Entity.h"
#include "Input.h"
#include "PathfindingLib.h"

class GameDummy
{
private:
	Map* map;
	DirectX::XMMATRIX* tileMatrixArr;

	// Testing bed for pathfinding - TO BE REMOVED LATER
	LinkedList<DirectX::XMFLOAT3> path;
	void PFTest();
	// 

	//- Attack data ------
	bool* hitData;
	void CheckPlayerAttack();
	//--------------------

	Ent::Player* player;
	
	int enemyArrSize;
	Ent::Enemy** enemyArr;
	DirectX::XMMATRIX* enemyMatrixArr;

	POINT clientSize;
	HWND windowHandle;
	float elapsedTime = 0.f;
	int frames = 0;

public:
	GameDummy();
	~GameDummy();

	HRESULT Initialize(HWND &wndHandle, HINSTANCE &hInstance, const D3D11_VIEWPORT &viewport);

	void Update(float deltaTime);

	DirectX::XMMATRIX GetPlayerMatrix();
	DirectX::XMVECTOR GetPlayerPosition();
	Ent::Action GetPlayerAction();
	float GetPlayerHitPoints();

	int GetEnemyArrSize();
	DirectX::XMMATRIX* GetEnemyMatrices();

	DirectX::XMMATRIX* GetTileMatrices();
	int GetNrOfTiles() const;

	void ReleaseCOM();
};

#endif