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
#include "Map.h"
#include "Window.h"
#include <iostream>

Player::Player() : Entity(EntityType::PLAYER)
{
	name.Create("Player");
}

Player::~Player() {

}

bool Player::Awake() {

	//L03: DONE 2: Initialize Player parameters
	initialPos = iPoint(config.attribute("x").as_int(), config.attribute("y").as_int());
	position = initialPos;

	idleDog.loop = config.child("animations").child("idledog").attribute("loop").as_bool();
	idleDog.speed = config.child("animations").child("idledog").attribute("speed").as_float();

	for (pugi::xml_node animNode = config.child("animations").child("idledog").child("idle"); animNode != NULL; animNode = animNode.next_sibling("idle")) {
		idleDog.PushBack({ animNode.attribute("x").as_int(), animNode.attribute("y").as_int() ,animNode.attribute("w").as_int() ,animNode.attribute("h").as_int() });
	}

	walkingDog.loop = config.child("animations").child("walkingdog").attribute("loop").as_bool();
	walkingDog.speed = config.child("animations").child("walkingdog").attribute("speed").as_float();

	for (pugi::xml_node animNode = config.child("animations").child("walkingdog").child("walk"); animNode != NULL; animNode = animNode.next_sibling("walk")) {
		walkingDog.PushBack({ animNode.attribute("x").as_int(), animNode.attribute("y").as_int() ,animNode.attribute("w").as_int() ,animNode.attribute("h").as_int() });
	}

	dieDog.loop = config.child("animations").child("diedog").attribute("loop").as_bool();
	dieDog.speed = config.child("animations").child("diedog").attribute("speed").as_float();

	for (pugi::xml_node animNode = config.child("animations").child("diedog").child("die"); animNode != NULL; animNode = animNode.next_sibling("die")) {
		dieDog.PushBack({ animNode.attribute("x").as_int(), animNode.attribute("y").as_int() ,animNode.attribute("w").as_int() ,animNode.attribute("h").as_int() });
	}

	jumpDog.loop = config.child("animations").child("jumpdog").attribute("loop").as_bool();
	jumpDog.speed = config.child("animations").child("jumpdog").attribute("speed").as_float();

	for (pugi::xml_node animNode = config.child("animations").child("jumpdog").child("jump"); animNode != NULL; animNode = animNode.next_sibling("jump")) {
		jumpDog.PushBack({ animNode.attribute("x").as_int(), animNode.attribute("y").as_int() ,animNode.attribute("w").as_int() ,animNode.attribute("h").as_int() });
	}

	attackDog.loop = config.child("animations").child("attackdog").attribute("loop").as_bool();
	attackDog.speed = config.child("animations").child("attackdog").attribute("speed").as_float();

	for (pugi::xml_node animNode = config.child("animations").child("attackdog").child("attack"); animNode != NULL; animNode = animNode.next_sibling("attack")) {
		attackDog.PushBack({ animNode.attribute("x").as_int(), animNode.attribute("y").as_int() ,animNode.attribute("w").as_int() ,animNode.attribute("h").as_int() });
	}

	barkFX = app->audio->LoadFx(config.attribute("barkfxpath").as_string());
	return true;
}

bool Player::Start() {

	texture = app->tex->Load(config.attribute("texturePath").as_string());

	// L07 DONE 5: Add physics to the player - initialize physics body
	app->tex->GetSize(texture, texW, texH);
	pbody = app->physics->CreateCircle(initialPos.x, initialPos.y, 16, bodyType::DYNAMIC);
	pbody->body->SetFixedRotation(true);
	bark = app->physics->CreateRectangleSensor(0, 0, 34, 50, bodyType::DYNAMIC);
	bark->listener = this;
	// L07 DONE 6: Assign player class (using "this") to the listener of the pbody. This makes the Physics module to call the OnCollision method
	pbody->listener = this;

	// L07 DONE 7: Assign collider type
	pbody->ctype = ColliderType::PLAYER;
	bark->ctype = ColliderType::BARK;
	bark->body->SetLinearVelocity({ 0,0 });
	bark->body->SetGravityScale(0);

	//initialize audio effect
	pickCoinFxId = app->audio->LoadFx(config.attribute("coinfxpath").as_string());
	currentAnimation = &idleDog;
	return true;
}

