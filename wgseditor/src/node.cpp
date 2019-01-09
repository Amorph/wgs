#include "imgui.h"

#include "node.h"
#include "editor.h"

void draw_node(WGSEditor* editor, struct wgse_node* node)
{
//	ImDrawList* draw_list = ImGui::GetWindowDrawList();
//	ImGui::PushID(node->ID);
//	ImGui::SetCursorPos(node->position);
//	ImGui::BeginChild("Rect", ImVec2(120, 60));
//
//	ImVec2 canvas_pos = ImGui::GetCursorScreenPos();            // ImDrawList API uses screen coordinates!
//	ImVec2 canvas_size = ImGui::GetContentRegionAvail();        // Resize canvas to what's available
//
//	draw_list->AddRectFilledMultiColor(canvas_pos, ImVec2(canvas_pos.x + canvas_size.x, canvas_pos.y + canvas_size.y), IM_COL32(50, 50, 50, 255), IM_COL32(50, 50, 60, 255), IM_COL32(60, 60, 70, 255), IM_COL32(50, 50, 60, 255));
//	//sdraw_list->AddText(canvas_pos)
//
//	if (ImGui::IsMouseHoveringRect(canvas_pos, ImVec2(canvas_pos.x + canvas_size.x, canvas_pos.y + canvas_size.y)))
//	{
//		if (ImGui::IsMouseDown(0) && !editor->isDragging())
//			editor->startDrag(node);
//
//		draw_list->AddRect(canvas_pos, ImVec2(canvas_pos.x + canvas_size.x, canvas_pos.y + canvas_size.y), IM_COL32(255, 255, 255, 255));
//	}
//
//	ImGui::EndChild();
//	ImGui::PopID();
}
