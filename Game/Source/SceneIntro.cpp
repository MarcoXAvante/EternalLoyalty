#include "App.h"
#include "Textures.h"
#include "SceneIntro.h"
#include "SceneMenu.h"
#include "FadeToBlack.h"
#include "Map.h"
#include "Window.h"

#include "Defs.h"
#include "Log.h"

SceneIntro::SceneIntro(bool startEnabled) : Module(startEnabled)
{
	name.Create("sceneIntro");
}

SceneIntro::~SceneIntro()
{}

bool SceneIntro::Awake(pugi::xml_node& config) {
	LOG("Loading SceneIntro");
	bool ret = true;

	return ret;
}

bool SceneIntro::Start() {
	logo = app->tex->Load("Assets/Textures/Fondos/nocturnals.png");
	return true;
}

bool SceneIntro::PreUpdate() {
	OPTICK_EVENT();
	app->entityManager->Disable();
	return true;
}

bool SceneIntro::Update(float dt) {
	OPTICK_EVENT();
	if (app->input->GetKey(SDL_SCANCODE_RETURN) == KEY_DOWN) {
		app->fadeToBlack->Fade(this, (Module*)app->sceneMenu);
		app->sceneMenu->Start();
	}
	return true;
}

bool SceneIntro::PostUpdate() {
	OPTICK_EVENT();

	bool ret = true;

	app->render->DrawTexture(logo, 3, 1.5);

	if (app->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN) {
		ret = false;
	}

	return ret;
}

bool SceneIntro::CleanUp() {
	LOG("Cleaning scene");

	return true;
}