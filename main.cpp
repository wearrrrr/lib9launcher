#include <thcrap.h>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdint.h>
#include "main.hpp"

#undef IN

struct GameDataOffsets {
  uintptr_t lives;
  uintptr_t bombs;
  uintptr_t power;
  uintptr_t character;
  uintptr_t shottype;
  uintptr_t score;
  uintptr_t stage;
  uintptr_t difficulty;
  uintptr_t gamestate;
  uint8_t game;
};

enum GameId : uint8_t {
  EoSD,
  PCB,
  IN,
  PoFV,
  StB,
  MoF,
  SA,
  UFO,
  DS,
  GFW,
  TD,
  DDC,
  ISC,
  LoLK,
  HSiFS,
  VD,
  WBaWC,
  UM,
  HBM,
  UDoALG,
  GAME_UNKNOWN
};

static const char* game_strs[] {
    "th06",
    "th07",
    "th08",
    "th09",
    "th095",
    "th10",
    "th11",
    "th12",
    "th125",
    "th128",
    "th13",
    "th14",
    "th143",
    "th15",
    "th16",
    "th165",
    "th17",
    "th18",
    "th185",
    "th19",
};

GameId GAME_ID;

static const GameDataOffsets game_offsets_lookup[] {
    // Currently only EoSD is supported, I had to spend all day refactoring the code give me a break D:
    [EoSD] = {
        .lives = TH06_LIVES_ADDR,
        .bombs = TH06_BOMBS_ADDR,
        .power = TH06_POWER_ADDR,
        .character = TH06_CHAR_ADDR,
        .shottype = TH06_SHOTTYPE_ADDR,
        .score = TH06_SCORE_ADDR,
        .stage = TH06_STAGE_ADDR,
        .difficulty = TH06_DIFFICULTY_ADDR,
        .gamestate = TH06_GAMESTATE_ADDR,
        .game = EoSD
    },
  };

const GameDataOffsets* current_game_offsets;


DWORD __stdcall mem_scan(void*)
{
    while (true)
    {
        HANDLE hFile = CreateFileA("Z:/dev/shm/9launcher/data.json", GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
        if (hFile == INVALID_HANDLE_VALUE) return 1;
        // Write the content of the buffer to the file
        DWORD bytesWritten;
        
        // Get the game data from game_offsets_lookup
        current_game_offsets = &game_offsets_lookup[GAME_ID];
    
        // Write information to JSON file, make sure its formatted with tabs
        // TODO: make this a bit more elegant, maybe use a library
        const char *json = "{\n\t\"lives\": %d,\n\t\"bombs\": %d,\n\t\"power\": %d,\n\t\"character\": %d,\n\t\"shottype\": %d,\n\t\"score\": %d,\n\t\"stage\": %d,\n\t\"difficulty\": %d,\n\t\"gamestate\": %d,\n\t\"game\": %d\n}";
        char *buffer = (char *)malloc(strlen(json) + 4);
        __ms_sprintf(buffer, json, *(uint8_t *)current_game_offsets->lives, *(uint8_t *)current_game_offsets->bombs, *(uint8_t *)current_game_offsets->power, *(uint8_t *)current_game_offsets->character, *(uint8_t *)current_game_offsets->shottype, *(uint32_t *)current_game_offsets->score, *(uint8_t *)current_game_offsets->stage, *(uint8_t *)current_game_offsets->difficulty, *(uint8_t *)current_game_offsets->gamestate, (uint8_t)current_game_offsets->game);
        WriteFile(hFile, buffer, strlen(buffer), &bytesWritten, NULL);

        CloseHandle(hFile);
        free(buffer);
        Sleep(300);
    }
}

inline bool get_game_id(const char* find_str) {
  for (size_t i = 0; i < countof(game_strs); ++i) {
    if (!strcmp(find_str, game_strs[i])) {
      GAME_ID = (GameId)i;
      return true;
    }
  }
  return false;
}

extern "C"
{

    TH_EXPORT int thcrap_plugin_init()
    {
        log_print("[9L] Starting...\n");

        // Check if folder /dev/shm/9launcher/ exists
        DWORD nineLauncherTempDir = GetFileAttributesA("Z:/dev/shm/9launcher/");
        if (nineLauncherTempDir == INVALID_FILE_ATTRIBUTES)
        {
            if (!CreateDirectoryA("Z:/dev/shm/9launcher/", NULL))
            {
                return 1;
            }
        }

        HANDLE hFile = CreateFileA("Z:/dev/shm/9launcher/data.json", GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
        if (hFile == INVALID_HANDLE_VALUE) return 1;

        CloseHandle(hFile);

        // Get the game ID from the runconfig
        get_game_id(runconfig_game_get());
        // Start the memory scanning thread :D
        HANDLE hThread = CreateThread(NULL, 0, mem_scan, NULL, 0, NULL);
        return 0;
    }

    //APIENTRY BOOL DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
    BOOL WINAPI DllMainCRTStartup(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
    {
        if (ul_reason_for_call == DLL_PROCESS_DETACH) {
            log_print("[9L] Stopping lib9launcher.. \n");
            DeleteFileA("Z:/dev/shm/9launcher/data.json");
        }
        return true;
    }
}
