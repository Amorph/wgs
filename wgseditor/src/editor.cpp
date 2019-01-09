#include "imgui.h"

#include <limits.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <assert.h>

#define ASSERT(expr) assert(expr)


#include "editor.h"
#include "utils.h"

class WGSUnitHeaderDrawComponentCommon : public WGSUnitHeaderDrawComponent
{
	const wgs_vec2i32 HEADER_EXTENT = { 6, 6 };
public:
	WGSUnitHeaderDrawComponentCommon(WGSEditor* editor) : ed_(editor) {}

	wgs_vec2i32 computeSize(WGSEUnit* unit) override
	{
		ImVec2 title_text_size = ImGui::CalcTextSize(unit->name().c_str());
		return{ (wgs_int32)title_text_size.x + HEADER_EXTENT.x * 2, (wgs_int32)title_text_size.y + HEADER_EXTENT.y * 2 };
	}

	void draw(WGSEUnit* unit) override
	{
		ImVec2 canvas_pos = ImGui::GetCursorScreenPos();       
		ImVec2 canvas_size = ImGui::GetContentRegionAvail();

		ImDrawList* header_draw_list = ImGui::GetWindowDrawList();
		ImVec2 title_text_size = ImGui::CalcTextSize(unit->name().c_str());
		ImVec2 header_pos = canvas_pos;
		header_pos.x += canvas_size.x * 0.5f - title_text_size.x * 0.5f;
		header_pos.y += canvas_size.y * 0.5f - title_text_size.y * 0.5f;

		header_draw_list->AddRectFilled(canvas_pos, ImVec2(canvas_pos.x + canvas_size.x, canvas_pos.y + canvas_size.y), IM_COL32(64, 128, 255, 255), 4, 3);
		header_draw_list->AddText(header_pos, IM_COL32(255, 255, 255, 255), unit->name().c_str());
	}
protected:
	WGSEditor* ed_;
};

class WGSUnitPinDrawComponentBase : public WGSUnitPinDrawComponent
{
protected:
	const wgs_vec2i32 PIN_EXTENT = { 7, 5 };

public:
	WGSUnitPinDrawComponentBase(WGSEditor* editor) : ed_(editor){}

	wgs_vec2i32 computePinLayout(WGSEUnit* unit, WGSEUnitPin* pin, bool input) override
	{
		ImVec2 text_size = ImGui::CalcTextSize(pin->name().c_str());
		return{ (wgs_int32)text_size.x + PIN_EXTENT.x * 2, (wgs_int32)text_size.y + PIN_EXTENT.y * 2 };
	}

	void drawPin(WGSEUnit* unit, WGSEUnitPin* pin, bool input) override
	{
		ImVec2 text_size = ImGui::CalcTextSize(pin->name().c_str());

		ImVec2 canvas_pos = ImGui::GetCursorScreenPos();
		ImVec2 canvas_size = ImGui::GetContentRegionAvail();

		ImDrawList* dlist = ImGui::GetWindowDrawList();
		//Debug rect
		//dlist->AddRect(canvas_pos, ImVec2(canvas_pos.x + canvas_size.x, canvas_pos.y + canvas_size.y), IM_COL32(255, 255, 255, 255));
		ImVec2 name_pos = canvas_pos;
		name_pos.y += canvas_size.y * 0.5f - text_size.y * 0.5f;
		if (!input)
		{
			name_pos.x = canvas_pos.x + canvas_size.x - text_size.x - PIN_EXTENT.x;
		}
		else
		{
			name_pos.x += PIN_EXTENT.x;
		}
		dlist->AddText(name_pos, IM_COL32(255, 255, 255, 255), pin->name().c_str());
		if (!ed_->isDragging() && ImGui::IsMouseHoveringRect(ImVec2(pin->ed().connector_screen_pos.x, pin->ed().connector_screen_pos.y), ImVec2(pin->ed().connector_screen_pos.z, pin->ed().connector_screen_pos.w),false))
		{
			if(ImGui::IsMouseDown(0))
			{
				if (!ed_->isCreatingLink())
					ed_->startCreatingLink(pin);
				else
					ed_->checkDropLink(pin);
			}
			if(!ed_->isCreatingLink())
				ImGui::SetMouseCursor(ImGuiMouseCursor_Cross);
		}
	}

