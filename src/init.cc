#include "bm2dxi.h"

auto DllMain(HINSTANCE module, DWORD reason, LPVOID) -> BOOL
{
    if (reason != DLL_PROCESS_ATTACH)
        return TRUE;

    DisableThreadLibraryCalls(module);

    try
    {
        SteamErrMsg error;

        if (SteamAPI_InitEx(&error) != k_ESteamAPIInitResult_OK)
            return FALSE;

        auto const base = reinterpret_cast<std::uint8_t*>(GetModuleHandle(nullptr));
        auto const dos = reinterpret_cast<PIMAGE_DOS_HEADER>(base);
        auto const nt = reinterpret_cast<PIMAGE_NT_HEADERS>(base + dos->e_lfanew);

        bm2dxi::init({ base, nt->OptionalHeader.SizeOfImage });

        SteamTimeline()->SetTimelineGameMode(k_ETimelineGameMode_Menus);
    }
    catch (const std::exception& err)
    {
        MessageBoxA(nullptr, err.what(), "infsteam initialization error", MB_ICONERROR);
        return FALSE;
    }

    return TRUE;
}