bool Player::Update(float dt)
{
	// L07 DONE 5: Add physics to the player - updated player position using physics

	//L03: DONE 4: render the player texture and modify the position of the player using WSAD keys and render the texture
	b2Vec2 velocity;
	if (!god) {
		velocity = b2Vec2(0, -GRAVITY_Y);
	}
	else {
		velocity = b2Vec2(0, 0);
	}

	if (pbody->body->GetContactList() == NULL) {
		grounded = false;
	}

	currentAnimation->Update();
	if (!dead) {
		if (app->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) {
			velocity.x = -0.3 * dt;
			if (remainingJumpSteps == 0 && grounded) {
				velocity.y = 0.01 * dt;
			}
			currentAnimation = &walkingDog;
			flip = true;
		}

		if (app->input->GetKey(SDL_SCANCODE_A) == KEY_UP) {
			currentAnimation = &idleDog;
		}

		if (app->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) {
			velocity.x = 0.3 * dt;
			if (remainingJumpSteps == 0 && grounded) {
				velocity.y = 0.01 * dt;
			}
			currentAnimation = &walkingDog;
			flip = false;
		}

		if (app->input->GetKey(SDL_SCANCODE_D) == KEY_UP) {
			currentAnimation = &idleDog;
		}

		if (!god) {
			if (pbody->body->GetContactList() != NULL && grounded) {
				if (app->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN)// && pbody->listener->OnCollision())
				{
					remainingJumpSteps = maxJumpSteps; //  1/10th of a second at 60Hz
					jumpForceReduce = 0;
				}
			}

			if (remainingJumpSteps > 0)
			{
				//to change velocity by 10 in one time step
				pbody->body->SetLinearDamping(0.1f);
				float force = pbody->body->GetMass() * 10 / (1 / 30.0); //f = mv/t
				force /= 4.0;	//spread this over 6 time steps
				force -= jumpForceReduce;

				jumpForceReduce = (maxJumpSteps - remainingJumpSteps);
				pbody->body->ApplyForce(b2Vec2(0, -(force * dt)), pbody->body->GetWorldCenter(), true);
				remainingJumpSteps--;
			}
		}
		else {
			if (app->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT) {
				velocity.y = -0.2 * dt;
				currentAnimation = &walkingDog;
			}
			if (app->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) {
				velocity.y = 0.2 * dt;
				currentAnimation = &walkingDog;
			}
		}


		if (app->input->GetKey(SDL_SCANCODE_Q) == KEY_DOWN) {
			attackDog.Reset();
			app->audio->PlayFx(barkFX);
			currentAnimation = &attackDog; 
			b2Vec2 bPosition;
			if (flip){
				bPosition = b2Vec2(PIXEL_TO_METERS(position.x), PIXEL_TO_METERS(position.y));
			}
			else {
				bPosition = b2Vec2(PIXEL_TO_METERS(position.x + 1), PIXEL_TO_METERS(position.y));
			}
			bark->body->SetTransform(bPosition, 0);

		}
	}
	else {
		currentAnimation = &dieDog;
	}

	if (attackDog.HasFinished()) {
		bark->body->SetTransform({ 0,0 }, 0);
	}

	if (app->input->GetKey(SDL_SCANCODE_F1) == KEY_DOWN) {
		pbody->body->SetTransform(b2Vec2(PIXEL_TO_METERS(initialPos.x), PIXEL_TO_METERS(initialPos.y)), pbody->body->GetAngle());
		position = initialPos;

		if (dead) {
			dead = false;
		}

		app->render->camera.y = ((position.y - texH / 2) - (app->scene->windowH / 2)) * -1;
		app->render->camera.x = ((position.x - texW / 2) - (app->scene->windowW / 2)) * -1;

		currentAnimation = &idleDog;
		dieDog.Reset();

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

		if (dead) {
			dead = false;
		}

		currentAnimation = &idleDog;
		dieDog.Reset();

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
		if (!god) {
			god = true;
		}
		else {
			god = false;
			position.x += (46 / 2);
			position.y += (37 / 2);
			b2Vec2 pPosition = b2Vec2(PIXEL_TO_METERS(position.x), PIXEL_TO_METERS(position.y));
			pbody->body->SetTransform(pPosition, 0);
		}
	}

	if (app->input->GetKey(SDL_SCANCODE_F4) == KEY_DOWN) {
		if (!dead) {
			dieDog.Reset();
		}
		dead = !dead;
	}

	if (!god) {
		pbody->body->SetLinearVelocity(velocity);
		b2Transform pbodyPos = pbody->body->GetTransform();
		position.x = METERS_TO_PIXELS(pbodyPos.p.x) - 48 / 2;
		position.y = METERS_TO_PIXELS(pbodyPos.p.y) - 32 / 2;
	}
	else {
		if (app->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT) {
			position.y += -0.2 * dt;
		}
		if (app->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) {
			position.y += 0.2 * dt;
		}
		if (app->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) {
			position.x += -0.3 * dt;
		}
		if (app->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) {
			position.x += 0.3 * dt;
		}
	}
	if (position.y > app->win->height/2 - 32 && position.y > 0) {
		dead = true;
		position.y = app->win->height / 2 - 32;
	}
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
		grounded = true;
		LOG("Collision PLATFORM");
		break;
	case ColliderType::ITEM:
		LOG("Collision ITEM");
		app->audio->PlayFx(pickCoinFxId);
		break;
	case ColliderType::UNKNOWN:
		LOG("Collision UNKNOWN");
		break;
	case ColliderType::ENEMY:
		if (physA == pbody) {
			dead = true;
		}
		break;
	case ColliderType::BARK:
		grounded = false;
		break;
	default:
		break;
	}
}

int Player::getPlayerTileX()
{
	return (position.x + (currentAnimation->GetCurrentFrame().w / 2)) / app->map->getTileWidth();
}

int Player::getPlayerTileY()
{
	return (position.y + (currentAnimation->GetCurrentFrame().h / 2)) / app->map->getTileHieght();
}

bool Player::LoadState(pugi::xml_node &node) {
	this->position.x = node.attribute("x").as_int();
	this->position.y = node.attribute("y").as_int();
	b2Vec2 pPosition = b2Vec2(PIXEL_TO_METERS(position.x), PIXEL_TO_METERS(position.y));
	pbody->body->SetTransform(pPosition, 0);

	return true;
};