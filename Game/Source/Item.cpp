#include "Item.h"
#include "App.h"
#include "Textures.h"
#include "Audio.h"
#include "Input.h"
#include "Render.h"
#include "Scene.h"
#include "Log.h"
#include "Point.h"
#include "Physics.h"

Item::Item() : Entity(EntityType::ITEM)
{
	name.Create("item");
}

Item::~Item() {}

bool Item::Awake() {

	position.x = parameters.attribute("x").as_int();
	position.y = parameters.attribute("y").as_int();
	texturePath = parameters.attribute("texturepath").as_string();

	return true;
}

bool Item::Start() {


	//initilize textures
	texture = app->tex->Load(texturePath);
	
	// L07 DONE 4: Add a physics to an item - initialize the physics body
	app->tex->GetSize(texture, texW, texH);

	int width = 32;
	int height = 32;

	if (type == ItemType::COOKIE) {
		pbody = app->physics->CreateCircleSensor(position.x + width / 2, position.y + height / 2, 12, bodyType::STATIC, ColliderType::ITEM_COOKIE);
	}

	if (type == ItemType::LIFE) {
		pbody = app->physics->CreateCircleSensor(position.x + width / 2, position.y + height / 2, 14, bodyType::STATIC, ColliderType::ITEM_LIFE);
	}

	if (type == ItemType::CHECKPOINT) {
		pbody = app->physics->CreateRectangleSensor(position.x, position.y, 60, 49, bodyType::STATIC, ColliderType::ITEM_CHECKPOINT);
	}

	pbody->listener = this;

	return true;
}

bool Item::Update(float dt)
{
	// L07 DONE 4: Add a physics to an item - update the position of the object from the physics.  

	b2Transform pbodyPos = pbody->body->GetTransform();
	position.x = METERS_TO_PIXELS(pbodyPos.p.x) - texH / 2;
	position.y = METERS_TO_PIXELS(pbodyPos.p.y) - texH / 2;

	if (type == ItemType::CHECKPOINT) {
		SDL_Rect rect;

		if (checkpoint) {
			rect = { 60,0,60,49 };
		}
		else {
			rect = { 0, 0, 60, 49 };
		}

		app->render->DrawTexture(texture, position.x, position.y, &rect);
	}
	else {
		app->render->DrawTexture(texture, position.x, position.y, false);
	}
	return true;
}

bool Item::CleanUp()
{
	return true;
}

void Item::OnCollision(PhysBody* physA, PhysBody* physB) {

	switch (physB->ctype)
	{
	case ColliderType::ITEM_COOKIE:
		LOG("Collision ITEM_COOKIE");

		break;
	case ColliderType::ITEM_LIFE:
		LOG("Collision ITEM_LIFE");

		break;
	case ColliderType::PLATFORM:
		LOG("Collision PLATFORM");

		break;
	case ColliderType::UNKNOWN:
		LOG("Collision UNKNOWN");

		break;

	case ColliderType::PLAYER:
		LOG("Collision PLAYER");

		if (type == ItemType::COOKIE || type == ItemType::LIFE) {
			this->Disable();
			pbody->body->DestroyFixture(pbody->body->GetFixtureList());
			pbody->body->SetActive(false);

		} else if (type == ItemType::CHECKPOINT) {

			if (!checkpoint) app->audio->PlayFx(checkpointFX);

			checkpoint = true;

			app->scene->player->checkPos = { position.x, position.y };

			app->SaveRequest();
		}
		break;
	}
}