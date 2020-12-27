#include "ImGui_Ext.hpp"

#include "imgui-master/imgui.h"

void ImGui::QuickLabel(const char* szLabelString)
{
	ImGui::TextColored(QUICKLABEL_CLR, szLabelString);
}