	void drawPinConnector(WGSEUnit* unit, WGSEUnitPin* pin, bool input) override
	{
		ImDrawList* dlist = ImGui::GetWindowDrawList();
		wgs_vec4i32& pin_cursor = pin->ed().connector_screen_pos;
		wgs_vec4i32& pin_color = pin->type()->ed().pin_color;
		
		dlist->PushClipRect(ImVec2(pin_cursor.x, pin_cursor.y), ImVec2(pin_cursor.z, pin_cursor.w));
		if (pin->ed().has_links)
		{
			dlist->AddCircleFilled(ImVec2(pin_cursor.x + 4.f, pin_cursor.y + 4.f), 4.f, IM_COL32(pin_color.x, pin_color.y, pin_color.z, pin_color.w), 8);
		}
		else
		{
			dlist->AddCircle(ImVec2(pin_cursor.x + 4.f, pin_cursor.y + 4.f), 4.f, IM_COL32(pin_color.x, pin_color.y, pin_color.z, pin_color.w), 8, 2.f);
		}
		// will be set on links draw if links are present
		pin->ed().has_links = false;
		
		dlist->PopClipRect();
	}
protected:
	WGSEditor* ed_;
};

class WGSUnitPinDrawComponentExec : public WGSUnitPinDrawComponentBase
{
public:
	WGSUnitPinDrawComponentExec(WGSEditor* editor) : WGSUnitPinDrawComponentBase(editor) {}
	void drawPinConnector(WGSEUnit* unit, WGSEUnitPin* pin, bool input) override
	{
		ImDrawList* dlist = ImGui::GetWindowDrawList();
		wgs_vec4i32& pin_cursor = pin->ed().connector_screen_pos;

		ImVec2 a = ImVec2(pin_cursor.x + 0.f, pin_cursor.y + 0.f);
		ImVec2 b = ImVec2(pin_cursor.x + 8.f, pin_cursor.y + 4.f);
		ImVec2 c = ImVec2(pin_cursor.x + 0.f, pin_cursor.y + 8.f);
		dlist->PushClipRect(ImVec2(pin_cursor.x, pin_cursor.y), ImVec2(pin_cursor.z, pin_cursor.w));
		if (pin->ed().has_links)
		{
			dlist->AddTriangleFilled(a, b, c, IM_COL32(255, 255, 255, 255));
		}
		else
		{
			dlist->AddTriangle(a, b, c, IM_COL32(255, 255, 255, 255), 1.f);
		}
		
		dlist->PopClipRect();

		// will be set on links draw if links are present
		pin->ed().has_links = false;
	}
};

class WGSUnitHoverManager : public WGSEHoverManager
{
public:

	WGSUnitHoverManager(): lastHoverUnit_(nullptr), currentHoverUnit_(nullptr){}

	void frameStart() override
	{
		lastHoverUnit_ = currentHoverUnit_;
		currentHoverUnit_ = nullptr;
	}

	bool wantHover(WGSEDiagramNode* node) override
	{
		currentHoverUnit_ = node;
		return isHovered(node);
	}

	WGSEDiagramNode* getHoveredItem() override
	{
		return lastHoverUnit_;
	}

	bool isHovered(WGSEDiagramNode* node) override
	{
		return node == getHoveredItem();
	}

	bool isSameTypeHovered(WGSEDiagramNode* node) override
	{
		if (!getHoveredItem() || !node)
			return false;
		return getHoveredItem()->getNodeType() == node->getNodeType();
	}

protected:
	WGSEDiagramNode*	lastHoverUnit_;
	WGSEDiagramNode*	currentHoverUnit_;
};

