#include "EnemyAir.h"
#include "Scene.h"
#include "Audio.h"

EnemyAir::EnemyAir() :Enemy()
{
	name.Create("EnemyFly");
}

bool EnemyAir::Awake()
{
	Enemy::Awake();
	idleAir.loop = parameters.child("animations").child("idleair").attribute("loop").as_bool();
	idleAir.speed = parameters.child("animations").child("idleair").attribute("speed").as_float();

	for (pugi::xml_node animNode = parameters.child("animations").child("idleair").child("idle"); animNode != NULL; animNode = animNode.next_sibling("idle")) {
		idleAir.PushBack({ animNode.attribute("x").as_int(), animNode.attribute("y").as_int() ,animNode.attribute("w").as_int() ,animNode.attribute("h").as_int() });
	}

	flyingAir.loop = parameters.child("animations").child("moveair").attribute("loop").as_bool();
	flyingAir.speed = parameters.child("animations").child("moveair").attribute("speed").as_float();

	for (pugi::xml_node animNode = parameters.child("animations").child("moveair").child("fly"); animNode != NULL; animNode = animNode.next_sibling("fly")) {
		flyingAir.PushBack({ animNode.attribute("x").as_int(), animNode.attribute("y").as_int() ,animNode.attribute("w").as_int() ,animNode.attribute("h").as_int() });
	}

	dieAir.loop = parameters.child("animations").child("dieair").attribute("loop").as_bool();
	dieAir.speed = parameters.child("animations").child("dieair").attribute("speed").as_float();

	for (pugi::xml_node animNode = parameters.child("animations").child("dieair").child("die"); animNode != NULL; animNode = animNode.next_sibling("die")) {
		dieAir.PushBack({ animNode.attribute("x").as_int(), animNode.attribute("y").as_int() ,animNode.attribute("w").as_int() ,animNode.attribute("h").as_int() });
	}

	deathFX = app->audio->LoadFx(parameters.attribute("deathfxpath").as_string());
	return true;
}

bool EnemyAir::Start()
{
	initPosition = position;
	texture = app->tex->Load(texturePath);
	currentAnimation = &flyingAir;
	// L07 DONE 5: Add physics to the player - initialize physics body
	pbody = app->physics->CreateCircle(position.x, position.y, currentAnimation->GetCurrentFrame().w / 3, bodyType::DYNAMIC);

	// L07 DONE 6: Assign player class (using "this") to the listener of the pbody. This makes the Physics module to call the OnCollision method
	pbody->listener = this;

	// L07 DONE 7: Assign collider type
	pbody->ctype = ColliderType::ENEMY;

	distChase = 12;

	right = true;

	pbody->body->SetGravityScale(0);

	chaseVelovity = 0.15;
	patrolVelocity = 0.1;

	Enemy::Start();

	return true;
}

bool EnemyAir::Update(float dt)
{
	if (abs(app->scene->GetPlayer()->getPlayerTileX() - getEnemyTileX()) > 50) {
		velocity.x = 0;
		velocity.y = 0;
		pbody->body->SetLinearVelocity(velocity);
		return true;
	}
	velocity = b2Vec2(0, 0);	

	if (!hit) {
		if (canChase(distChase)) {
			ActualVelocity = chaseVelovity;
			dest = iPoint(PTileX, PTileY);
			moveToPlayer(dt);
			currentAnimation = &flyingAir;
		}
		else {
			ActualVelocity = patrolVelocity;
 			Enemy::Patrol();
			moveToPlayer(dt);
			currentAnimation = &flyingAir;
		}
	}
	else {
		if (dieAir.HasFinished()) {
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

void EnemyAir::moveToPlayer(float dt)
{
	const DynArray<iPoint>* path = SearchWay();
	//check if t has arrivied
	if (path->Count() > 1) {
		//check if it shall move to x
		if (TileX != path->At(1)->x) {
			if (TileX > path->At(1)->x) {
				velocity.x = -ActualVelocity * dt;
			}
			else {
				velocity.x = ActualVelocity * dt;
			}
			if (path->Count() > 2) {
				//if next step move y, go diagonal
				if (path->At(2)->y != TileY) {
					if (TileY > path->At(2)->y) {
						velocity.y = -ActualVelocity / 1.3 * dt;
					}
					else {
						velocity.y = ActualVelocity / 1.3 * dt;
					}
					velocity.x /= 1.3;
				}
				else {
					velocity.y = 0;
				}
			}
		}
		else { //check if it shall move to y
			if (TileY > path->At(1)->y) {
				velocity.y = -ActualVelocity * dt;
			}
			else {
				velocity.y = ActualVelocity * dt;
			}
			if (path->Count() > 2) {
				//if next step move x, go diagonal
				if (path->At(2)->x != TileX) {
					if (TileX > path->At(2)->x) {
						velocity.x = -ActualVelocity / 1.3 * dt;
					}
					else {
						velocity.x = ActualVelocity / 1.3 * dt;
					}
					velocity.y /= 1.3;
				}
				else {
					velocity.x = 0;
				}
			}
		}
	}
	else if (path->Count() == 1) {
		if (app->scene->GetPlayer()->position.x < position.x) {
			velocity.x = -ActualVelocity * dt;
		}
		else {
			velocity.x = ActualVelocity * dt;
		}
		if (app->scene->GetPlayer()->position.y < position.y) {
			velocity.y = -ActualVelocity * dt;
		}
		else {
			velocity.y = ActualVelocity * dt;
		}
	}
}

void EnemyAir::OnCollision(PhysBody* physA, PhysBody* physB)
{
	switch (physB->ctype)
	{
	case ColliderType::BARK:
		hit = true;
		currentAnimation = &dieAir;
		if (!dead) {
			app->audio->PlayFx(deathFX);
		}
		pbody->body->SetActive(false);
		app->scene->score += 30;
		break;
	default:
		break;
	}
}
