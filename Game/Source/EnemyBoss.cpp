#include "EnemyBoss.h"
#include "Scene.h"
#include "Log.h"
#include "Audio.h"

EnemyBoss::EnemyBoss() :Enemy()
{

}

EnemyBoss::~EnemyBoss() {

}

bool EnemyBoss::Awake()
{
	Enemy::Awake();

	idleGround.loop = parameters.child("animations").child("idleboss").attribute("loop").as_bool();
	idleGround.speed = parameters.child("animations").child("idleboss").attribute("speed").as_float();

	for (pugi::xml_node animNode = parameters.child("animations").child("idleboss").child("idle"); animNode != NULL; animNode = animNode.next_sibling("idle")) {
		idleGround.PushBack({ animNode.attribute("x").as_int(), animNode.attribute("y").as_int() ,animNode.attribute("w").as_int() ,animNode.attribute("h").as_int() });
	}

	walkingGround.loop = parameters.child("animations").child("moveboss").attribute("loop").as_bool();
	walkingGround.speed = parameters.child("animations").child("moveboss").attribute("speed").as_float();

	for (pugi::xml_node animNode = parameters.child("animations").child("moveboss").child("walk"); animNode != NULL; animNode = animNode.next_sibling("walk")) {
		walkingGround.PushBack({ animNode.attribute("x").as_int(), animNode.attribute("y").as_int() ,animNode.attribute("w").as_int() ,animNode.attribute("h").as_int() });
	}

	dieGround.loop = parameters.child("animations").child("dieboss").attribute("loop").as_bool();
	dieGround.speed = parameters.child("animations").child("dieboss").attribute("speed").as_float();

	for (pugi::xml_node animNode = parameters.child("animations").child("dieboss").child("die"); animNode != NULL; animNode = animNode.next_sibling("die")) {
		dieGround.PushBack({ animNode.attribute("x").as_int(), animNode.attribute("y").as_int() ,animNode.attribute("w").as_int() ,animNode.attribute("h").as_int() });
	}

	deathFX = app->audio->LoadFx(parameters.attribute("deathfxpath").as_string());
	return true;
}

bool EnemyBoss::Start()
{
	Enemy::Start();

	distChase = 30;
	initPosition = position;
	texture = app->tex->Load(texturePath);
	currentAnimation = &walkingGround;

	// L07 DONE 5: Add physics to the player - initialize physics body
	pbody = app->physics->CreateCircle(position.x, position.y, currentAnimation->GetCurrentFrame().h / 2, bodyType::DYNAMIC);

	// L07 DONE 6: Assign player class (using "this") to the listener of the pbody. This makes the Physics module to call the OnCollision method
	pbody->listener = this;

	// L07 DONE 7: Assign collider type
	pbody->ctype = ColliderType::ENEMY;

	pbody->body->SetGravityScale(0);

	chaseVelovity = 0.12;
	patrolVelocity = 0.1;

	return true;
}

bool EnemyBoss::Update(float dt)
{
	if (abs(app->scene->GetPlayer()->getPlayerTileX() - getEnemyTileX()) > 50) {
		velocity.x = 0;
		velocity.y = 0;
		pbody->body->SetLinearVelocity(velocity);
		return true;
	}
	velocity = b2Vec2(0, 10);

	if (pbody->body->GetContactList() == NULL) {
		grounded = false;
	}
	else {
		grounded = true;
	}

	if (!hit) {
		if (canChase(distChase) && PTileX >= Patrol1.x && PTileX <= Patrol2.x) {
			ActualVelocity = chaseVelovity;
			dest = iPoint(PTileX, PTileY);
			moveToPlayer(dt);
			currentAnimation = &walkingGround;
		}
		else {
			ActualVelocity = patrolVelocity;
			moveToPoint(dt);
		}
	}
	else {
		if (dieGround.HasFinished()) {
			dead = true;
		}
	}

	Enemy::Update(dt);

	if (velocity.x < 0) {
		flip = true;
	}
	if (velocity.x > 0) {
		flip = false;
	}

	return true;
}

void EnemyBoss::moveToPlayer(float dt)
{
	const DynArray<iPoint>* path = SearchWay();
	//check if t has arrivied
	if (path->Count() > 1) {
		//check if it shall move to x
		if (TileX > path->At(1)->x) {
			velocity.x = -ActualVelocity * dt;
		}
		else {
			velocity.x = ActualVelocity * dt;
		}
	}
	else if (path->Count() == 1) {
		if (app->scene->GetPlayer()->position.x < position.x) {
			velocity.x = -ActualVelocity * dt;
		}
		else {
			velocity.x = ActualVelocity * dt;
		}
	}
}

void EnemyBoss::OnCollision(PhysBody* physA, PhysBody* physB)
{
	switch (physB->ctype)
	{
	case ColliderType::BARK:
		hit = true;
		velocity.x = 0;
		currentAnimation = &dieGround;
		if (!dead) {
			app->audio->PlayFx(deathFX);
		}
		break;
	case ColliderType::DEADLY:
		hit = true;
		velocity.x = 0;
		currentAnimation = &dieGround;
		if (!dead) {
			app->audio->PlayFx(deathFX);
		}
		break;
	case ColliderType::PLATFORM:
		break;
	default:
		break;
	}
}

void EnemyBoss::moveToPoint(float dt)
{
	Enemy::Patrol();
	const DynArray<iPoint>* path = SearchWay();
	//check if t has arrivied
	if (path->Count() > 1) {
		//check if it shall move to x
		if (TileX > path->At(1)->x) {
			velocity.x = -ActualVelocity * dt;
		}
		else {
			velocity.x = ActualVelocity * dt;
		}
	}
	else if (path->Count() == 1) {
		if (app->scene->GetPlayer()->position.x < position.x) {
			velocity.x = -ActualVelocity * dt;
		}
		else {
			velocity.x = ActualVelocity * dt;
		}
	}
}