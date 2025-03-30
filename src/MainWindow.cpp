#include "MainWindow.h"

#include <filesystem>

#include "FileUtils.h"
#include "nfd.h"
#include "XmlSettingsStore.h"

MainWindow::MainWindow()
{
	ImGui::SetCurrentFont(LoadFonts(22.0f));

	Settings = std::make_unique<Dasher::XmlSettingsStore>("Settings.xml", this);
	Settings->Load();
	Settings->Save();

	Controller = std::make_unique<DasherController>(Settings.get());
	Controller->Initialize();

	Controller->GetPermittedValues(Dasher::SP_ALPHABET_ID, Alphabets);
}

#define FORWARD_KEY(ImGui_Key, DasherKey) { \
	if(ImGui::IsKeyPressed(ImGui_Key, false)) Controller->KeyDown(time, DasherKey); \
	if(ImGui::IsKeyReleased(ImGui_Key, false)) Controller->KeyUp(time, DasherKey); \
}
void MainWindow::HandleInput(const std::unique_ptr<DasherController>& controller, long time)
{
	// Space
	FORWARD_KEY(ImGuiKey_Space, Dasher::Keys::Big_Start_Stop_Key)

	// Button 1
	FORWARD_KEY(ImGuiKey_LeftArrow, Dasher::Keys::Button_1)
	FORWARD_KEY(ImGuiKey_Keypad4, Dasher::Keys::Button_1)
	FORWARD_KEY(ImGuiKey_A, Dasher::Keys::Button_1)
	FORWARD_KEY(ImGuiKey_J, Dasher::Keys::Button_1)
	FORWARD_KEY(ImGuiKey_1, Dasher::Keys::Button_1)

	// Button 2
	FORWARD_KEY(ImGuiKey_UpArrow, Dasher::Keys::Button_2)
	FORWARD_KEY(ImGuiKey_Keypad8, Dasher::Keys::Button_2)
	FORWARD_KEY(ImGuiKey_W, Dasher::Keys::Button_2)
	FORWARD_KEY(ImGuiKey_I, Dasher::Keys::Button_2)
	FORWARD_KEY(ImGuiKey_2, Dasher::Keys::Button_2)

	// Button 3
	FORWARD_KEY(ImGuiKey_RightArrow, Dasher::Keys::Button_3)
	FORWARD_KEY(ImGuiKey_Keypad6, Dasher::Keys::Button_3)
	FORWARD_KEY(ImGuiKey_S, Dasher::Keys::Button_3)
	FORWARD_KEY(ImGuiKey_K, Dasher::Keys::Button_3)
	FORWARD_KEY(ImGuiKey_3, Dasher::Keys::Button_3)

	// Button 4
	FORWARD_KEY(ImGuiKey_DownArrow, Dasher::Keys::Button_4)
	FORWARD_KEY(ImGuiKey_Keypad2, Dasher::Keys::Button_4)
	FORWARD_KEY(ImGuiKey_Z, Dasher::Keys::Button_4)
	FORWARD_KEY(ImGuiKey_M, Dasher::Keys::Button_4)
	FORWARD_KEY(ImGuiKey_4, Dasher::Keys::Button_4)
}
#undef FORWARD_KEY

