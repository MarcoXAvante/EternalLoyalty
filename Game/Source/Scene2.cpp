#include "App.h"
#include "Input.h"
#include "Textures.h"
#include "Audio.h"
#include "FadeToBlack.h"
#include "Render.h"
#include "Window.h"
#include "SceneMenu.h"
#include "Scene.h"
#include "Scene2.h"
#include "WinScene.h"
#include "Map.h"
#include "Item.h"
#include "Physics.h"
#include "Enemy.h"

#include "Defs.h"
#include "Log.h"

#include "SDL_mixer/include/SDL_mixer.h"

Scene2::Scene2(bool startEnabled) : Module(startEnabled)
{
	name.Create("scene2");
	this->active = startEnabled;
}

// Destructor
Scene2::~Scene2()
{}

// Called before render is available
bool Scene2::Awake(pugi::xml_node config)
{
	LOG("Loading Scene");
	bool ret = true;
	this->config = config;
	//L03: DONE 3b: Instantiate the player using the entity manager
	// 

	//Get the map name from the config file and assigns the value in the module
	//Get the size of the window
	app->win->GetWindowSize(windowW, windowH);
	//Get the size of the texture
	app->tex->GetSize(img, texW, texH);

	//L04 DONE 7: Get player paremeters
	this->player = app->scene->GetPlayer();


	pugi::xml_node enemyNode = config.child("enemy");
	//GroundEnemy
	for (pugi::xml_node groundNode = enemyNode.child("groundenemy"); groundNode; groundNode = groundNode.next_sibling("groundenemy"))
	{
		Enemy* GroundEnemy = (Enemy*)app->entityManager->CreateEntity(EntityType::ENEMYGROUND);
		GroundEnemy->parameters = groundNode;
		GroundEnemy->currentlevel = 2;
	}

	//AirEnemy
	for (pugi::xml_node airNode = enemyNode.child("airenemy"); airNode; airNode = airNode.next_sibling("airenemy"))
	{
		Enemy* AirEnemy = (Enemy*)app->entityManager->CreateEntity(EntityType::ENEMYAIR);
		AirEnemy->parameters = airNode;
		AirEnemy->currentlevel = 2;
	}
	//BossEnemy
	for (pugi::xml_node bossNode = enemyNode.child("bossenemy"); bossNode; bossNode = bossNode.next_sibling("bossnode")) {
		Enemy* BossEnemy = (Enemy*)app->entityManager->CreateEntity(EntityType::ENEMYBOSS);
		BossEnemy->parameters = bossNode;
		BossEnemy->currentlevel = 2;
	}


	pugi::xml_node itemNode = config.child("item");
	//Cookies
	for (pugi::xml_node cookieNode = itemNode.child("cookie"); cookieNode; cookieNode = cookieNode.next_sibling("cookie"))
	{
		Item* item = (Item*)app->entityManager->CreateEntity(EntityType::ITEM);
		item->parameters = cookieNode;
		item->type = ItemType::COOKIE;
		item->currentlevel = 2;
	}
	//Lives
	for (pugi::xml_node lifeNode = itemNode.child("life"); lifeNode; lifeNode = lifeNode.next_sibling("life"))
	{
		Item* item = (Item*)app->entityManager->CreateEntity(EntityType::ITEM);
		item->parameters = lifeNode;
		item->type = ItemType::LIFE;
		item->currentlevel = 2;
	}
	//Checkpoints
	for (pugi::xml_node checkNode = itemNode.child("checkpoint"); checkNode; checkNode = checkNode.next_sibling("checkpoint"))
	{
		Item* item = (Item*)app->entityManager->CreateEntity(EntityType::ITEM);
		item->parameters = checkNode;
		item->type = ItemType::CHECKPOINT;
		item->currentlevel = 2;
	}

	return ret;
}

