#include "EntityManager.h"
#include "Player.h"
#include "EnemyGround.h"
#include "EnemyBoss.h"
#include "EnemyAir.h"
#include "Item.h"
#include "App.h"
#include "Textures.h"
#include "Scene.h"

#include "Defs.h"
#include "Log.h"

EntityManager::EntityManager(bool startEnabled) : Module(startEnabled)
{
	name.Create("entitymanager");
}

// Destructor
EntityManager::~EntityManager()
{}

// Called before render is available
bool EntityManager::Awake(pugi::xml_node config)
{
	LOG("Loading Entity Manager");
	bool ret = true;

	//Iterates over the entities and calls the Awake
	ListItem<Entity*>* item;
	Entity* pEntity = NULL;

	for (item = entities.start; item != NULL && ret == true; item = item->next)
	{
		pEntity = item->data;

		if (pEntity->active == false) continue;
		ret = item->data->Awake();
	}

	return ret;

}

bool EntityManager::Start() {

	bool ret = true; 

	//Iterates over the entities and calls Start
	ListItem<Entity*>* item;
	Entity* pEntity = NULL;

	for (item = entities.start; item != NULL && ret == true; item = item->next)
	{
		pEntity = item->data;

		if (pEntity->active == false) continue;
		ret = item->data->Start();
	}

	return ret;
}

// Called before quitting
bool EntityManager::CleanUp()
{
	bool ret = true;
	ListItem<Entity*>* item;
	item = entities.end;

	while (item != NULL && ret == true && !item->data->active)
	{
		ret = item->data->CleanUp();
		item = item->prev;
	}

	return ret;
}

Entity* EntityManager::CreateEntity(EntityType type)
{
	Entity* entity = nullptr; 

	//L03: DONE 3a: Instantiate entity according to the type and add the new entity to the list of Entities
	switch (type)
	{
	case EntityType::PLAYER:
		entity = new Player();
		break;
	case EntityType::ITEM:
		entity = new Item();
		break;
	case EntityType::ENEMYGROUND:
		entity = new EnemyGround();
		break;
	case EntityType::ENEMYAIR:
		entity = new EnemyAir();
		break;
	case EntityType::ENEMYBOSS:
		entity = new EnemyBoss();
		break;
	default:
		break;
	}

	entities.Add(entity);

	return entity;
}

void EntityManager::DestroyEntity(Entity* entity)
{
	ListItem<Entity*>* item;

	for (item = entities.start; item != NULL; item = item->next)
	{
		if (item->data == entity) entities.Del(item);
	}
}

void EntityManager::AddEntity(Entity* entity)
{
	if ( entity != nullptr) entities.Add(entity);
}

bool EntityManager::Update(float dt)
{
	bool ret = true;
	ListItem<Entity*>* item;
	Entity* pEntity = NULL;

	for (item = entities.start; item != NULL && ret == true; item = item->next)
	{
		pEntity = item->data;

		if (pEntity->active == false) continue;
		ret = item->data->Update(dt);
	}

	return ret;
}

bool EntityManager::LoadState(pugi::xml_node node) {

	pugi::xml_node entityNode = node.child("entities");
	pugi::xml_node enemyNode;
	ListItem<Entity*>* item;
	enemyNode = entityNode.child("enemy");
	for (item = entities.start; item != NULL; item = item->next) {
		if (item->data->type == EntityType::PLAYER) {
			item->data->LoadState(entityNode.child("player"));
		}
		else if (item->data->type == EntityType::ENEMY) {
			item->data->LoadState(enemyNode);
			enemyNode = enemyNode.next_sibling("enemy");
		}
	}
	

	return true;
}

bool EntityManager::SaveState(pugi::xml_node node) {

	pugi::xml_node entityNode = node.append_child("entities");
	pugi::xml_node enemyNode;
	ListItem<Entity*>* item;
	Entity* pEntity = NULL;

	for (item = entities.start; item != NULL; item = item->next) {
		pEntity = item->data;
		
		switch (pEntity->type)
		{
		case EntityType::PLAYER:
			entityNode.append_child("player").append_attribute("x").set_value(pEntity->position.x);
			entityNode.child("player").append_attribute("y").set_value(pEntity->position.y);
			entityNode.child("player").append_attribute("dead").set_value(pEntity->dead);
			break;
		case EntityType::ENEMY:
			enemyNode = entityNode.append_child("enemy");
			enemyNode.append_attribute("x").set_value(pEntity->position.x);
			enemyNode.append_attribute("y").set_value(pEntity->position.y);
			enemyNode.append_attribute("dead").set_value(pEntity->dead);
			break;
		default:
			break;
		}
	}


	return true;
}

void EntityManager::LevelController(int currentlevel) {
	ListItem<Entity*>* item;

	for (item = entities.start; item != NULL; item = item->next)
	{
		if (item->data->currentlevel == currentlevel) {
			item->data->active = true;
		}
		else {
			item->data->active = false;
			item->data->CleanUp();
		}
	}
}