#pragma once

#include "node.h"

#include "unit.h"

class WGSUnitHeaderDrawComponent
{
public:
	virtual wgs_vec2i32 computeSize(WGSEUnit* unit) = 0;
	virtual void draw(WGSEUnit* unit) = 0;
};

class WGSUnitPinDrawComponent
{
public:
	virtual wgs_vec2i32 computePinLayout(WGSEUnit* unit, WGSEUnitPin* pin, bool input) = 0;
	virtual void drawPin(WGSEUnit* unit, WGSEUnitPin* pin, bool input) = 0;
	virtual void drawPinConnector(WGSEUnit* unit, WGSEUnitPin* pin, bool input) = 0;
};

class WGSEHoverManager
{
public:
	virtual void frameStart() = 0;
	virtual bool wantHover(WGSEDiagramNode* node) = 0;
	virtual WGSEDiagramNode* getHoveredItem() = 0;
	virtual bool isHovered(WGSEDiagramNode* node) = 0;
	virtual bool isSameTypeHovered(WGSEDiagramNode* node) = 0;
};

class WGSEditor
{
public:
	WGSEditor();
	~WGSEditor();

	void draw();

	WGSEHoverManager* hover() { return hover_mgr_; }

	void startDrag(WGSEUnit* node);
	bool isDragging() { return dragUnit_ ? true : false; }

	void startCreatingLink(WGSEUnitPin* pin);
	void checkDropLink(WGSEUnitPin* pin);
	bool isCreatingLink() { return createLinkPin_ ? true : false; }

	WGSUnitPinDrawComponent* getPinDrawCoponent(WGSEUnitPin* pin);

	bool getPinUnit(WGSEUnitPin* pin, WGSEUnit** unit, wgs_bool* is_input);

	bool isSelected(WGSEUnit* unit);
	void clickSelection(WGSEUnit* unit);

protected:
	void drawCurrent();
	void drawUnit(WGSEUnit* unit);
	void drawLinks();

	bool checkLinking();
	void finishLinking();
protected:
	WGSEDiagram*		current_;

	WGSEUnit*			dragUnit_;
	ImVec2				dragStartPos_;

	WGSEUnitPin*		createLinkPin_;
	WGSEUnitPin*		createLinkDropPin_;

	WGSEUnitList		selection_;

	WGSUnitHeaderDrawComponent* header_draw_;
	WGSUnitPinDrawComponent*	pin_draw_;
	WGSEHoverManager*			hover_mgr_;
};
