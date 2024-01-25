#pragma once
#include "Module.h"
#include "GuiManager.h"
#include "GuiButton.h"
#include "GuiSlider.h"
#include "GuiCheckBox.h"

struct SDL_Texture;

class SceneMenu : public Module
{
public:
	SceneMenu(bool startEnabled);

	virtual ~SceneMenu();

	// Called before render is available
	bool Awake(pugi::xml_node& config);

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

public:
	int font = -1;
	bool startMusic;
	bool canContinue;
	bool settingsMenu;
	bool showCredits;

	SDL_Texture* background;

	GuiButton* play;
	GuiButton* continueButton;
	GuiButton* settings;
	GuiButton* credits;
	GuiButton* exit;
	GuiButton* back;
	GuiButton* back2;
	GuiSlider* sliderMusic;
	GuiSlider* sliderFX;
	GuiCheckBox* checkBoxFullscreen;
	GuiCheckBox* checkBoxVsync;
	SDL_Texture* playTex;
	SDL_Texture* continueTex;
	SDL_Texture* settingsTex;
	SDL_Texture* creditsTex;
	SDL_Texture* exitTex;
	SDL_Texture* backTex;
	SDL_Texture* sliderTex;
	SDL_Texture* checkBoxTex;

	pugi::xml_document saveFile;
	pugi::xml_parse_result result;
};