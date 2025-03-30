#include "DasherUIScreen.h"
#include "imgui.h"
#include "MainWindow.h"
#include "imgui/imgui_internal.h"

DasherUIScreen::DasherUIScreen(): Dasher::CDasherScreen(100,100), CScreenCoordInput(_("Mouse Input"))
{
	Font = MainWindow::LoadFonts(50.0f);
}

std::pair<Dasher::screenint, Dasher::screenint> DasherUIScreen::TextSize(Label* Label, unsigned iFontSize)
{
	const ImVec2 Size = Font->CalcTextSizeA(static_cast<float>(iFontSize), FLT_MAX, -1, Label->m_strText.c_str());
	return {static_cast<Dasher::screenint>(Size.x), static_cast<Dasher::screenint>(Size.y)};
}

void DasherUIScreen::DrawString(Label* label, Dasher::screenint x, Dasher::screenint y, unsigned iFontSize, const Dasher::ColorPalette::Color& Color)
{
	if(Color.isFullyTransparent()) return;
	const ImVec4 ImColor = { static_cast<float>(Color.Red) / 255.0f, static_cast<float>(Color.Green) / 255.0f, static_cast<float>(Color.Blue) / 255.0f, static_cast<float>(Color.Alpha) / 255.0f };
	const ImVec2 Pos = { static_cast<float>(x),static_cast<float>(y)};
	ImGui::GetWindowDrawList()->AddText(Font, static_cast<float>(iFontSize), Pos + CanvasPos, ImGui::ColorConvertFloat4ToU32(ImColor), label->m_strText.c_str());
}

void DasherUIScreen::DrawRectangle(Dasher::screenint x1, Dasher::screenint y1, Dasher::screenint x2, Dasher::screenint y2, const Dasher::ColorPalette::Color& FillColor, const Dasher::ColorPalette::Color& OutlineColor, int iThickness)
{
	const ImVec4 ImFillColor = { static_cast<float>(FillColor.Red) / 255.0f, static_cast<float>(FillColor.Green) / 255.0f, static_cast<float>(FillColor.Blue) / 255.0f, static_cast<float>(FillColor.Alpha) / 255.0f };
	const ImVec2 p1 = { static_cast<float>(x1), static_cast<float>(y1)};
	const ImVec2 p2 = { static_cast<float>(x2), static_cast<float>(y2)};

	if(!FillColor.isFullyTransparent()) ImGui::GetWindowDrawList()->AddRectFilled(CanvasPos + p1, CanvasPos + p2, ImGui::ColorConvertFloat4ToU32(ImFillColor));

	if(iThickness > 0 && !OutlineColor.isFullyTransparent())
	{
		const ImVec4 ImOutlineColor = { static_cast<float>(OutlineColor.Red) / 255.0f, static_cast<float>(OutlineColor.Green) / 255.0f, static_cast<float>(OutlineColor.Blue) / 255.0f, static_cast<float>(FillColor.Alpha) / 255.0f };

		ImGui::GetWindowDrawList()->AddRect(CanvasPos + p1, CanvasPos + p2, ImGui::ColorConvertFloat4ToU32(ImOutlineColor),0,0, static_cast<float>(iThickness));
	}
}

