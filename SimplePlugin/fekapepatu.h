#pragma once
#include "../plugin_sdk/plugin_sdk.hpp"

struct PermashowElement
{
	TreeEntry* AssignedMenuElement = nullptr;

	std::string Name;
	std::string Value;
	std::uint32_t ValueColor;

	vector NamePos;
	vector ValuePos;
	vector SeperatorPos;
};

class Permashow
{
public:
	static Permashow Instance;
public:
	bool Initialized = false;

	std::string Title = "PERMASHOW";
	vector TitleBoxSize;
	vector TitleSize;
	float SeperatorHeight;

	point2 box_size;
	point2 drag_offset;

	std::vector<PermashowElement> PermashowElements;

	void AddElement(TreeEntry*);
	void Update();

	void Init();
	void Destroy();
};

Permashow& GetPermashow();