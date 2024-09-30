#pragma once
#include <string>


class OSOutput
{
public:
    OSOutput();
    virtual ~OSOutput();

    void deleteCharacter();
    void outputCharacter(const std::string& sText);
};