WGSEditor::WGSEditor()
	:dragUnit_(nullptr)
	, createLinkPin_(nullptr)
	, createLinkDropPin_(nullptr)
	, hover_mgr_(new WGSUnitHoverManager())
{
	WGSEUnitType* execType = new WGSEUnitType("exec");
	execType->ed().pin_color = { 255, 255, 255, 255 };
	execType->ed().bold_link = wgs_true;
	execType->ed().renderer = new WGSUnitPinDrawComponentExec(this);

	WGSEUnitType* boolType = new WGSEUnitType("bool");
	boolType->ed().pin_color = { 255, 0, 0, 255 };
	WGSEUnitType* floatType = new WGSEUnitType("float");
	floatType->ed().pin_color = { 0, 255, 0, 255 };
	WGSEUnitType* int32Type = new WGSEUnitType("int32");
	int32Type->ed().pin_color = { 0, 255, 128, 255 };
	WGSEUnitType* vec4i32Type = new WGSEUnitType("vec4i32");
	WGSEUnitType* vec4fType = new WGSEUnitType("vec4f");
	
	header_draw_ = new WGSUnitHeaderDrawComponentCommon(this);
	pin_draw_ = new WGSUnitPinDrawComponentBase(this);

	current_ = new WGSEDiagram();
	WGSEUnit* u0 = new WGSEUnit("CheckFloat");
	u0->inputs_.push_back(new WGSEUnitPin("", execType));
	u0->inputs_.push_back(new WGSEUnitPin("strict", boolType));
	u0->inputs_.push_back(new WGSEUnitPin("float", floatType));
	u0->outputs_.push_back(new WGSEUnitPin("", execType));
	u0->outputs_.push_back(new WGSEUnitPin("isValid", boolType));

	u0->ed().pos.x = 20;
	u0->ed().pos.y = 100;
	
	current_->addUnit(u0);
	WGSEUnit* u1 = new WGSEUnit("node1");
	u1->inputs_.push_back(new WGSEUnitPin("", execType));
	u1->inputs_.push_back(new WGSEUnitPin("intParam", int32Type));
	u1->inputs_.push_back(new WGSEUnitPin("floatParam", floatType));
	u1->inputs_.push_back(new WGSEUnitPin("boolParam", boolType));
	u1->outputs_.push_back(new WGSEUnitPin("", execType));

	u1->ed().pos.x = 400;
	u1->ed().pos.y = 200;
	current_->addUnit(u1);

	WGSEUnit* u2 = new WGSEUnit("PureNode");
	u2->outputs_.push_back(new WGSEUnitPin("pure_variable", int32Type));
	u2->pure(true);

	u2->ed().pos.x = 255;
	u2->ed().pos.y = 200;
	current_->addUnit(u2);

	WGSEUnit* u3 = new WGSEUnit("Branch");
	u3->inputs_.push_back(new WGSEUnitPin("", execType));
	u3->inputs_.push_back(new WGSEUnitPin("condition", boolType));
	u3->outputs_.push_back(new WGSEUnitPin("true", execType));
	u3->outputs_.push_back(new WGSEUnitPin("false", execType));

	u3->ed().pos.x = 200;
	u3->ed().pos.y = 100;
	current_->addUnit(u3);

	WGSEUnit* u4 = new WGSEUnit("DoSomeAction");
	u4->inputs_.push_back(new WGSEUnitPin("", execType));
	u4->outputs_.push_back(new WGSEUnitPin("", execType));
	
	u4->ed().pos.x = 350;
	u4->ed().pos.y = 100;
	current_->addUnit(u4);

	//current_->addLink(new WGSEUnitPinLink(*std::next(u0->outputs_.begin(), 0), *std::next(u3->inputs_.begin(), 0)));
	//current_->addLink(new WGSEUnitPinLink(*std::next(u0->outputs_.begin(), 1), *std::next(u3->inputs_.begin(), 1)));
	//current_->addLink(new WGSEUnitPinLink(*std::next(u0->outputs_.begin(), 1), *std::next(u1->inputs_.begin(), 3)));
	//current_->addLink(new WGSEUnitPinLink(*std::next(u3->outputs_.begin(), 1), *std::next(u1->inputs_.begin(), 0)));
	//current_->addLink(new WGSEUnitPinLink(*std::next(u2->outputs_.begin(), 0), *std::next(u1->inputs_.begin(), 1)));
	//scurrent_->addLink(new WGSEUnitPinLink(*std::next(u3->outputs_.begin(), 0), *std::next(u4->inputs_.begin(), 0)));
}

