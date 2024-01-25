#ifndef __MODULE_H__
#define __MODULE_H__

#include "SString.h"

#include "PugiXml/src/pugixml.hpp"

#include "Optick/include/optick.h"

class App;
class GuiControl;

class Module
{
public:

	Module(bool startEnabled) : active(false), isEnabled(startEnabled)
	{}

	void Init()
	{
		active = isEnabled;
	}

	// Called before render is available
	virtual bool Awake(pugi::xml_node config)
	{
		return true;
	}

	// Called before the first frame
	virtual bool Start()
	{
		return true;
	}

	// Called each loop iteration
	virtual bool PreUpdate()
	{
		return true;
	}

	// Called each loop iteration
	virtual bool Update(float dt)
	{
		return true;
	}

	// Called each loop iteration
	virtual bool PostUpdate()
	{
		return true;
	}

	// Called before quitting
	virtual bool CleanUp()
	{
		return true;
	}

	virtual bool LoadState(pugi::xml_node node)
	{
		return true;
	}

	// Called when we want to save data from XML
	virtual bool SaveState(pugi::xml_node node)
	{
		return true;
	}

	virtual bool OnGuiMouseClickEvent(GuiControl* control)
	{
		return true;
	}

	void Enable()
	{
		if (!isEnabled)
		{

			active = true;
			isEnabled = true;

		}
	}

	void Disable()
	{
		if (isEnabled)
		{

			active = false;
			isEnabled = false;

		}
	}

	inline bool IsEnabled() const { return isEnabled; }

public:

	SString name;
	bool active;

	bool isEnabled;
};

#endif // __MODULE_H__