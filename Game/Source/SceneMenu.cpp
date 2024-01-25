#include "App.h"
#include "Audio.h"
#include "FadeToBlack.h"
#include "Fonts.h"
#include "Map.h"
#include "SceneMenu.h"
#include "Scene.h"
#include "Window.h"
#include "Player.h"

#include "Defs.h"
#include "Log.h"

#include "SDL/include/SDL.h"
#include "SDL_mixer/include/SDL_mixer.h"

SceneMenu::SceneMenu(bool startEnabled) : Module(startEnabled)
{
	name.Create("sceneMenu");
}

SceneMenu::~SceneMenu()
{}

bool SceneMenu::Awake(pugi::xml_node& config) {
	LOG("Loading SceneMenu");
	bool ret = true;

	return ret;
}

bool SceneMenu::Start() {
	char lookupTable[] = { "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789.,“”’?!_#$%()+-/:;<=>[*]^{|}~ ©" };

	font = app->fonts->Load("Assets/Fonts/Oswald.ttf", lookupTable, 1);

	background = app->tex->Load("Assets/Textures/Fondos/menu.png");

	playTex = app->tex->Load("Assets/UI/play.png");
	continueTex = app->tex->Load("Assets/UI/continue.png");
	settingsTex = app->tex->Load("Assets/UI/settings.png");
	creditsTex = app->tex->Load("Assets/UI/credits.png");
	exitTex = app->tex->Load("Assets/UI/exit.png");
	backTex = app->tex->Load("Assets/UI/back.png");
	sliderTex = app->tex->Load("Assets/UI/slider.png");
	checkBoxTex = app->tex->Load("Assets/UI/checkbox.png");
	
	play = (GuiButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 1, playTex, "", { 427/3 + 130, 375/3, 160/2, 49/2 }, this);
	continueButton = (GuiButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 2, continueTex, "", { 360/3 + 120,452/3,301/2,49/2 }, this);
	settings = (GuiButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 3, settingsTex, "", { 365/3 + 120,530/3,288/2,49/2 }, this);
	credits = (GuiButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 4, creditsTex, "", { 388/3 + 120,607/3,248/2,49/2 }, this);
	exit = (GuiButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 5, exitTex, "", { 438/3 + 120,679/3,145/2,49/2 }, this);
	back = (GuiButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 6, backTex, "", { 704/3 + 120,619/3,112/2,59/2 }, this);
	back2 = (GuiButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 6, backTex, "", { 704/3 + 120,619/3,112/2,59/2 }, this);
	sliderMusic = (GuiSlider*)app->guiManager->CreateGuiControl(GuiControlType::SLIDER, 7, sliderTex, "", { 621/3,220/3,30/2,59/2 }, this);
	sliderFX = (GuiSlider*)app->guiManager->CreateGuiControl(GuiControlType::SLIDER, 8, sliderTex, "", { 621/3,375/3,30/2,59/2 }, this);
	checkBoxFullscreen = (GuiCheckBox*)app->guiManager->CreateGuiControl(GuiControlType::CHECKBOX, 9, checkBoxTex, "", { 701/3,472/3,48/2,47/2 }, this);
	checkBoxVsync = (GuiCheckBox*)app->guiManager->CreateGuiControl(GuiControlType::CHECKBOX, 10, checkBoxTex, "", { 610/3,570/3,48/2,47/2 }, this);
	continueButton->state = GuiControlState::DISABLED;
	back->state = GuiControlState::DISABLED;
	back2->state = GuiControlState::DISABLED;
	sliderMusic->state = GuiControlState::DISABLED;
	sliderFX->state = GuiControlState::DISABLED;
	checkBoxFullscreen->state = GuiControlState::DISABLED;
	checkBoxVsync->state = GuiControlState::DISABLED;

    result = saveFile.load_file("save_game.xml");

	settingsMenu = false;
	showCredits = false;
	canContinue = false;
	startMusic = true;

	return true;
}

bool SceneMenu::PreUpdate() {
	OPTICK_EVENT();
	app->entityManager->Disable();

	if (startMusic) {
		app->audio->PlayMusic("", 1.0f);
		startMusic = false;
	}

	return true;
}

bool SceneMenu::Update(float dt) {
	OPTICK_EVENT();

	if (result) {
		canContinue = true;
	}
	else {
		canContinue = false;
	}

	if (play->state == GuiControlState::PRESSED) {
		startMusic = true;

		app->scene->time.Start();
		app->scene->score = 0;

		app->scene->player->position = app->scene->player->initialPos;

		app->fadeToBlack->Fade(this, (Module*)app->scene, 0);
		app->map->Enable();

	}

	if (continueButton->state == GuiControlState::PRESSED) {
		startMusic = true;

		app->fadeToBlack->Fade(this, (Module*)app->scene, 0);
		app->map->Enable();

		app->LoadRequest();
	}

	if (settings->state == GuiControlState::PRESSED) {
		settingsMenu = true;
		back->state = GuiControlState::NORMAL;
	}

	if (back->state == GuiControlState::PRESSED) {
		settingsMenu = false;
	}

	if (credits->state == GuiControlState::PRESSED) {
		showCredits = true;
		back2->state = GuiControlState::NORMAL;
	}

	if (back2->state == GuiControlState::PRESSED) {
		showCredits = false;
	}

	if (checkBoxFullscreen->crossed) {

		app->scene->checkBoxFullscreen->crossed = true;
		SDL_SetWindowFullscreen(app->win->window, SDL_WINDOW_FULLSCREEN_DESKTOP);
		SDL_RenderSetLogicalSize(app->render->renderer, 1140, 640);

	} else {

		app->scene->checkBoxFullscreen->crossed = false;
		SDL_SetWindowFullscreen(app->win->window, 0);

	}

	if (checkBoxVsync->crossed) {
		app->scene->checkBoxVsync->crossed = true;
		SDL_GL_SetSwapInterval(1);

	} else {

		app->scene->checkBoxVsync->crossed = false;
		SDL_GL_SetSwapInterval(0);

	}

	app->scene->sliderMusic->bounds.x = sliderMusic->bounds.x;
	app->scene->sliderMusic->posx = sliderMusic->posx;

	app->scene->sliderFX->bounds.x = sliderFX->bounds.x;
	app->scene->sliderFX->posx = sliderFX->posx;

	Mix_VolumeMusic((sliderMusic->bounds.x - 212) * (128) / (387 - 212));

	for (int i = 0; i < app->audio->fx.Count(); i++) {

		Mix_VolumeChunk(app->audio->fx.At(i)->data, (sliderFX->bounds.x - 212) * (128) / (387 - 212));

	}

	return true;
}

