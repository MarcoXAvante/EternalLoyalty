#include "Enemy.h"
#include "Log.h"
#include "Render.h"
#include "Map.h"
#include "Player.h"
#include "Scene.h"

Enemy::Enemy() : Entity(EntityType::ENEMY)
{
	name.Create("Enemy");
}

Enemy::~Enemy()
{
}

bool Enemy::Awake()
{
	position.x = parameters.attribute("x").as_int();
	position.y = parameters.attribute("y").as_int();
	texturePath = parameters.attribute("texturepath").as_string();
	Patrol1 = { parameters.attribute("dest1X").as_int() ,parameters.attribute("dest1Y").as_int() };
	Patrol2 = { parameters.attribute("dest2X").as_int() , parameters.attribute("dest2Y").as_int() };
	dark = parameters.attribute("dark").as_bool();
	return true;
}

bool Enemy::Start()
{
	mouseTileTex = app->tex->Load("Assets/Maps/tileSelection.png");
	dead = false;
	hit = false;
	patrol = true;
	return true;
}

bool Enemy::Update(float dt)
{
	pbody->body->SetLinearVelocity(velocity);
	b2Transform pbodyPos = pbody->body->GetTransform();

	if (hit) {
		b2Vec2 diePos = b2Vec2(PIXEL_TO_METERS(0), PIXEL_TO_METERS(0));
		pbody->body->SetTransform(diePos, 0);
	}
	else {
		position.x = METERS_TO_PIXELS(pbodyPos.p.x) - (currentAnimation->GetCurrentFrame().w / 2);
		position.y = METERS_TO_PIXELS(pbodyPos.p.y) - (currentAnimation->GetCurrentFrame().h / 2);
	}

	app->render->DrawTextureDX(texture, position.x, position.y, flip, &currentAnimation->GetCurrentFrame());
				

 	currentAnimation->Update();
	
	if (dead)
	{
		removable = true;
		// mirar de borrar texture i memory leaks
		//app->entityManager->DestroyEntity(this);
	}

	return true;
}

bool Enemy::CleanUp()
{
	return true;
}

void Enemy::OnCollision(PhysBody* physA, PhysBody* physB)
{
}

int Enemy::getEnemyTileX()
{
	return (position.x + (currentAnimation->GetCurrentFrame().w / 2)) / app->map->getTileWidth();
}

int Enemy::getEnemyTileY()
{
	return (position.y + (currentAnimation->GetCurrentFrame().h / 2)) / app->map->getTileHieght();
}

const DynArray<iPoint>* Enemy::SearchWay()
{
	iPoint origin = iPoint(TileX, TileY);

	app->map->pathfinding->CreatePath(origin, dest);

	// L13: Get the latest calculated path and draw
	const DynArray<iPoint>* path = app->map->pathfinding->GetLastPath();
	if (app->physics->debug) {
		for (uint i = 0; i < path->Count(); ++i)
		{
			iPoint pos = app->map->MapToWorld(path->At(i)->x, path->At(i)->y);
			app->render->DrawTexture(mouseTileTex, pos.x, pos.y);
		}
	}

	return path;
}

bool Enemy::canChase(int dist)
{
	bool canChase = false;

	TileX = getEnemyTileX();
	TileY = getEnemyTileY();

	PTileX = app->scene->GetPlayer()->getPlayerTileX();
	PTileY = app->scene->GetPlayer()->getPlayerTileY();

	if ((abs(TileX - PTileX) + abs(TileY - PTileY)) < dist) {
		canChase = true;
	}

	if (canChase && !app->scene->GetPlayer()->god) {
		canChase =  true;
	}
	else {
		canChase = false;
	}

	return canChase;
}

void Enemy::moveToPlayer(float dt)
{

}

void Enemy::Patrol()
{
	if (patrol) {
		dest.x = Patrol1.x;
		dest.y = Patrol1.y;
		if (getEnemyTileX() == dest.x) {
			patrol = false;
		}
	}
	else {
		dest.x = Patrol2.x;
		dest.y = Patrol2.y;
		if (getEnemyTileX() == dest.x) {
			patrol = true;
		}
	}
}

bool Enemy::LoadState(pugi::xml_node& node) {
	this->position.x = node.attribute("x").as_int();
	this->position.y = node.attribute("y").as_int();
	this->dead = node.attribute("dead").as_bool();
	b2Vec2 pPosition = b2Vec2(PIXEL_TO_METERS(position.x), PIXEL_TO_METERS(position.y));
	pbody->body->SetTransform(pPosition, 0);

	return true;
};