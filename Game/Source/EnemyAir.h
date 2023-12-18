#pragma once
#include "Enemy.h"
class EnemyAir :  public Enemy
{
public:
	EnemyAir();
	bool Awake();

	bool Start();

	bool Update(float dt);

	void moveToPlayer(float dt);

	void OnCollision(PhysBody* physA, PhysBody* physB);

	Animation idleAir;
	Animation flyingAir;
	Animation dieAir;
};

