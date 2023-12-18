#pragma once
#include "Enemy.h"
class EnemyGround : public Enemy
{
public:
	EnemyGround();
	bool Awake();

	bool Start();

	bool Update(float dt);

	void moveToPlayer(float dt);

	void OnCollision(PhysBody* physA, PhysBody* physB);

	void moveToPoint(float dt);

	Animation idleGround;
	Animation walkingGround;
	Animation dieGround;
	bool grounded;
	int deathFX;
};