WGSEditor::~WGSEditor()
{
	delete header_draw_;
}

WGSUnitPinDrawComponent* WGSEditor::getPinDrawCoponent(WGSEUnitPin* pin)
{
	if(!pin->type() || !pin->type()->ed().renderer)
		return pin_draw_;

	return static_cast<WGSUnitPinDrawComponent*>(pin->type()->ed().renderer);
}

void WGSEditor::draw()
{
	static ImGuiDockNodeFlags opt_flags = ImGuiDockNodeFlags_None;
	ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;

	ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(viewport->Pos);
	ImGui::SetNextWindowSize(viewport->Size);
	ImGui::SetNextWindowViewport(viewport->ID);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
	window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
	window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	ImGui::Begin("WGSE Editor", nullptr, window_flags);
	ImGui::PopStyleVar(3);

	ImGuiID dockspace_id = ImGui::GetID("Editor Dockplace");
	ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), opt_flags);

	{
		ImGui::Begin("Objects list");
		ImGui::Text("This is some useful text.");
		ImGui::End();
	}
	{
		ImGui::Begin("Property list");
		ImGui::Text("This is some useful text.");
		ImGui::End();
	}
	{
		ImGui::SetNextWindowSizeConstraints(ImVec2(0, -1), ImVec2(FLT_MAX, -1));
		ImGui::SetNextWindowSize(ImVec2(100.f, 64.f));
		ImGui::Begin("Toolbar");
		//ImGui::Begin("Toolbar", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
		ImGui::Text(".");
		ImGui::End();
	}
	{
		ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 0.f);
		ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, 0.f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.f, 0.f));

		ImGui::Begin("Document");
		
		ImVec2 min = ImGui::GetWindowPos();
		ImVec2 max = ImGui::GetWindowSize();//contentSize = ImGui::GetWindowSize();
		ImDrawList* draw_list = ImGui::GetWindowDrawList();
		draw_list->AddLine(min, ImVec2(min.x + max.x, min.y + max.y), 0xFF0000FF);
		ImVec2 canvas_pos = ImGui::GetCursorScreenPos();            // ImDrawList API uses screen coordinates!
		ImVec2 canvas_size = ImGui::GetContentRegionAvail();        // Resize canvas to what's available
		if (canvas_size.x < 50.0f) canvas_size.x = 50.0f;
		if (canvas_size.y < 50.0f) canvas_size.y = 50.0f;
		draw_list->AddRectFilledMultiColor(canvas_pos, ImVec2(canvas_pos.x + canvas_size.x, canvas_pos.y + canvas_size.y), IM_COL32(50, 50, 50, 255), IM_COL32(50, 50, 60, 255), IM_COL32(60, 60, 70, 255), IM_COL32(50, 50, 60, 255));
		draw_list->AddRect(canvas_pos, ImVec2(canvas_pos.x + canvas_size.x, canvas_pos.y + canvas_size.y), IM_COL32(255, 255, 255, 255));

		//ImGui::line
		//ImGui::Text("This is some useful text.");
		//ImGui::Begin("ddd", nullptr);
		if (dragUnit_)
		{
			WGSEUnitList& units = current_->units_;
			if (*units.begin() != dragUnit_)
			{
				units.remove(dragUnit_);
				units.push_front(dragUnit_);
			}

			ImVec2 current_pos = ImGui::GetMousePos();
			dragUnit_->ed().pos.x += current_pos.x - dragStartPos_.x;
			dragUnit_->ed().pos.y += current_pos.y - dragStartPos_.y;

			dragStartPos_ = current_pos;
			if (ImGui::IsMouseReleased(0))
			{
				dragUnit_ = nullptr;
			}
		}
		if (createLinkPin_)
		{
			wgs_vec2i32 startPos = createLinkPin_->ed().link_pos;
			wgs_vec2i32 endPos = { ImGui::GetMousePos().x, ImGui::GetMousePos().y };
			wgs_bool valid_link = true;

			if (createLinkDropPin_)
			{
				endPos = createLinkDropPin_->ed().link_pos;
				valid_link = checkLinking();
			}

			if (true)
			{
				wgs_vec4i32 link_color = createLinkPin_->type()->ed().pin_color;
				if (!valid_link)
					link_color.w = 64;

				float link_thickness = 3.f;

				if (createLinkPin_->type()->ed().bold_link)
					link_thickness = 3.f;

				wgs_float bazierStep = 50.f;

				createLinkPin_->ed().has_links = true;
				if (createLinkDropPin_)
					createLinkDropPin_->ed().has_links = true;

				wgs_float dist = fabs(startPos.y - endPos.y);
				bazierStep = wgs_min(bazierStep, dist);

				bool is_input;
				if (getPinUnit(createLinkPin_, nullptr, &is_input) && is_input)
					std::swap(startPos, endPos);

				draw_list->AddBezierCurve(
					ImVec2(startPos.x, startPos.y), ImVec2(startPos.x + bazierStep, startPos.y),
					ImVec2(endPos.x - bazierStep, endPos.y), ImVec2(endPos.x, endPos.y),
					IM_COL32(link_color.x, link_color.y, link_color.z, link_color.w),
					link_thickness);
			}

			if (ImGui::IsMouseReleased(0))
			{
				if (valid_link)
				{
					finishLinking();
				}
				createLinkPin_ = nullptr;
			}
			// will be filled on pin draw
			createLinkDropPin_ = nullptr;
		}
		ImGui::PopStyleVar(3);
		drawCurrent();
	
		ImGui::End();

		
	}
	ImGui::End();
}

