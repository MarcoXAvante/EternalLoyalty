#include "App.h"
#include "Input.h"
#include "Textures.h"
#include "Audio.h"
#include "Render.h"
#include "Window.h"
#include "Scene.h"
#include "Map.h"
#include "Item.h"
#include "Physics.h"
#include "Enemy.h"

#include "Defs.h"
#include "Log.h"

#include "SDL_mixer/include/SDL_mixer.h"

Scene::Scene(bool startEnabled) : Module(startEnabled)
{
	name.Create("scene");
}

// Destructor
Scene::~Scene()
{}

// Called before render is available
bool Scene::Awake(pugi::xml_node config)
{
	LOG("Loading Scene");
	bool ret = true;
	this->config = config;
	//L03: DONE 3b: Instantiate the player using the entity manager
	//L04 DONE 7: Get player paremeters
	player = (Player*) app->entityManager->CreateEntity(EntityType::PLAYER);
	//Assigns the XML node to a member in player
	player->config = config.child("player");

	//Get the map name from the config file and assigns the value in the module
	app->map->name = config.child("map").attribute("name").as_string();
	app->map->path = config.child("map").attribute("path").as_string();


	pugi::xml_node enemyNode = config.child("enemy");
	//GroundEnemy
	for (pugi::xml_node groundNode = enemyNode.child("groundenemy"); groundNode; groundNode = groundNode.next_sibling("groundenemy"))
	{
		Enemy* GroundEnemy = (Enemy*)app->entityManager->CreateEntity(EntityType::ENEMYGROUND);
		GroundEnemy->parameters = groundNode;
	}

	//AirEnemy
	for (pugi::xml_node airNode = enemyNode.child("airenemy"); airNode; airNode = airNode.next_sibling("airenemy"))
	{
		Enemy* AirEnemy = (Enemy*)app->entityManager->CreateEntity(EntityType::ENEMYAIR);
		AirEnemy->parameters = airNode;
	}

	// iterate all items in the scene
	// check https://pugixml.org/docs/quickstart.html#access
	for (pugi::xml_node itemNode = config.child("item"); itemNode; itemNode = itemNode.next_sibling("item"))
	{
		Item* item = (Item*)app->entityManager->CreateEntity(EntityType::ITEM);
		item->parameters = itemNode;
	}

	return ret;
}

// Called before the first frame
bool Scene::Start()
{
	// NOTE: We have to avoid the use of paths in the code, we will move it later to a config file
	
	//Music is commented so that you can add your own music
	//app->audio->PlayMusic("Assets/Audio/Music/music_spy.ogg");

	//Get the size of the window
	app->win->GetWindowSize(windowW, windowH);
	//Get the size of the texture
	app->tex->GetSize(img, texW, texH);

	textPosX = (float)windowW / 2 - (float)texW / 2;
	textPosY = (float)windowH / 2 - (float)texH / 2;

	startMusic = true;

	resumeTex = app->tex->Load("Assets/UI/resume.png");
	resume = (GuiButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 1, resumeTex, "", { 125,100,117,24 }, this);
	resume->state = GuiControlState::DISABLED;

	settingsTex = app->tex->Load("Assets/UI/settings.png");
	settings = (GuiButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 2, settingsTex, "", { 125,130,144,24 }, this);
	settings->state = GuiControlState::DISABLED;

	backToTitleTex = app->tex->Load("Assets/UI/backToTitle.png");
	backToTitle = (GuiButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 3, backToTitleTex, "", { 125,165,220,24 }, this);
	backToTitle->state = GuiControlState::DISABLED;

	exitTex = app->tex->Load("Assets/UI/exit.png");
	exit = (GuiButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 4, exitTex, "", { 125,200,73,24 }, this);
	exit->state = GuiControlState::DISABLED;

	backTex = app->tex->Load("Assets/UI/back.png");
	back = (GuiButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 5, backTex, "", { 235,205,56,29 }, this);
	back->state = GuiControlState::DISABLED;

	sliderTex = app->tex->Load("Assets/UI/slider.png");

	sliderMusic = (GuiSlider*)app->guiManager->CreateGuiControl(GuiControlType::SLIDER, 7, sliderTex, "", { 621/3,216/3,15,30 }, this);
	sliderMusic->state = GuiControlState::DISABLED;

	sliderFX = (GuiSlider*)app->guiManager->CreateGuiControl(GuiControlType::SLIDER, 8, sliderTex, "", { 621/3,371/3,15,30 }, this);
	sliderFX->state = GuiControlState::DISABLED;

	checkBoxTex = app->tex->Load("Assets/UI/checkBox.png");

	checkBoxFullscreen = (GuiCheckBox*)app->guiManager->CreateGuiControl(GuiControlType::CHECKBOX, 9, checkBoxTex, "", { 701/3,468/3,24,24 }, this);
	checkBoxFullscreen->state = GuiControlState::DISABLED;

	checkBoxVsync = (GuiCheckBox*)app->guiManager->CreateGuiControl(GuiControlType::CHECKBOX, 10, checkBoxTex, "", { 610/3,566/3,24,24 }, this);
	checkBoxVsync->state = GuiControlState::DISABLED;

	optionsTex = app->tex->Load("Assets/Textures/OptionsScreenFromPause.png");

	pauseMenu = false;

	return true;
}

