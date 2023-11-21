#include <thcrap.h>
#include <windows.h>

void th06_mem_scan()
{
    while (true)
    {
        HANDLE hFile = CreateFileA("Z:/dev/shm/9launcher/data.json", GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
        if (hFile == INVALID_HANDLE_VALUE)
        {
            log_print("[9L] Failed to create file!\n");
            return;
        }
        // Write the content of the buffer to the file
        DWORD bytesWritten;
        int8_t lives = *(char *)0x0069d4ba;
        int8_t bombs = *(char *)0x0069d4bb;
        unsigned short power = *(unsigned short *)0x0069d4b0;
        int8_t character = *(char *)0x0069d4bd;
        int8_t shottype = *(char *)0x0069d4be;
        int score = *(int *)0x0069bca4;
        int stage = *(int *)0x0069d6d4;
        int difficulty = *(int *)0x0069bcb0;
        int gamestate = *(int *)0x006c6ea0;

        // Write information to JSON file, make sure its formatted with tabs
        // TODO: make this a bit more elegant, maybe use a library
        const char *json = "{\n\t\"lives\": %d,\n\t\"bombs\": %d,\n\t\"power\": %d,\n\t\"character\": %d,\n\t\"shottype\": %d,\n\t\"score\": %d,\n\t\"stage\": %d,\n\t\"difficulty\": %d,\n\t\"gamestate\": %d\n}";
        char *buffer = (char *)malloc(strlen(json) + 4);
        sprintf(buffer, json, lives, bombs, power, character, shottype, score, stage, difficulty, gamestate);
        WriteFile(hFile, buffer, strlen(buffer), &bytesWritten, NULL);

        CloseHandle(hFile);

        Sleep(300);
    }
}

extern "C"
{

    TH_EXPORT int thcrap_plugin_init()
    {
        log_print("[9L] Starting lib9launcher...\n");

        // Check if folder /dev/shm/9launcher/ exists
        DWORD nineLauncherTempDir = GetFileAttributesA("Z:/dev/shm/9launcher/");
        if (nineLauncherTempDir == INVALID_FILE_ATTRIBUTES)
        {
            log_print("[9L] Creating directory...\n");
            if (!CreateDirectoryA("Z:/dev/shm/9launcher/", NULL))
            {
                log_print("[9L] Failed to create directory!\n");
                return 1;
            }
        }

        HANDLE hFile = CreateFileA("Z:/dev/shm/9launcher/data.json", GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
        if (hFile == INVALID_HANDLE_VALUE)
        {
            log_print("[9L] Failed to create file!\n");
            return 1;
        }

        CloseHandle(hFile);

        const char *game = runconfig_game_get();
        log_printf("[9L] Game: %s\n", game);
        if (strcmp(game, "th06") == 0) {
            HANDLE hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)th06_mem_scan, NULL, 0, NULL);
        }

        return 0;
    }

    APIENTRY BOOL DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
    {

        switch (ul_reason_for_call)
        {
            case DLL_PROCESS_DETACH:
                log_print("[9L] Stopping lib9launcher.. \n");
                DeleteFileA("Z:/dev/shm/9launcher/data.json");
                break;
        }

        return true;
    }
}