void WGSEditor::drawCurrent()
{
	WGSEUnitList& units = current_->units_;

	hover()->frameStart();

	drawLinks();

	auto it = units.rbegin();
	while (it != units.rend())
	{
		drawUnit(*it);
		it++;
	}
}

void WGSEditor::drawUnit(WGSEUnit* unit)
{
	const wgs_int32 PIN_HEIGHT = 18;
	
	WGSEUnitEditorData& ed = unit->ed();
	wgs_uint32 id = ed.id;
	if (!id)
	{
		id = current_->ed_.last_id++;
		ed.id = id;
	}

	wgs_vec2i32 header_size = header_draw_->computeSize(unit);
	if (unit->pure())
		header_size.y = 0;

	wgs_int32 connector_size = 8;
	wgs_vec2i32 computed_size = header_size;
	wgs_vec2i32 input_pins_block = { 0, 0 };
	wgs_vec2i32 output_pins_block = { 0, 0 };
	{
		WGSEPinList& pins = unit->inputs_;
		auto it = pins.begin();
		while (it != pins.end())
		{
			auto pin_draw = getPinDrawCoponent(*it);
			wgs_vec2i32 pin_block = pin_draw->computePinLayout(unit, *it, true);
			input_pins_block.y += pin_block.y;
			input_pins_block.x = wgs_max(input_pins_block.x, pin_block.x);
			
			++it;
		}
	}
	{
		WGSEPinList& pins = unit->outputs_;
		auto it = pins.begin();
		while (it != pins.end())
		{
			auto pin_draw = getPinDrawCoponent(*it);

			wgs_vec2i32 pin_block = pin_draw->computePinLayout(unit, *it, false);
			output_pins_block.y += pin_block.y;
			output_pins_block.x = wgs_max(output_pins_block.x, pin_block.x);
			++it;
		}
	}
	computed_size.x = wgs_max(computed_size.x, input_pins_block.x + output_pins_block.x);
	computed_size.y += wgs_max(input_pins_block.y, output_pins_block.y);

	if (input_pins_block.y > 0 && output_pins_block.y > 0)
		computed_size.x += 20;

	ed.size.x = wgs_max(ed.size.x, computed_size.x);
	ed.size.y = wgs_max(ed.size.y, computed_size.y);
	
	ImGui::PushID(id);
	ImGui::SetCursorPos(ImVec2(ed.pos.x, ed.pos.y));
	ImGui::BeginChild("Rect", ImVec2(ed.size.x, ed.size.y));
	//

	ImDrawList* unit_draw_list = ImGui::GetWindowDrawList();

	ImVec2 unit_pos = ImGui::GetCursorScreenPos();            // ImDrawList API uses screen coordinates!
	ImVec2 unit_size = ImGui::GetContentRegionAvail();        // Resize canvas to what's available

	ed.screen_pos.x = unit_pos.x;
	ed.screen_pos.y = unit_size.y;
	ed.screen_pos.z = ed.screen_pos.x + unit_size.x;
	ed.screen_pos.w = ed.screen_pos.y + unit_size.y;

	bool canDrag = false;
	ImGui::PushClipRect(ImVec2(unit_pos.x, unit_pos.y), ImVec2(unit_pos.x + unit_size.x + 8, unit_pos.y + unit_size.y + 8), false);
	unit_draw_list->AddRect(ImVec2(unit_pos.x + 1, unit_pos.y + 1), ImVec2(unit_pos.x + unit_size.x + 3, unit_pos.y + unit_size.y + 3), IM_COL32(0, 0, 0, 32), 8, 15, 5.f);
	ImGui::PopClipRect();

	unit_draw_list->AddRectFilled(unit_pos, ImVec2(unit_pos.x + unit_size.x, unit_pos.y + unit_size.y), IM_COL32(20, 20, 20, 128), 4);
	if (!unit->pure())
	{
		header_size.x = unit_size.x;
		//draw Title
		ImGui::BeginChild("header", ImVec2(header_size.x, header_size.y));
		header_draw_->draw(unit);

		bool canDrag = ImGui::IsWindowHovered(ImGuiHoveredFlags_None);

		if (canDrag && !isDragging() && !isCreatingLink())
		{
			ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeAll);
			if(ImGui::IsMouseDown(0))
				startDrag(unit);
		}

		ImGui::EndChild();
	}
	ImVec2 canvas_pos = ImGui::GetCursorScreenPos();
	// 
	{
		ImGui::SetCursorPos(ImVec2(0, header_size.y));
		wgs_vec2i32 cursor = { 0, header_size.y };

		WGSEPinList& pins = unit->inputs_;
		auto it = pins.begin();
		while (it != pins.end())
		{
			auto pin_draw = getPinDrawCoponent(*it);
			wgs_vec2i32 pin_block = pin_draw->computePinLayout(unit, *it, true);
			wgs_vec4i32& connector = (*it)->ed().connector_screen_pos;
			wgs_vec2i32& pin_link_center = (*it)->ed().link_pos;

			connector.x = unit_pos.x + cursor.x - (connector_size >> 1);
			connector.y = unit_pos.y + cursor.y + (pin_block.y >> 1) - (connector_size >> 1);
			connector.z = connector.x + connector_size;
			connector.w = connector.y + connector_size;

			pin_link_center.x = connector.x + ((connector.z - connector.x) >> 1);
			pin_link_center.y = connector.y + ((connector.w - connector.y) >> 1);

			ImGui::SetCursorPos(ImVec2(cursor.x, cursor.y));
			cursor.y += pin_block.y;

			ImGui::PushID(*it);
			ImGui::BeginChild("pin", ImVec2(pin_block.x, pin_block.y));
			pin_draw->drawPin(unit, *it, true);
			pin_draw->drawPinConnector(unit, *it, true);

			bool canDrag = ImGui::IsWindowHovered(ImGuiHoveredFlags_None);

			if (canDrag && !isDragging() && !isCreatingLink())
			{
				//ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeAll);
				if (ImGui::IsMouseDown(0))
					startDrag(unit);
			}
			
			ImGui::EndChild();
			ImGui::PopID();

			it++;
		}
	}
	{
		ImGui::SetCursorPos(ImVec2(0, header_size.y));
		wgs_vec2i32 cursor = { 0, header_size.y };

		WGSEPinList& pins = unit->outputs_;
		auto it = pins.begin();
		while (it != pins.end())
		{
			auto pin_draw = getPinDrawCoponent(*it);
			wgs_vec2i32 pin_block = pin_draw->computePinLayout(unit, *it, false);
			wgs_vec4i32& connector = (*it)->ed().connector_screen_pos;
			wgs_vec2i32& pin_link_center = (*it)->ed().link_pos;

			connector.x = unit_pos.x + unit_size.x - (connector_size >> 1);
			connector.y = unit_pos.y + cursor.y + (pin_block.y >> 1) - (connector_size >> 1);
			connector.z = connector.x + connector_size;
			connector.w = connector.y + connector_size;

			pin_link_center.x = connector.x + ((connector.z - connector.x) >> 1);
			pin_link_center.y = connector.y + ((connector.w - connector.y) >> 1);

			ImGui::SetCursorPos(ImVec2(unit_size.x - pin_block.x, cursor.y));
			cursor.y += pin_block.y;

			ImGui::PushID(*it);
			ImGui::BeginChild("pin", ImVec2(pin_block.x, pin_block.y));
			pin_draw->drawPin(unit, *it, false);
			pin_draw->drawPinConnector(unit, *it, false);

			bool canDrag = ImGui::IsWindowHovered(ImGuiHoveredFlags_None);

			if (canDrag && !isDragging() && !isCreatingLink())
			{
				//ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeAll);
				if (ImGui::IsMouseDown(0))
					startDrag(unit);
			}

			ImGui::EndChild();
			ImGui::PopID();

			it++;
		}
	}

	canDrag |= ImGui::IsWindowHovered(ImGuiHoveredFlags_None);

	ImGui::EndChild();
	ImGui::PopID();

	if (ImGui::IsMouseHoveringRect(unit_pos, ImVec2(unit_pos.x + unit_size.x, unit_pos.y + unit_size.y)))
	{
		if (hover()->wantHover(unit))
		{
			if (canDrag && !isDragging() && !isCreatingLink())
			{
				ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeAll);
				if (ImGui::IsMouseDown(0))
					startDrag(unit);
			}

			unit_draw_list->AddRect(ImVec2(unit_pos.x - 1, unit_pos.y - 1), ImVec2(unit_pos.x + unit_size.x + 1, unit_pos.y + unit_size.y + 1), IM_COL32(255, 255, 0, 128), 8, 15, 1.f);
		}
	}
}