// Called before the first frame
bool Scene2::Start()
{
	// NOTE: We have to avoid the use of paths in the code, we will move it later to a config file

	//Music is commented so that you can add your own music
	//app->audio->PlayMusic("Assets/Audio/Music/music_spy.ogg");
	time.Start();
	player->active = true;
	player->currentlevel = 2;
	app->entityManager->LevelController(2);
	app->entityManager->Start();


	player->pbody->body->SetTransform(b2Vec2(PIXEL_TO_METERS(32), PIXEL_TO_METERS(88)),player->pbody->body->GetAngle());

	b2Transform pbodyPos = player->pbody->body->GetTransform();
	player->position.x = METERS_TO_PIXELS(pbodyPos.p.x) - 48 / 2;
	player->position.y = METERS_TO_PIXELS(pbodyPos.p.y) - 32 / 2;

	app->render->camera.y = ((player->position.y - texH / 2) - (app->scene->windowH / 2)) * -1;
	app->render->camera.x = ((player->position.x - texW / 2) - (app->scene->windowW / 2)) * -1;


	if (app->render->camera.x >= 0) {
		app->render->camera.x = 0;
	}
	if (app->render->camera.y >= 0) {
		app->render->camera.y = 0;
	}


	startMusic = true;

	resumeTex = app->tex->Load("Assets/UI/resume.png");
	settingsTex = app->tex->Load("Assets/UI/settings.png");
	backToTitleTex = app->tex->Load("Assets/UI/backToTitle.png");
	exitTex = app->tex->Load("Assets/UI/exit.png");
	backTex = app->tex->Load("Assets/UI/back.png");
	sliderTex = app->tex->Load("Assets/UI/slider.png");
	checkBoxTex = app->tex->Load("Assets/UI/checkBox.png");
	optionsTex = app->tex->Load("Assets/Textures/Fondos/menusetting.png");

	resume = (GuiButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 1, resumeTex, "", { 125,100,117,24 }, this);
	settings = (GuiButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 2, settingsTex, "", { 125,130,144,24 }, this);
	backToTitle = (GuiButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 3, backToTitleTex, "", { 125,165,220,24 }, this);
	exit = (GuiButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 4, exitTex, "", { 125,200,73,24 }, this);
	back = (GuiButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 5, backTex, "", { 355,257,112 / 2,59 / 2 }, this);
	sliderMusic = (GuiSlider*)app->guiManager->CreateGuiControl(GuiControlType::SLIDER, 6, sliderTex, "", { 621 / 3,78,30 / 2,59 / 2 }, this);
	sliderFX = (GuiSlider*)app->guiManager->CreateGuiControl(GuiControlType::SLIDER, 7, sliderTex, "", { 621 / 3,150,30 / 2,59 / 2 }, this);
	checkBoxFullscreen = (GuiCheckBox*)app->guiManager->CreateGuiControl(GuiControlType::CHECKBOX, 8, checkBoxTex, "", { 380,192,48 / 2,47 / 2 }, this);
	checkBoxVsync = (GuiCheckBox*)app->guiManager->CreateGuiControl(GuiControlType::CHECKBOX, 9, checkBoxTex, "", { 270,232,48 / 2,47 / 2 }, this);

	checkBoxFullscreen->state = app->scene->checkBoxFullscreen->state;
	checkBoxVsync->state = app->scene->checkBoxVsync->state;

	resume->state = GuiControlState::DISABLED;
	settings->state = GuiControlState::DISABLED;
	backToTitle->state = GuiControlState::DISABLED;
	exit->state = GuiControlState::DISABLED;
	back->state = GuiControlState::DISABLED;
	sliderMusic->state = GuiControlState::DISABLED;
	sliderFX->state = GuiControlState::DISABLED;
	checkBoxFullscreen->state = GuiControlState::DISABLED;
	checkBoxVsync->state = GuiControlState::DISABLED;


	pauseMenu = false;

	return true;
}

// Called each loop iteration
bool Scene2::PreUpdate()
{

	if (startMusic) {
		app->audio->PlayMusic(config.child("music").attribute("path").as_string(), 1.0f);
		startMusic = false;
	}
	return true;
}

