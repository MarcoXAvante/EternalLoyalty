#ifndef __PLAYER_H__
#define __PLAYER_H__

#include "Entity.h"
#include "Point.h"
#include "SDL/include/SDL.h"
#include "Animation.h"

struct SDL_Texture;

class Player : public Entity
{
public:

	Player();
	
	virtual ~Player();

	bool Awake();

	bool Start();

	bool Update(float dt);

	bool CleanUp();

	// L07 DONE 6: Define OnCollision function for the player. 
	void OnCollision(PhysBody* physA, PhysBody* physB);

	void EndCollision(PhysBody* physA, PhysBody* physB);

	int getPlayerTileX();

	int getPlayerTileY();

	bool LoadState(pugi::xml_node &node);


public:

	//L02: DONE 2: Declare player parameters
	bool grounded;
	bool ramp;
	bool gameover;
	bool damage;
	float speed = 0.2f;
	SDL_Texture* texture = NULL;
	pugi::xml_node config;
	uint texW, texH;
	bool god = false;

	iPoint checkPos;

	Animation walkingDog;
	Animation idleDog;
	Animation dieDog;
	Animation jumpDog;
	Animation attackDog;
	Animation* currentAnimation = nullptr;

	bool flip;
	int lives;
	int remainingJumpSteps = 0;
	int jumpForceReduce = 0;
	int maxJumpSteps = 30;
	int checkpoint = 0;

	//Audio fx
	int pickCoinFxId;
	int barkFX;

	// L07 DONE 5: Add physics to the player - declare a Physics body
	PhysBody* pbody = NULL;
	PhysBody* bark;
};

#endif // __PLAYER_H__