void DasherUIScreen::DrawCircle(Dasher::screenint iCX, Dasher::screenint iCY, Dasher::screenint iR, const Dasher::ColorPalette::Color& FillColor, const Dasher::ColorPalette::Color& LineColor, int iLineWidth)
{
	const ImVec4 ImFillColor = { static_cast<float>(FillColor.Red) / 255.0f, static_cast<float>(FillColor.Green) / 255.0f, static_cast<float>(FillColor.Blue) / 255.0f, static_cast<float>(FillColor.Alpha) / 255.0f };

	if(!FillColor.isFullyTransparent()) ImGui::GetWindowDrawList()->AddCircleFilled(CanvasPos + ImVec2(static_cast<float>(iCX), static_cast<float>(iCY)), static_cast<float>(iR), ImGui::ColorConvertFloat4ToU32(ImFillColor));

	if (iLineWidth > 0 && !LineColor.isFullyTransparent())
	{
		const ImVec4 ImOutlineColor = { static_cast<float>(LineColor.Red) / 255.0f, static_cast<float>(LineColor.Green) / 255.0f, static_cast<float>(LineColor.Blue) / 255.0f, static_cast<float>(LineColor.Alpha) / 255.0f };

		ImGui::GetWindowDrawList()->AddCircle(CanvasPos + ImVec2(static_cast<float>(iCX), static_cast<float>(iCY)), static_cast<float>(iR), ImGui::ColorConvertFloat4ToU32(ImOutlineColor),0, static_cast<float>(iLineWidth));
	}
}

void DasherUIScreen::Polyline(point* Points, int Number, int iWidth, const Dasher::ColorPalette::Color& LineColor)
{
	if(LineColor.isFullyTransparent()) return;

	const ImVec4 ImLineColor = { static_cast<float>(LineColor.Red) / 255.0f, static_cast<float>(LineColor.Green) / 255.0f, static_cast<float>(LineColor.Blue) / 255.0f, static_cast<float>(LineColor.Alpha) / 255.0f};

	std::vector<ImVec2> points;

	for(unsigned int i = 0; i < Number; i++)
	{
		points.push_back(CanvasPos + ImVec2(static_cast<float>(Points[i].x), static_cast<float>(Points[i].y)));
	}

	ImGui::GetWindowDrawList()->AddPolyline(points.data(), Number, ImGui::ColorConvertFloat4ToU32(ImLineColor), 0, static_cast<float>(iWidth));
}

void DasherUIScreen::Polygon(point* Points, int Number, const Dasher::ColorPalette::Color& FillColor, const Dasher::ColorPalette::Color& outlineColor, int lineWidth)
{
	const ImVec4 ImFillColor = { static_cast<float>(FillColor.Red) / 255.0f, static_cast<float>(FillColor.Green) / 255.0f, static_cast<float>(FillColor.Blue) / 255.0f, static_cast<float>(FillColor.Alpha) / 255.0f };

	std::vector<ImVec2> points;

	for (unsigned int i = 0; i < Number; i++)
	{
		points.push_back(CanvasPos + ImVec2(static_cast<float>(Points[i].x), static_cast<float>(Points[i].y)));
	}

	if(!FillColor.isFullyTransparent()) ImGui::GetWindowDrawList()->AddConvexPolyFilled(points.data(), Number, ImGui::ColorConvertFloat4ToU32(ImFillColor));

	if(lineWidth > 0)
	{
		Polyline(Points, Number, lineWidth, outlineColor);
	}
}

void DasherUIScreen::Display(){}

bool DasherUIScreen::IsPointVisible(Dasher::screenint, Dasher::screenint){
	return true;
}

bool DasherUIScreen::SetCanvasSize(const ImVec2 position, const ImVec2 size)
{
	if(fabs(this->CanvasSize.x - size.x) > std::numeric_limits<float>::epsilon() || fabs(this->CanvasSize.y - size.y) > std::numeric_limits<float>::epsilon())
	{
		resize(static_cast<Dasher::screenint>(size.x), static_cast<Dasher::screenint>(size.y));
		this->CanvasSize = size;
		this->CanvasPos = position;
		return true;
	}

	this->CanvasPos = position;
	return false;
}

bool DasherUIScreen::GetScreenCoords(Dasher::screenint& iX, Dasher::screenint& iY, Dasher::CDasherView* pView)
{
	const ImVec2 MousePos = ImGui::GetMousePos();
	if(!ImGui::IsMousePosValid(&MousePos)) return false;
	iX = static_cast<Dasher::screenint>(MousePos.x - CanvasPos.x);
	iY = static_cast<Dasher::screenint>(MousePos.y - CanvasPos.y);
	return true;
}