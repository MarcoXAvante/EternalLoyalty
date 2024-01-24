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

Scene::Scene() : Module()
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

	//L03: DONE 3b: Instantiate the player using the entity manager
	//L04 DONE 7: Get player paremeters
	player = (Player*) app->entityManager->CreateEntity(EntityType::PLAYER);
	//Assigns the XML node to a member in player
	player->config = config.child("player");
	app->audio->PlayMusic(config.child("music").attribute("path").as_string(), 1.0f);

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
	// Check https://pugixml.org/docs/quickstart.html#access
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

	resume = app->tex->Load("Assets/UI/Resume.png");
	Resume = (GuiButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 1, resume, "", { 125,100,117,24 }, this);
	Resume->state = GuiControlState::DISABLED;

	settings = app->tex->Load("Assets/UI/Settings.png");
	Settings = (GuiButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 2, settings, "", { 125,130,144,24 }, this);
	Settings->state = GuiControlState::DISABLED;

	backToTitle = app->tex->Load("Assets/UI/BackToTitle.png");
	BackToTitle = (GuiButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 3, backToTitle, "", { 125,150,220,24 }, this);
	BackToTitle->state = GuiControlState::DISABLED;

	exit = app->tex->Load("Assets/UI/Exit.png");
	Exit = (GuiButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 4, exit, "", { 125,200,73,24 }, this);
	Exit->state = GuiControlState::DISABLED;

	back = app->tex->Load("Assets/UI/Back.png");
	Back = (GuiButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 5, back, "", { 235,205,56,29 }, this);
	Back->state = GuiControlState::DISABLED;

	slider = app->tex->Load("Assets/UI/Slider.png");

	SliderMusic = (GuiSlider*)app->guiManager->CreateGuiControl(GuiControlType::SLIDER, 7, slider, "", { 621/3,216/3,15,30 }, this);
	SliderMusic->state = GuiControlState::DISABLED;

	SliderFX = (GuiSlider*)app->guiManager->CreateGuiControl(GuiControlType::SLIDER, 8, slider, "", { 621/3,371/3,15,30 }, this);
	SliderFX->state = GuiControlState::DISABLED;

	checkBox = app->tex->Load("Assets/UI/CheckBox.png");

	CheckBoxFullscreen = (GuiCheckBox*)app->guiManager->CreateGuiControl(GuiControlType::CHECKBOX, 9, checkBox, "", { 701/3,468/3,24,24 }, this);
	CheckBoxFullscreen->state = GuiControlState::DISABLED;

	CheckBoxVsync = (GuiCheckBox*)app->guiManager->CreateGuiControl(GuiControlType::CHECKBOX, 10, checkBox, "", { 610/3,566/3,24,24 }, this);
	CheckBoxVsync->state = GuiControlState::DISABLED;

	options = app->tex->Load("Assets/Textures/OptionsScreenFromPause.png");

	pauseMenu = false;

	return true;
}

// Called each loop iteration
bool Scene::PreUpdate()
{
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

	if (app->input->GetKey(SDL_SCANCODE_F5) == KEY_DOWN) {
		app->SaveRequest();
	};
	if (app->input->GetKey(SDL_SCANCODE_F6) == KEY_DOWN) {
		app->LoadRequest();
	};

	if (app->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN || Resume->state == GuiControlState::PRESSED) {

		if (paused) {

			paused = false;
		}
		else {
			paused = true;
		}

		pauseMenu = false;

	}

	if (Settings->state == GuiControlState::PRESSED) {

		pauseMenu = true;
		Back->state = GuiControlState::NORMAL;

	}

	if (Back->state == GuiControlState::PRESSED) {
		pauseMenu = false;
	}

	app->scene->SliderMusic->bounds.x = SliderMusic->bounds.x;
	app->scene->SliderMusic->posx = SliderMusic->posx;

	app->scene->SliderFX->bounds.x = SliderFX->bounds.x;
	app->scene->SliderFX->posx = SliderFX->posx;

	Mix_VolumeMusic((SliderMusic->bounds.x - 324) * (128 - 0) / (674 - 324) + 0);



	return true;
}

