#include "App.h"
#include "Textures.h"
#include "Audio.h"
#include "Scene.h"
#include "WinScene.h"
#include "FadeToBlack.h"
#include "Map.h"
#include "Window.h"

#include "Defs.h"
#include "Log.h"

WinScene::WinScene(bool startEnabled) : Module(startEnabled)
{
	name.Create("sceneIntro");
}

WinScene::~WinScene()
{}

bool WinScene::Awake(pugi::xml_node& config) {
	LOG("Loading SceneIntro");
	bool ret = true;

	return ret;
}

bool WinScene::Start() {
	logo = app->tex->Load("Assets/Textures/Fondos/victory.png");
	return true;
}

bool WinScene::PreUpdate() {
	OPTICK_EVENT();
	if (startMusic) {
		app->audio->PlayMusic("Assets/Audio/Music/win.wav", 1.0f);
		startMusic = false;
	}
	app->entityManager->Disable();
	return true;
}

bool WinScene::Update(float dt) {
	OPTICK_EVENT();
	if (app->input->GetKey(SDL_SCANCODE_RETURN) == KEY_DOWN) {
		app->scene->player->lives = 3;
		app->fadeToBlack->Fade(this, (Module*)app->sceneMenu);
	}
	return true;
}

bool WinScene::PostUpdate() {
	OPTICK_EVENT();

	bool ret = true;

	app->render->DrawTexture(logo, app->win->width / 8, 0);

	if (app->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN) {
		ret = false;
	}

	return ret;
}

bool WinScene::CleanUp() {
	LOG("Cleaning scene");

	return true;
}