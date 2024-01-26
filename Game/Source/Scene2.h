#pragma once

#include "Module.h"
#include "Player.h"
#include "GuiManager.h"
#include "GuiButton.h"
#include "GuiSlider.h"
#include "GuiCheckBox.h"
#include <iostream>
#include "Timer.h"

struct SDL_Texture;

class Scene2 : public Module
{
public:

	Scene2(bool startEnabled);

	// Destructor
	virtual ~Scene2();

	// Called before render is available
	bool Awake(pugi::xml_node config);

	// Called before the first frame
	bool Start();

	// Called before all Updates
	bool PreUpdate();

	// Called each loop iteration
	bool Update(float dt);

	// Called before all Updates
	bool PostUpdate();

	// Called before quitting
	bool CleanUp();

	Player* GetPlayer() {
		return player;
	}

public:
	SDL_Texture* img;
	float textPosX, textPosY = 0;
	uint texW, texH;
	uint windowW, windowH;

	pugi::xml_node config;
	//L03: DONE 3b: Declare a Player attribute
	Player* player;

	bool pauseMenu;
	bool paused;
	bool startMusic;
	GuiButton* resume;
	GuiButton* settings;
	GuiButton* backToTitle;
	GuiButton* exit;
	GuiButton* back;
	GuiSlider* sliderMusic;
	GuiSlider* sliderFX;
	GuiCheckBox* checkBoxFullscreen;
	GuiCheckBox* checkBoxVsync;
	SDL_Texture* pauseTex;
	SDL_Texture* optionsTex;
	SDL_Texture* resumeTex;
	SDL_Texture* settingsTex;
	SDL_Texture* backToTitleTex;
	SDL_Texture* exitTex;
	SDL_Texture* backTex;
	SDL_Texture* sliderTex;
	SDL_Texture* checkBoxTex;

	Timer time;
	int score;
};
