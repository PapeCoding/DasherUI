#pragma once

#include <DasherScreen.h>

#include "DasherInput.h"
#include "imgui.h"

class DasherUIScreen : public Dasher::CDasherScreen, public Dasher::CScreenCoordInput
{
public:
	DasherUIScreen();

	std::pair<Dasher::screenint, Dasher::screenint> TextSize(Label* label, unsigned iFontSize) override;
	void Display() override;
	bool IsPointVisible(Dasher::screenint x, Dasher::screenint y) override;
		
	bool SetCanvasSize(const ImVec2 position, const ImVec2 size);

	bool GetScreenCoords(Dasher::screenint& iX, Dasher::screenint& iY, Dasher::CDasherView* pView) override;

    void DrawString(Label* label, Dasher::screenint x, Dasher::screenint y, unsigned iFontSize, const Dasher::ColorPalette::Color& color) override;
    void DrawRectangle(Dasher::screenint x1, Dasher::screenint y1, Dasher::screenint x2, Dasher::screenint y2, const Dasher::ColorPalette::Color& color, const Dasher::ColorPalette::Color& outlineColor, int iThickness) override;
    void DrawCircle(Dasher::screenint iCX, Dasher::screenint iCY, Dasher::screenint iR, const Dasher::ColorPalette::Color& fillColor, const Dasher::ColorPalette::Color& lineColor, int iLineWidth) override;
    void Polyline(point* Points, int Number, int iWidth, const Dasher::ColorPalette::Color& color) override;
    void Polygon(point* Points, int Number, const Dasher::ColorPalette::Color& fillColor, const Dasher::ColorPalette::Color& outlineColor, int lineWidth) override;

private:
	ImVec2 CanvasPos;
	ImVec2 CanvasSize;

	ImFont* Font;
};