// Called each loop iteration
bool Scene::PostUpdate()
{
	bool ret = true;

	if (paused) {

		app->render->DrawTexture(pause, -app->render->camera.x, -app->render->camera.y);

		if (Resume->state == GuiControlState::DISABLED) Resume->state = GuiControlState::NORMAL;
		if (Settings->state == GuiControlState::DISABLED) Settings->state = GuiControlState::NORMAL;
		if (BackToTitle->state == GuiControlState::DISABLED) BackToTitle->state = GuiControlState::NORMAL;
		if (Exit->state == GuiControlState::DISABLED) Exit->state = GuiControlState::NORMAL;

		Resume->Draw(app->render);
		Settings->Draw(app->render);
		BackToTitle->Draw(app->render);
		Exit->Draw(app->render);

		if (pauseMenu) {

			if (Back->state == GuiControlState::DISABLED) Back->state = GuiControlState::NORMAL;
			if (SliderMusic->state == GuiControlState::DISABLED) SliderMusic->state = GuiControlState::NORMAL;
			if (SliderFX->state == GuiControlState::DISABLED) SliderFX->state = GuiControlState::NORMAL;
			if (CheckBoxFullscreen->state == GuiControlState::DISABLED) CheckBoxFullscreen->state = GuiControlState::NORMAL;
			if (CheckBoxVsync->state == GuiControlState::DISABLED) CheckBoxVsync->state = GuiControlState::NORMAL;

			if (Resume->state != GuiControlState::DISABLED) Resume->state = GuiControlState::DISABLED;

			if (Settings->state != GuiControlState::DISABLED) Settings->state = GuiControlState::DISABLED;
			if (BackToTitle->state != GuiControlState::DISABLED) BackToTitle->state = GuiControlState::DISABLED;
			if (Exit->state != GuiControlState::DISABLED) Exit->state = GuiControlState::DISABLED;

			app->render->DrawTexture(options, -app->render->camera.x, -app->render->camera.y);

			Back->Draw(app->render);
			SliderMusic->Draw(app->render);
			SliderFX->Draw(app->render);
			CheckBoxFullscreen->Draw(app->render);
			CheckBoxVsync->Draw(app->render);
		}

		if (!pauseMenu) {

			if (Resume->state == GuiControlState::DISABLED) Resume->state = GuiControlState::NORMAL;
			if (Settings->state == GuiControlState::DISABLED) Settings->state = GuiControlState::NORMAL;
			if (BackToTitle->state == GuiControlState::DISABLED) BackToTitle->state = GuiControlState::NORMAL;
			if (Exit->state == GuiControlState::DISABLED) Exit->state = GuiControlState::NORMAL;

			if (Back->state != GuiControlState::DISABLED) Back->state = GuiControlState::DISABLED;
			if (SliderMusic->state != GuiControlState::DISABLED) SliderMusic->state = GuiControlState::DISABLED;
			if (SliderFX->state != GuiControlState::DISABLED) SliderFX->state = GuiControlState::DISABLED;
			if (CheckBoxFullscreen->state != GuiControlState::DISABLED) CheckBoxFullscreen->state = GuiControlState::DISABLED;
			if (CheckBoxVsync->state != GuiControlState::DISABLED) CheckBoxVsync->state = GuiControlState::DISABLED;

		}

	}

	if (!paused) {

		if (Resume->state != GuiControlState::DISABLED) Resume->state = GuiControlState::DISABLED;
		if (Settings->state != GuiControlState::DISABLED) Settings->state = GuiControlState::DISABLED;
		if (BackToTitle->state != GuiControlState::DISABLED) BackToTitle->state = GuiControlState::DISABLED;
		if (Exit->state != GuiControlState::DISABLED) Exit->state = GuiControlState::DISABLED;

	}

	if (Exit->state == GuiControlState::PRESSED) {
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