// Called each loop iteration
bool Scene::PreUpdate()
{
	if (startMusic) {
		app->audio->PlayMusic(config.child("music").attribute("path").as_string(), 1.0f);
		startMusic = false;
	}
	return true;
}

// Called each loop iteration
bool Scene::Update(float dt)
{

	int limitCamX = ((player->position.x * app->win->scale - (windowW / 2)));
	if (limitCamX > 0 && limitCamX < (app->map->getMapWidth() - windowW) * app->win->scale) {
		app->render->camera.x = (player->position.x * app->win->scale - (windowW / 2)) * -1;
	}

	/*
	int limitCamY = player->position.y - (windowH / 2);
	if (limitCamY > 0 && limitCamY < (app->map->getMapHeght() - windowH)) {
		app->render->camera.y = (player->position.y - windowH / 2) * -1;
	}
	*/

	/*
	//L02 DONE 3: Make the camera movement independent of framerate
	float camSpeed = 1; 

	if(app->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT)
		app->render->camera.y -= (int)ceil(camSpeed * dt);

	if(app->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT)
		app->render->camera.y += (int)ceil(camSpeed * dt);

	if(app->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT)
		app->render->camera.x -= (int)ceil(camSpeed * dt);

	if(app->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT)
		app->render->camera.x += (int)ceil(camSpeed * dt);
	*/
	// Renders the image in the center of the screen 
	//app->render->DrawTexture(img, (int)textPosX, (int)textPosY);
	if (!app->entityManager->IsEnabled()) {

		app->entityManager->Enable();

	}


	if (app->input->GetKey(SDL_SCANCODE_F5) == KEY_DOWN) {
		app->SaveRequest();
	};
	if (app->input->GetKey(SDL_SCANCODE_F6) == KEY_DOWN) {
		app->LoadRequest();
	};

	if (app->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN || resume->state == GuiControlState::PRESSED) {

		if (paused) {

			paused = false;
		}
		else {
			paused = true;
		}

		pauseMenu = false;

	}

	if (settings->state == GuiControlState::PRESSED) {

		pauseMenu = true;
		back->state = GuiControlState::NORMAL;

	}

	if (back->state == GuiControlState::PRESSED) {
		pauseMenu = false;
	}

	app->scene->sliderMusic->bounds.x = sliderMusic->bounds.x;
	app->scene->sliderMusic->posx = sliderMusic->posx;

	app->scene->sliderFX->bounds.x = sliderFX->bounds.x;
	app->scene->sliderFX->posx = sliderFX->posx;

	Mix_VolumeMusic((sliderMusic->bounds.x - 324) * (128 - 0) / (674 - 324) + 0);

	for (int i = 0; i < app->audio->fx.Count(); i++) {

		Mix_VolumeChunk(app->audio->fx.At(i)->data, (sliderFX->bounds.x - 324) * (128 - 0) / (674 - 324) + 0);

	}


	return true;
}

