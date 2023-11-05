#include "Player.h"
#include "App.h"
#include "Textures.h"
#include "Audio.h"
#include "Input.h"
#include "Render.h"
#include "Scene.h"
#include "Log.h"
#include "Point.h"
#include "Physics.h"
#include <iostream>

Player::Player() : Entity(EntityType::PLAYER)
{
	name.Create("Player");
	idleDog.PushBack({ 10, 7, 46, 37 });
	dieDog.PushBack({ 60, 7, 46, 37});
	dieDog.PushBack({ 115, 7, 46, 37});
	dieDog.PushBack({ 169, 7, 46, 37});
	dieDog.loop = false;
	dieDog.speed = 0.10f;
	walkingDog.PushBack({60, 58, 46, 37});
	walkingDog.PushBack({117, 58, 46, 37});
	walkingDog.PushBack({175, 58, 46, 37});
	walkingDog.PushBack({226, 58, 46, 37});
	walkingDog.PushBack({276, 58, 46, 37});
	walkingDog.PushBack({335, 58, 46, 37});
	walkingDog.speed = 0.20f;
	jumpDog.PushBack({58, 106, 46, 37});
	jumpDog.PushBack({115, 106, 46, 37});
	jumpDog.PushBack({171, 106, 46, 37});
	jumpDog.PushBack({228, 106, 46, 37});
	jumpDog.speed = 0.20f;
}

Player::~Player() {

}

bool Player::Awake() {

	//L03: DONE 2: Initialize Player parameters
	initialPos = iPoint(config.attribute("x").as_int(), config.attribute("y").as_int());
	position = initialPos;
	return true;
}

bool Player::Start() {

	texture = app->tex->Load(config.attribute("texturePath").as_string());

	// L07 DONE 5: Add physics to the player - initialize physics body
	app->tex->GetSize(texture, texW, texH);
	pbody = app->physics->CreateRectangle(initialPos.x, initialPos.y, 44, 32, bodyType::DYNAMIC);
	pbody->body->SetFixedRotation(true);
	// L07 DONE 6: Assign player class (using "this") to the listener of the pbody. This makes the Physics module to call the OnCollision method
	pbody->listener = this;

	// L07 DONE 7: Assign collider type
	pbody->ctype = ColliderType::PLAYER;

	//initialize audio effect
	pickCoinFxId = app->audio->LoadFx(config.attribute("coinfxpath").as_string());
	currentAnimation = &idleDog;
	return true;
}

bool Player::Update(float dt)
{
	// L07 DONE 5: Add physics to the player - updated player position using physics

	//L03: DONE 4: render the player texture and modify the position of the player using WSAD keys and render the texture
	
	b2Vec2 velocity = b2Vec2(0, -GRAVITY_Y);
	currentAnimation->Update();

	if (app->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) {
		velocity.x = -0.2*dt;
		currentAnimation = &walkingDog;
		flip = true;
	}

	if (app->input->GetKey(SDL_SCANCODE_A) == KEY_UP) {
		currentAnimation = &idleDog;
	}

	if (app->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) {
		velocity.x = 0.2*dt;
		currentAnimation = &walkingDog;
		flip = false;
	}

	if (app->input->GetKey(SDL_SCANCODE_D) == KEY_UP) {
		currentAnimation = &idleDog;
	}

	if (pbody->body->GetContactList() != NULL) {
		if (app->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN)// && pbody->listener->OnCollision())
		{
			remainingJumpSteps = maxJumpSteps; //  1/10th of a second at 60Hz
			jumpForceReduce = 0;
		}
	}

	if (app->input->GetKey(SDL_SCANCODE_F1) == KEY_DOWN) {
		pbody->body->SetTransform(b2Vec2(PIXEL_TO_METERS(initialPos.x), PIXEL_TO_METERS(initialPos.y)), pbody->body->GetAngle());
		position = initialPos;

		app->render->camera.y = ((position.y - texH / 2) - (app->scene->windowH / 2)) * -1;
		app->render->camera.x = ((position.x - texW / 2) - (app->scene->windowW / 2)) * -1;

		if (app->render->camera.x >= 0) {
			app->render->camera.x = 0;
		}
		if (app->render->camera.y >= 0) {
			app->render->camera.y = 0;
		}

	}

	if (app->input->GetKey(SDL_SCANCODE_F3) == KEY_DOWN) {
		pbody->body->SetTransform(b2Vec2(PIXEL_TO_METERS(initialPos.x), PIXEL_TO_METERS(initialPos.y)), pbody->body->GetAngle());
		position = initialPos;

		app->render->camera.y = ((position.y - texH / 2) - (app->scene->windowH / 2)) * -1;
		app->render->camera.x = ((position.x - texW / 2) - (app->scene->windowW / 2)) * -1;

		if (app->render->camera.x >= 0) {
			app->render->camera.x = 0;
		}
		if (app->render->camera.y >= 0) {
			app->render->camera.y = 0;
		}
	}

	if (app->input->GetKey(SDL_SCANCODE_F10) == KEY_DOWN) {
		god = !god;
	}

	if (app->input->GetKey(SDL_SCANCODE_F4) == KEY_DOWN) {
		currentAnimation = &dieDog;
		//dead = true;
	}

	if (app->input->GetKey(SDL_SCANCODE_F4) == KEY_DOWN) {
		dieDog.Reset();
		//dead = true;
	}

	if (remainingJumpSteps > 0)
	{
		//to change velocity by 10 in one time step
		float force = pbody->body->GetMass() * 10 / (1 / 30.0); //f = mv/t
		force /= 4.0;	//spread this over 6 time steps
		force -= jumpForceReduce;

		jumpForceReduce = maxJumpSteps - remainingJumpSteps;
		pbody->body->ApplyForce(b2Vec2(0, -(force * dt)), pbody->body->GetWorldCenter(), true);
		if (!god) {
			remainingJumpSteps--;
		}
	}

	pbody->body->SetLinearVelocity(velocity);
	b2Transform pbodyPos = pbody->body->GetTransform();
	position.x = METERS_TO_PIXELS(pbodyPos.p.x) - 48 / 2;
	position.y = METERS_TO_PIXELS(pbodyPos.p.y) - 32 / 2;

	app->render->DrawTextureDX(texture, position.x, position.y, flip, &currentAnimation->GetCurrentFrame());

	return true;
}

bool Player::CleanUp()
{
	return true;
}

// L07 DONE 6: Define OnCollision function for the player. 
void Player::OnCollision(PhysBody* physA, PhysBody* physB) {
	switch (physB->ctype)
	{
	case ColliderType::PLATFORM:
		LOG("Collision PLATFORM");
		break;
	case ColliderType::ITEM:
		LOG("Collision ITEM");
		app->audio->PlayFx(pickCoinFxId);
		break;
	case ColliderType::UNKNOWN:
		LOG("Collision UNKNOWN");
		break;
	default:
		break;
	}
}