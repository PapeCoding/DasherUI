#include "DasherController.h"

class XmlServerStore;

DasherController::DasherController(Dasher::CSettingsStore* pSettingsStore): CDashIntfScreenMsgs(pSettingsStore)
{
	ScreenModule = std::make_shared<DasherUIScreen>();
	CDashIntfScreenMsgs::ChangeScreen(ScreenModule.get());

	OSOutputModule = std::make_unique<OSOutput>();
}

void DasherController::editOutput(const std::string& strText, Dasher::CDasherNode* pNode) {

	if (Buffer.capacity() < strText.length() + Buffer.size()) {
		Buffer.reserve((strText.length() + Buffer.size()) * 2);
	}
	Buffer.append(strText);
	Cursor += static_cast<unsigned int>(strText.length());
	OSOutputModule->outputCharacter(strText);
	CDasherInterfaceBase::editOutput(strText, pNode);
}

void DasherController::editDelete(const std::string& strText, Dasher::CDasherNode* pNode) {
	if(0 == Buffer.compare(Buffer.length() - strText.length(), strText.length(), strText))
	{
		Buffer.erase(Buffer.length() - strText.length(), strText.length());
		OSOutputModule->deleteCharacter();
	}
	CDasherInterfaceBase::editDelete(strText, pNode);
}

unsigned DasherController::ctrlMove(bool bForwards, Dasher::EditDistance dist)
{
	if (dist == Dasher::EditDistance::EDIT_CHAR) {
		if (bForwards) Cursor++;
		else Cursor--;
	}
	return Cursor;
}

unsigned DasherController::ctrlDelete(bool bForwards, Dasher::EditDistance dist)
{
	if(dist == Dasher::EditDistance::EDIT_CHAR) {

		Buffer.erase(Cursor - (bForwards ? 0 : 1), 1);
	}
	if(!bForwards) Cursor--;
	return Cursor;
}

std::string DasherController::GetContext(unsigned iStart, unsigned iLength)
{
	return Buffer.substr(iStart, iLength);
}

std::string DasherController::GetAllContext()
{
	std::string CurrentBuffer = Buffer;
	SetBuffer(0);
	return CurrentBuffer;
}

std::string DasherController::GetTextAroundCursor(Dasher::EditDistance iDist) {
	if (Buffer.length() > Cursor && Buffer.length() >= 2) {
		if (iDist == Dasher::EditDistance::EDIT_CHAR) {
			return Buffer.substr(Cursor - 1, 2);
		}

		std::cerr << "tried to get more than just a char" << std::endl;
		return "";
	}
	std::cerr << "Cursor out of bounds" << std::endl;
	return "";
}


//TODO: Some day fix typo in function name
int DasherController::GetAllContextLenght()
{
	return static_cast<int>(Buffer.length());
}

void DasherController::Initialize()
{
	Realize(0);
}

void DasherController::CreateModules()
{
	CDashIntfScreenMsgs::CreateModules();

    GetModuleManager()->RegisterInputDeviceModule(ScreenModule.get(), true);
}

void DasherController::CopyToClipboard(const std::string& text)
{
	ImGui::SetClipboardText(text.c_str());
}

void DasherController::Render(long deltaTime, ImVec2 canvasPos, ImVec2 canvasSize)
{
	Time += deltaTime;

	NewFrame(Time, true);

	if(ScreenModule->SetCanvasSize(canvasPos, canvasSize))
	{
		ScreenResized(ScreenModule.get());
	}

	//Handle Input
	if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) && ImGui::IsMouseHoveringRect(canvasPos, canvasPos + canvasSize))
	{
		KeyDown(Time, Dasher::Keys::Primary_Input); //100 is the keycode for LMB
	}

	if(ImGui::IsMouseReleased(ImGuiMouseButton_Left))
	{
		KeyUp(Time, Dasher::Keys::Primary_Input); //100 is the keycode for LMB
	}
}
