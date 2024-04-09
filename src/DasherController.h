#pragma once
#include <DashIntfScreenMsgs.h>

#include "DasherUIScreen.h"
#include "imgui.h"
#include "imgui_internal.h"
#include <memory>

#include "OSOutput.h"
#include "SocketInput.h"


class DasherController : public Dasher::CDashIntfScreenMsgs
{
public:
	DasherController(Dasher::CSettingsStore* pSettingsStore);

	void editOutput(const std::string& strText, Dasher::CDasherNode* pNode) override;
	void editDelete(const std::string& strText, Dasher::CDasherNode* pNode) override;
	unsigned ctrlMove(bool bForwards, Dasher::CControlManager::EditDistance dist) override;
	unsigned ctrlDelete(bool bForwards, Dasher::CControlManager::EditDistance dist) override;
	std::string GetContext(unsigned iStart, unsigned iLength) override;
	std::string GetAllContext() override;
	std::string GetTextAroundCursor(Dasher::CControlManager::EditDistance iDist) override;
	int GetAllContextLenght() override;

	void Initialize();
	void Render(long deltaTime, ImVec2 canvasPos, ImVec2 canvasSize);

	std::string* GetBufferRef() { return &Buffer; }

private:
	void CreateModules() override;

public:
	std::shared_ptr<DasherUIScreen> GetScreenModule() { return ScreenModule; }
	bool SupportsClipboard() override { return true; }
	void CopyToClipboard(const std::string& text) override;

private:
	//Cursor position in the output buffer
	unsigned int Cursor = 0;
	//Output Buffer
	std::string Buffer;
	//Accumulated deltaTime
	unsigned long Time;

	//Modules
	std::shared_ptr<DasherUIScreen> ScreenModule;
	std::shared_ptr<SocketInput> SocketInputModule;
	std::shared_ptr<OSOutput> OSOutputModule;
};
