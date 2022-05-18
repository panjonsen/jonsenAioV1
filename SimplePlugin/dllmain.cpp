#include "../plugin_sdk/plugin_sdk.hpp"
#include "Nami.h"
#include "Karma.h"
#include "Gangplank.h"

PLUGIN_NAME("JonSenAio");
SUPPORTED_CHAMPIONS(champion_id::Nami, champion_id::Karma);
PLUGIN_TYPE(plugin_type::champion);
PLUGIN_API bool on_sdk_load(plugin_sdk_core* plugin_sdk_good)
{
	DECLARE_GLOBALS(plugin_sdk_good);
	console->print("Load Script");

	switch (myhero->get_champion())
	{
	case champion_id::Nami:
		Nami::load();
		return true;
	case champion_id::Karma:
		Karma::load();
		return true;
	//case champion_id::Gangplank:
	//	Gangplank::load();
	//	return true;
	//case champion_id::Renata:
	//	Gangplank::load();
	//	return true;
	default:
		break;
	}

	return false;
}

PLUGIN_API void on_sdk_unload()
{
	console->print("unload Script");
	switch (myhero->get_champion())
	{
	case champion_id::Nami:
		Nami::unload();
		return;
	case champion_id::Karma:
		Karma::unload();
		return;
	//case champion_id::Gangplank:
	//	Gangplank::unload();
	//	return;
	default:
		break;
	}
}