bool MainWindow::render(float DeltaTime)
{
    static ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoBackground ;

    const ImGuiViewport* viewport = ImGui::GetMainViewport();
	const ImVec2 spacing = ImGui::GetStyle().ItemSpacing;
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);
	
    if (ImGui::Begin("MainWindow", nullptr, flags))
    {
	    if(ImGui::BeginMainMenuBar())
	    {
            if (ImGui::BeginMenu("File"))
	        {
	            if (ImGui::MenuItem("Load TrainingData", "CTRL+T"))
	            {
					nfdu8char_t *outPath;
					if(NFD_OpenDialogU8(&outPath, nullptr, 0, nullptr) == NFD_OKAY)
					{
						Controller->ImportTrainingText(outPath);
						NFD_FreePathU8(outPath);
					}
	            }
				if (ImGui::MenuItem("Quit", "CTRL+Q"))
	            {
		            return false;
	            }
	            ImGui::EndMenu();
	        }
			if (ImGui::BeginMenu("Edit"))
	        {
	            if (ImGui::MenuItem("Copy", "CTRL+C"))
	            {
					CopyTextToClipboard(*Controller->GetBufferRef());
	            }
				if (ImGui::MenuItem("Clear", "CTRL+R"))
	            {
					ClearBuffer();
	            }
				int speed = Controller->GetLongParameter(Dasher::LP_MAX_BITRATE);
				if(ImGui::SliderInt ("Speed", &speed, 1, 2000))
				{
					Controller->SetLongParameter(Dasher::LP_MAX_BITRATE, speed);
				}

				static int item_current = 0;
				if(ImGui::Combo("Alphabet", &item_current, [](void* data, int idx, const char** out_text) { *out_text = static_cast<const std::vector<std::string>*>(data)->at(idx).c_str(); return true; }, (void*)&Alphabets, static_cast<int>(Alphabets.size()), 10))
				{
					Controller->SetStringParameter(Dasher::SP_ALPHABET_ID, Alphabets[item_current]);
				}

	            ImGui::EndMenu();
	        }
	    	if (ImGui::BeginMenu("Help"))
	        {
	            if (ImGui::MenuItem("Demo Window"))
	            {
					DisplayDemoWindow = !DisplayDemoWindow;
	            }
	            ImGui::EndMenu();
	        }

			ImGui::EndMainMenuBar();
	    }


    	ImVec2 MainWindowSize = ImGui::GetContentRegionMax();
        ImGui::InputTextMultiline("##DasherOutput", Controller->GetBufferRef(), ImVec2(-(MainWindowSize.x*0.1f + spacing.x), MainWindowSize.y * 0.1f));
		ImGui::SameLine();
		ImGui::BeginGroup();
			float CursorPosXGroup = ImGui::GetCursorScreenPos().x;
			ImVec2 ButtonSize = ImVec2(-FLT_MIN, MainWindowSize.y*0.05f - spacing.y * 0.5f);
			if(ImGui::Button("Copy", ButtonSize))
			{
				CopyTextToClipboard(*Controller->GetBufferRef());
			}
			ImGui::SetCursorPosX(CursorPosXGroup);
			if(ImGui::Button("Clear", ButtonSize))
			{
				ClearBuffer();
			}
		ImGui::EndGroup();

		const ImVec2 canvasPos = ImGui::GetCursorScreenPos();
	    const ImVec2 canvasSize = ImGui::GetContentRegionAvail();

		ImGui::PushClipRect(canvasPos, canvasPos + canvasSize, false);

	    ImDrawList* WindowDrawList = ImGui::GetWindowDrawList();
		WindowDrawList->AddRectFilled(
			canvasPos,
			ImVec2(canvasPos.x + canvasSize.x, canvasPos.y + canvasSize.y),
			ImGui::ColorConvertFloat4ToU32({0,1,0,1})
		);

		Controller->Render(static_cast<long>(DeltaTime * 1000.0f), canvasPos, canvasSize); //convert to millis

		ImGui::PopClipRect();

		HandleInput(Controller, static_cast<long>(DeltaTime * 1000.0f));
    }
    ImGui::End();

	if(DisplayDemoWindow) ImGui::ShowDemoWindow(&DisplayDemoWindow);
	return true;
}

void MainWindow::CopyTextToClipboard(const std::string& text) const
{
	Controller->CopyToClipboard(text);
}

void MainWindow::ClearBuffer() const
{
	Controller->GetBufferRef()->clear();
	Controller->SetBuffer(0);
}

ImFont* MainWindow::LoadFonts(float pixel_size)
{
	ImVector<ImWchar> ranges;
	ImFontGlyphRangesBuilder builder;
	builder.AddRanges(ImGui::GetIO().Fonts->GetGlyphRangesJapanese());
	builder.AddText("\xE2\x96\xA1"); // box
	builder.BuildRanges(&ranges);

	ImFontConfig config;
	config.MergeMode = true;
	ImGuiIO& io = ImGui::GetIO();
	ImFont* Font = io.Fonts->AddFontFromFileTTF("./Resources/NotoSans-Medium.ttf", pixel_size, nullptr, io.Fonts->GetGlyphRangesDefault());
	io.Fonts->AddFontFromFileTTF("./Resources/NotoSansJP-Medium.otf", pixel_size, &config, ranges.Data); //merge with first one
	io.Fonts->Build();

	return Font;
}
