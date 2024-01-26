#include "App.h"
#include "Textures.h"
#include "Scene.h"
#include "GameOverScene.h"
#include "FadeToBlack.h"
#include "Map.h"
#include "Window.h"

#include "Defs.h"
#include "Log.h"

GameOverScene::GameOverScene(bool startEnabled) : Module(startEnabled)
{
	name.Create("sceneIntro");
}

GameOverScene::~GameOverScene()
{}

bool GameOverScene::Awake(pugi::xml_node& config) {
	LOG("Loading SceneIntro");
	bool ret = true;

	return ret;
}

bool GameOverScene::Start() {
	logo = app->tex->Load("Assets/Textures/Fondos/gameover.png");
	return true;
}

bool GameOverScene::PreUpdate() {
	OPTICK_EVENT();
	app->entityManager->Disable();
	return true;
}

bool GameOverScene::Update(float dt) {
	OPTICK_EVENT();
	if (app->input->GetKey(SDL_SCANCODE_RETURN) == KEY_DOWN) {
		app->scene->player->lives = 3;
		app->fadeToBlack->Fade(this, (Module*)app->sceneMenu);
	}
	return true;
}

bool GameOverScene::PostUpdate() {
	OPTICK_EVENT();

	bool ret = true;

	app->render->DrawTexture(logo, app->win->width / 8, 0);

	if (app->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN) {
		ret = false;
	}

	return ret;
}

bool GameOverScene::CleanUp() {
	LOG("Cleaning scene");

	return true;
}