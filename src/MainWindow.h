#pragma once
#include <string>

#include "DasherController.h"
#include "FileUtils.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "XmlSettingsStore.h"
#include "imgui/misc/cpp/imgui_stdlib.h"

class MainWindow : public Dasher::CommandlineErrorDisplay
{
public:
	MainWindow();
	~MainWindow() = default;

	void HandleInput(const std::unique_ptr<DasherController>& controller, long time);
	bool render(float DeltaTime);

	void CopyTextToClipboard(const std::string& text) const;
	void ClearBuffer() const;

	ImVec4& GetClearColor(){return clear_color;}

	static ImFont* LoadFonts(float pixel_size);
	
private:
    ImVec4 clear_color = ImVec4(0, 0, 0, 1);
	bool DisplayDemoWindow = false;

	//Dasher Related Objects
	std::unique_ptr <Dasher::XmlSettingsStore> Settings;
	std::unique_ptr<DasherController> Controller;
	std::vector<std::string> Alphabets;
	std::vector<std::string> ColorPalettes;
};

