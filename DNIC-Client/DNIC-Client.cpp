#include <NosStdLib/Global.hpp>

#include <iostream>
#include <conio.h>

int main()
{
    NosStdLib::Global::Console::InitializeModifiers::EnableUnicode();
    NosStdLib::Global::Console::InitializeModifiers::EnableANSI();

    wprintf(L"Some test text\n");

    wprintf(L"Press any button to continue"); _getch();
    return 0;
}