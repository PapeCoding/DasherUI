#include "OSOutput.h"

#include <string>

#ifdef _WIN32
#include <codecvt>

#define WIN32_LEAN_AND_MEAN      // Exclude rarely-used stuff from Windows headers
#include <windows.h>
#include <stringapiset.h>
#include <WinUser.h>
#undef WIN32_LEAN_AND_MEAN
#endif

OSOutput::OSOutput(){}

OSOutput::~OSOutput(){}

void OSOutput::deleteCharacter()
{
#ifdef _WIN32
    INPUT fakekey[2];
    fakekey[0].type = fakekey[1].type = INPUT_KEYBOARD;
    fakekey[0].ki.wVk = fakekey[1].ki.wVk = VK_BACK;
    fakekey[0].ki.time = fakekey[1].ki.time = 0;
    fakekey[1].ki.dwFlags = KEYEVENTF_KEYUP;

    SendInput(2, fakekey, sizeof(INPUT));
#endif
}

void OSOutput::outputCharacter(const std::string &sText)
{
#ifdef _WIN32
    INPUT fakekey[2];
    if(sText[0] == '\r' && sText[1] == '\n') {
        // Newline, so we want to fake an enter
        fakekey[0].type = fakekey[1].type = INPUT_KEYBOARD;
        fakekey[0].ki.wVk = fakekey[1].ki.wVk = VK_RETURN;
        fakekey[0].ki.time = fakekey[1].ki.time = 0;
        fakekey[1].ki.dwFlags = KEYEVENTF_KEYUP;
        SendInput(2, fakekey, sizeof(INPUT));
    }
    else {
        int wchars_num = MultiByteToWideChar( CP_UTF8 , 0 , sText.c_str() , -1, nullptr , 0 );
        wchar_t* wstr = new wchar_t[wchars_num];
        MultiByteToWideChar( CP_UTF8 , 0 , sText.c_str() , -1, wstr , wchars_num );

        for(int i = 0; i < wchars_num; ++i) {
            fakekey[0].type = INPUT_KEYBOARD;
            fakekey[0].ki.dwFlags = KEYEVENTF_UNICODE;
            fakekey[0].ki.wVk = 0;
            fakekey[0].ki.time = NULL;
            fakekey[0].ki.wScan = wstr[i];
            SendInput(1, fakekey, sizeof(INPUT));
        }

        delete[] wstr;
    }
#endif
}