// Called each loop iteration
bool Scene2::Update(float dt)
{

	int limitCamX = ((player->position.x * app->win->scale - (windowW / 2)));
	if (limitCamX > 0 && limitCamX < (app->map->getMapWidth() - windowW / 2) * app->win->scale) {
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

	if (backToTitle->state == GuiControlState::PRESSED) {

		paused = false;

		if (resume->state != GuiControlState::DISABLED) resume->state = GuiControlState::DISABLED;
		if (settings->state != GuiControlState::DISABLED) settings->state = GuiControlState::DISABLED;
		if (backToTitle->state != GuiControlState::DISABLED) backToTitle->state = GuiControlState::DISABLED;
		if (exit->state != GuiControlState::DISABLED) exit->state = GuiControlState::DISABLED;

		startMusic = true;

		app->fadeToBlack->Fade(this, (Module*)app->sceneMenu, 0);
		app->map->Disable();
	}

	if (settings->state == GuiControlState::PRESSED) {

		pauseMenu = true;
		back->state = GuiControlState::NORMAL;

	}

	if (back->state == GuiControlState::PRESSED) {
		pauseMenu = false;
	}

	if (checkBoxFullscreen->crossed) {

		app->sceneMenu->checkBoxFullscreen->crossed = true;
		SDL_SetWindowFullscreen(app->win->window, SDL_WINDOW_FULLSCREEN_DESKTOP);
		SDL_RenderSetLogicalSize(app->render->renderer, 1140, 640);

	}
	else {

		app->sceneMenu->checkBoxFullscreen->crossed = false;
		SDL_SetWindowFullscreen(app->win->window, 0);

	}

	if (checkBoxVsync->crossed) {
		app->sceneMenu->checkBoxVsync->crossed = true;
		SDL_GL_SetSwapInterval(1);

	}
	else {

		app->sceneMenu->checkBoxVsync->crossed = false;
		SDL_GL_SetSwapInterval(0);

	}


	if (player->position.x > 5500) {
		app->map->CleanUp();
		app->map->Disable();
		app->entityManager->Disable();
		app->entityManager->CleanUp();
		app->fadeToBlack->Fade(this, app->winscene, 0);
		app->winscene->Start();
	}

	app->sceneMenu->sliderMusic->bounds.x = sliderMusic->bounds.x;
	app->sceneMenu->sliderMusic->posx = sliderMusic->posx;

	app->sceneMenu->sliderFX->bounds.x = sliderFX->bounds.x;
	app->sceneMenu->sliderFX->posx = sliderFX->posx;

	Mix_VolumeMusic((sliderMusic->bounds.x - 212) * (128) / (387 - 212));

	for (int i = 0; i < app->audio->fx.Count(); i++) {

		Mix_VolumeChunk(app->audio->fx.At(i)->data, (sliderFX->bounds.x - 212) * (128) / (387 - 212));

	}


	return true;
}

// Called each loop iteration
bool Scene2::PostUpdate()
{
	bool ret = true;

	app->render->DrawText("Score: " + std::to_string((int)score), 10, 10, 300, 70, { 0,0,0 });

	app->render->DrawText("Lives: " + std::to_string((int)player->lives), 460, 10, 300, 70, { 0,0,0 });

	if (time.ReadSec() < 10) {

		app->render->DrawText(std::to_string((int)(time.ReadSec())), 1050, 20, 40, 70, { 0,0,0 });

	}
	else if (time.ReadSec() >= 10 && time.ReadSec() < 100) {

		app->render->DrawText(std::to_string((int)(time.ReadSec())), 1020, 20, 70, 70, { 0,0,0 });

	}
	else {

		app->render->DrawText(std::to_string((int)(time.ReadSec())), 990, 20, 100, 70, { 0,0,0 });

	}

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

			app->render->DrawTexture(optionsTex, -app->render->camera.x + 150, -app->render->camera.y);

			back->Draw(app->render);
			sliderMusic->Draw(app->render);
			sliderFX->Draw(app->render);
			checkBoxFullscreen->Draw(app->render);
			checkBoxVsync->Draw(app->render);
		}
		else {

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
	else {

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
bool Scene2::CleanUp()
{
	LOG("Freeing scene");

	return true;
}