void WGSEditor::drawLinks()
{
	ImDrawList* dlist = ImGui::GetWindowDrawList();
	
	WGSEUnitPinLinkList& links = current_->links_;
	WGSEUnitPinLinkList::iterator to_remove = current_->links_.end();
	auto it = links.begin();
	while (it != links.end())
	{
		WGSEUnitPinLink* link = *it;
		ImGui::PushID(link);
		WGSEUnitPin* source = link->source();
		WGSEUnitPin* target = link->target();

		wgs_vec4i32 link_color = source->type()->ed().pin_color;

		if (isCreatingLink() || (hover()->isSameTypeHovered(link) && !hover()->isHovered(link)))
			link_color.w = 64;

		float link_thickness = 1.f;

		if (source->type()->ed().bold_link)
			link_thickness = 2.f;

		wgs_float bazierStep = 50.f;

		source->ed().has_links = true;
		target->ed().has_links = true;

		wgs_vec2i32& source_pos = source->ed().link_pos;
		wgs_vec2i32& target_pos = target->ed().link_pos;

		wgs_float dist = fabs(source_pos.y - target_pos.y);
		bazierStep = wgs_min(bazierStep, dist);

		wgs_vec2f p0 = { (wgs_float)source_pos.x, (wgs_float)source_pos.y };
		wgs_vec2f p3 = { (wgs_float)target_pos.x, (wgs_float)target_pos.y };
		wgs_vec2f p1 = { p0.x + bazierStep, p0.y };
		wgs_vec2f p2 = { p3.x - bazierStep, p3.y };

		wgs_vec2f l0 = { ImGui::GetMousePos().x - 5.f, ImGui::GetMousePos().y - 5.f };
		wgs_vec2f l1 = { ImGui::GetMousePos().x + 5.f, ImGui::GetMousePos().y + 5.f };

		wgs_vec2f l2 = { ImGui::GetMousePos().x + 5.f, ImGui::GetMousePos().y - 5.f };
		wgs_vec2f l3 = { ImGui::GetMousePos().x - 5.f, ImGui::GetMousePos().y + 5.f };

		
		bool want_hover = intersect_cubic_spline_line(p0, p1, p2, p3, l0, l1) || intersect_cubic_spline_line(p0, p1, p2, p3, l2, l3);
		
		if (want_hover && hover()->wantHover(link))
		{
			link_thickness = 4.f;
			if(ImGui::IsMouseClicked(1))
				ImGui::OpenPopup("LinkPopup");
		}
		static float value =0.f;
		if (ImGui::BeginPopup("LinkPopup", 0))
		{
			if (ImGui::Selectable("Break link")) to_remove = it;
			ImGui::EndPopup();
		}

		dlist->AddBezierCurve(
			ImVec2(p0.x + 2, p0.y + 2), ImVec2(p1.x + 2, p1.y + 2),
			ImVec2(p2.x + 2, p2.y + 2), ImVec2(p3.x + 2, p3.y + 2),
			IM_COL32(0, 0, 0, 32),
			link_thickness + 2.f);

		dlist->AddBezierCurve(
			ImVec2(p0.x, p0.y), ImVec2(p1.x, p1.y),
			ImVec2(p2.x, p2.y), ImVec2(p3.x, p3.y),
			IM_COL32(link_color.x, link_color.y, link_color.z, link_color.w),
			link_thickness);

		ImGui::PopID();
		it++;
	}
	if (to_remove != current_->links_.end())
	{
		current_->links_.erase(to_remove);
	}
}

