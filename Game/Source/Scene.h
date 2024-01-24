#ifndef __SCENE_H__
#define __SCENE_H__

#include "Module.h"
#include "Player.h"
#include "GuiManager.h"
#include "GuiButton.h"
#include "GuiSlider.h"
#include "GuiCheckBox.h"
#include <iostream>

struct SDL_Texture;

class Scene : public Module
{
public:

	Scene();

	// Destructor
	virtual ~Scene();

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

private:
	GuiButton* Resume;
	GuiButton* Settings;
	GuiButton* BackToTitle;
	GuiButton* Exit;
	GuiButton* Back;
	GuiSlider* SliderMusic;
	GuiSlider* SliderFX;
	GuiCheckBox* CheckBoxFullscreen;
	GuiCheckBox* CheckBoxVsync;
	SDL_Texture* pause;
	SDL_Texture* options;
	SDL_Texture* resume;
	SDL_Texture* settings;
	SDL_Texture* backToTitle;
	SDL_Texture* exit;
	SDL_Texture* back;
	SDL_Texture* slider;
	SDL_Texture* checkBox;
};

#endif // __SCENE_H__