// Called each loop iteration
bool Scene::PostUpdate()
{
	bool ret = true;

	if (paused) {

		app->render->DrawTexture(pauseTex, -app->render->camera.x, -app->render->camera.y);

		if (resume->state == GuiControlState::DISABLED) resume->state = GuiControlState::NORMAL;
		if (settings->state == GuiControlState::DISABLED) settings->state = GuiControlState::NORMAL;
		if (backToTitle->state == GuiControlState::DISABLED) backToTitle->state = GuiControlState::NORMAL;
		if (exit->state == GuiControlState::DISABLED) exit->state = GuiControlState::NORMAL;

		resume->Draw(app->render);
		settings->Draw(app->render);
		backToTitle->Draw(app->render);
		exit->Draw(app->render);

		if (pauseMenu) {

			if (back->state == GuiControlState::DISABLED) back->state = GuiControlState::NORMAL;
			if (sliderMusic->state == GuiControlState::DISABLED) sliderMusic->state = GuiControlState::NORMAL;
			if (sliderFX->state == GuiControlState::DISABLED) sliderFX->state = GuiControlState::NORMAL;
			if (checkBoxFullscreen->state == GuiControlState::DISABLED) checkBoxFullscreen->state = GuiControlState::NORMAL;
			if (checkBoxVsync->state == GuiControlState::DISABLED) checkBoxVsync->state = GuiControlState::NORMAL;

			if (resume->state != GuiControlState::DISABLED) resume->state = GuiControlState::DISABLED;

			if (settings->state != GuiControlState::DISABLED) settings->state = GuiControlState::DISABLED;
			if (backToTitle->state != GuiControlState::DISABLED) backToTitle->state = GuiControlState::DISABLED;
			if (exit->state != GuiControlState::DISABLED) exit->state = GuiControlState::DISABLED;

			app->render->DrawTexture(optionsTex, -app->render->camera.x, -app->render->camera.y);

			back->Draw(app->render);
			sliderMusic->Draw(app->render);
			sliderFX->Draw(app->render);
			checkBoxFullscreen->Draw(app->render);
			checkBoxVsync->Draw(app->render);
		}

		if (!pauseMenu) {

			if (resume->state == GuiControlState::DISABLED) resume->state = GuiControlState::NORMAL;
			if (settings->state == GuiControlState::DISABLED) settings->state = GuiControlState::NORMAL;
			if (backToTitle->state == GuiControlState::DISABLED) backToTitle->state = GuiControlState::NORMAL;
			if (exit->state == GuiControlState::DISABLED) exit->state = GuiControlState::NORMAL;

			if (back->state != GuiControlState::DISABLED) back->state = GuiControlState::DISABLED;
			if (sliderMusic->state != GuiControlState::DISABLED) sliderMusic->state = GuiControlState::DISABLED;
			if (sliderFX->state != GuiControlState::DISABLED) sliderFX->state = GuiControlState::DISABLED;
			if (checkBoxFullscreen->state != GuiControlState::DISABLED) checkBoxFullscreen->state = GuiControlState::DISABLED;
			if (checkBoxVsync->state != GuiControlState::DISABLED) checkBoxVsync->state = GuiControlState::DISABLED;

		}

	}

	if (!paused) {

		if (resume->state != GuiControlState::DISABLED) resume->state = GuiControlState::DISABLED;
		if (settings->state != GuiControlState::DISABLED) settings->state = GuiControlState::DISABLED;
		if (backToTitle->state != GuiControlState::DISABLED) backToTitle->state = GuiControlState::DISABLED;
		if (exit->state != GuiControlState::DISABLED) exit->state = GuiControlState::DISABLED;

	}

	if (exit->state == GuiControlState::PRESSED) {
		ret = false;
	}

	return ret;
}

// Called before quitting
bool Scene::CleanUp()
{
	LOG("Freeing scene");

	return true;
}