bool SceneMenu::PostUpdate() {
	OPTICK_EVENT();
	bool ret = true;

	app->render->DrawTexture(background, app->win->width / 8, 0);

	app->guiManager->Draw();


	if (settingsMenu) {

		if (back->state == GuiControlState::DISABLED) back->state = GuiControlState::NORMAL;
		if (sliderMusic->state == GuiControlState::DISABLED) sliderMusic->state = GuiControlState::NORMAL;
		if (sliderFX->state == GuiControlState::DISABLED) sliderFX->state = GuiControlState::NORMAL;
		if (checkBoxFullscreen->state == GuiControlState::DISABLED) checkBoxFullscreen->state = GuiControlState::NORMAL;
		if (checkBoxVsync->state == GuiControlState::DISABLED) checkBoxVsync->state = GuiControlState::NORMAL;

		if (play->state != GuiControlState::DISABLED) play->state = GuiControlState::DISABLED;
		if (continueButton->state != GuiControlState::DISABLED) continueButton->state = GuiControlState::DISABLED;
		if (settings->state != GuiControlState::DISABLED) settings->state = GuiControlState::DISABLED;
		if (credits->state != GuiControlState::DISABLED) credits->state = GuiControlState::DISABLED;
		if (exit->state != GuiControlState::DISABLED) exit->state = GuiControlState::DISABLED;

		//app->render->DrawTexture(settingsTex, 0, 0);

		back->Draw(app->render);
		sliderMusic->Draw(app->render);
		sliderFX->Draw(app->render);
		checkBoxFullscreen->Draw(app->render);
		checkBoxVsync->Draw(app->render);

	} else {

		if (play->state == GuiControlState::DISABLED) play->state = GuiControlState::NORMAL;
		if (continueButton->state == GuiControlState::DISABLED && canContinue) continueButton->state = GuiControlState::NORMAL;
		if (settings->state == GuiControlState::DISABLED) settings->state = GuiControlState::NORMAL;
		if (credits->state == GuiControlState::DISABLED) credits->state = GuiControlState::NORMAL;
		if (exit->state == GuiControlState::DISABLED) exit->state = GuiControlState::NORMAL;

		if (back->state != GuiControlState::DISABLED) back->state = GuiControlState::DISABLED;
		if (sliderMusic->state != GuiControlState::DISABLED) sliderMusic->state = GuiControlState::DISABLED;
		if (sliderFX->state != GuiControlState::DISABLED) sliderFX->state = GuiControlState::DISABLED;
		if (checkBoxFullscreen->state != GuiControlState::DISABLED) checkBoxFullscreen->state = GuiControlState::DISABLED;
		if (checkBoxVsync->state != GuiControlState::DISABLED) checkBoxVsync->state = GuiControlState::DISABLED;

	}

	if (showCredits) {

		if (back2->state == GuiControlState::DISABLED) back2->state = GuiControlState::NORMAL;

		if (play->state != GuiControlState::DISABLED) play->state = GuiControlState::DISABLED;
		if (continueButton->state != GuiControlState::DISABLED) continueButton->state = GuiControlState::DISABLED;
		if (settings->state != GuiControlState::DISABLED) settings->state = GuiControlState::DISABLED;
		if (credits->state != GuiControlState::DISABLED) credits->state = GuiControlState::DISABLED;
		if (exit->state != GuiControlState::DISABLED) exit->state = GuiControlState::DISABLED;

		back2->Draw(app->render);
	}
	else {

		if (play->state == GuiControlState::DISABLED) play->state = GuiControlState::NORMAL;
		if (continueButton->state == GuiControlState::DISABLED && canContinue) continueButton->state = GuiControlState::NORMAL;
		if (settings->state == GuiControlState::DISABLED) settings->state = GuiControlState::NORMAL;
		if (credits->state == GuiControlState::DISABLED) credits->state = GuiControlState::NORMAL;
		if (exit->state == GuiControlState::DISABLED) exit->state = GuiControlState::NORMAL;

		if (back2->state != GuiControlState::DISABLED) back2->state = GuiControlState::DISABLED;

	}

	if (exit->state == GuiControlState::PRESSED || app->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN) {
		ret = false;
	}

	return ret;
}

bool SceneMenu::CleanUp() {
	LOG("Cleaning menu");

	return true;
}