void WGSEditor::startDrag(WGSEUnit* node)
{
	WGSEUnitList& units = current_->units_;
	dragUnit_ = node;
	dragStartPos_ = ImGui::GetMousePos();
}

void WGSEditor::startCreatingLink(WGSEUnitPin* pin)
{
	wgs_assert(createLinkPin_ == nullptr);
	createLinkPin_ = pin;
}

void WGSEditor::checkDropLink(WGSEUnitPin* pin)
{
	if (pin == createLinkPin_)
		return;
	createLinkDropPin_ = pin;
}

bool WGSEditor::getPinUnit(WGSEUnitPin* pin, WGSEUnit** unit, wgs_bool* is_input)
{
	auto it = current_->units_.begin();
	WGSEUnit* o_unit = nullptr;
	bool o_input;
	while (it != current_->units_.end())
	{
		{
			auto& pin_list = (*it)->inputs_;
			auto pin_it = std::find(pin_list.begin(), pin_list.end(), pin);
			if (pin_it != pin_list.end())
			{
				o_unit = *it;
				o_input = true;
				break;
			}
		}
		{
			auto& pin_list = (*it)->outputs_;
			auto pin_it = std::find(pin_list.begin(), pin_list.end(), pin);
			if (pin_it != pin_list.end())
			{
				o_unit = *it;
				o_input = false;
				break;
			}
		}
		
		it++;
	}
	if (o_unit)
	{
		if (unit)
			*unit = o_unit;
		if (is_input)
			*is_input = o_input;

		return true;
	}
	return false;
}

bool WGSEditor::checkLinking()
{
	if (!createLinkPin_ || !createLinkDropPin_)
		return false;
	if (createLinkPin_->type() != createLinkDropPin_->type())
		return false;

	bool a_type, b_type;
	WGSEUnit *a_unit, *b_unit;

	if (!getPinUnit(createLinkPin_, &a_unit, &a_type))
		return false;
	if (!getPinUnit(createLinkDropPin_, &b_unit, &b_type))
		return false;
	if (a_type == b_type) // input != output check
		return false;
	if (a_unit == b_unit)
		return false;
	return true;
}

void WGSEditor::finishLinking()
{
	bool a_type, b_type;

	if (getPinUnit(createLinkPin_, nullptr, &a_type) && getPinUnit(createLinkDropPin_, nullptr, &b_type))
	{
		if (a_type)// createLinkPin_ make sure that in createLinkPin_ is output
			std::swap(createLinkPin_, createLinkDropPin_);
		current_->addLink(new WGSEUnitPinLink(createLinkPin_, createLinkDropPin_));
	}
}
