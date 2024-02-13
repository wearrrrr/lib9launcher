#include <thcrap.h>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdint.h>
#include "main.hpp"

#undef IN
#pragma GCC diagnostic ignored "-Wc99-designator"

char* alloc_vsprintf(const char* format, va_list va) {
    va_list va2;
    va_copy(va2, va);
    int length = vsnprintf(NULL, 0, format, va2);
    va_end(va2);
    char* buffer = (char*)malloc(length + 1);
    if (!buffer) {
      return NULL;
    }
    va_copy(va2, va);
    int ret = vsprintf(buffer, format, va2);
    va_end(va2);
    if (ret != -1) {
      return buffer;
    } else {
      free(buffer);
      return NULL;
    }
  };

char* alloc_sprintf(const char* format, ...) {
  va_list va;
  va_start(va, format);
  char* ret = alloc_vsprintf(format, va);
  va_end(va);
  return ret;
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

struct GameDataOffsetReturns {
  int32_t lives;
  int32_t bombs;
  int32_t power;
  int32_t character;
  int32_t shottype;
  int32_t score;
  int32_t stage;
  int32_t difficulty;
  int32_t gamestate;
  int32_t game;
};

struct GameDataOffsets {
  uintptr_t game_manager;

  uintptr_t globals_offset;
  bool lives_are_float;
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


  uintptr_t get_global_offset(uintptr_t address) const {
    uintptr_t globals_offset = address;
    uintptr_t globals = *(uintptr_t*)globals_offset;
    return globals;
  };

  boolean is_game_manager_init(uintptr_t address) const {
    uintptr_t game_manager = *(uintptr_t*)address;
    if (game_manager == 0) {
      return false;
    }
    return true;
  };

  // Convert the floating point lives and bombs into uint8_t, which the rest of the games use
  // ZUN why did you have to make this so complicated??
  uint8_t get_floating_lives(uintptr_t address) const {
      uintptr_t globals = get_global_offset(address);
      if (globals == 0) {
        return 0;
      }
      uintptr_t lives_offset = globals + 0x5C;

      return !lives_are_float ? *(uint8_t*)lives_offset : (uint8_t)*(float*)lives_offset;
  };
  uint8_t get_floating_bombs(uintptr_t address) const {
      uintptr_t globals = get_global_offset(address);
      if (globals == 0) {
        return 0;
      }

      uintptr_t bombs_offset = globals + 0x68;

      return !lives_are_float ? *(uint8_t*)bombs_offset : (uint8_t)*(float*)bombs_offset;
  };
  uint8_t get_floating_power(uintptr_t address) const {
      uintptr_t globals = get_global_offset(address);
      if (globals == 0) {
        return 0;
      }

      uintptr_t power_offset = globals + 0x7c;

      return !lives_are_float ? *(uint8_t*)power_offset : (uint8_t)*(float*)power_offset;
  };
  uint32_t get_score(uintptr_t address) const {
      uintptr_t globals = get_global_offset(address);
      if (globals == 0) {
        return 0;
      }
      uintptr_t score_offset = globals + 0x0;

      return *(uint32_t*)score_offset;
  };
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
    [PCB] = {
      .game_manager = TH07_GAMEMANAGER_ADDR,
      .globals_offset = TH07_GLOBALS_ADDR,
      .lives_are_float = true,
      .lives = TH07_GLOBALS_ADDR + 0x5C,
      .bombs = TH07_GLOBALS_ADDR + 0x68,
      .power = TH07_GLOBALS_ADDR + 0x7C,
      .character = TH07_CHARACTER_ADDR,
      .shottype = TH07_SHOTTYPE_ADDR,
      .score = TH07_GLOBALS_ADDR + 0x4,
      .stage = TH07_STAGE_ADDR,
      .difficulty = TH07_DIFFICULTY_ADDR,
      .game = PCB
    },
  };

const GameDataOffsets* current_game_offsets;

uint8_t read_byte(uintptr_t address) {
  if (address == 0) {
    return 0;
  }
  return *(uint8_t*)address;
}

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
        uint8_t lives = current_game_offsets->lives_are_float ? current_game_offsets->get_floating_lives(current_game_offsets->globals_offset) : *(uint8_t*)current_game_offsets->lives;
        uint8_t bombs = current_game_offsets->lives_are_float ? current_game_offsets->get_floating_bombs(current_game_offsets->globals_offset) : *(uint8_t*)current_game_offsets->bombs;
        uint8_t power = current_game_offsets->lives_are_float ? current_game_offsets->get_floating_power(current_game_offsets->globals_offset) : *(uint8_t*)current_game_offsets->power;
        uint32_t score = current_game_offsets->lives_are_float ? current_game_offsets->get_score(current_game_offsets->globals_offset) : *(uint32_t*)current_game_offsets->score;


        // Write information to JSON file, make sure its formatted with tabs
        // TODO: make this a bit more elegant, maybe use a library
        const char* json = "{\n\t\"lives\": %d,\n\t\"bombs\": %d,\n\t\"power\": %d,\n\t\"score\": %d,\n\t\"character\": %d,\n\t\"shottype\": %d,\n\t\"stage\": %d,\n\t\"difficulty\": %d,\n\t\"gamestate\": %d,\n\t\"game\": %d\n}";
        char* buf = alloc_sprintf(json, lives, bombs, power, score, read_byte(current_game_offsets->character), read_byte(current_game_offsets->shottype), read_byte(current_game_offsets->stage), read_byte(current_game_offsets->difficulty), current_game_offsets->is_game_manager_init(current_game_offsets->game_manager), current_game_offsets->game);
        WriteFile(hFile, buf, strlen(buf), &bytesWritten, NULL);


        free(buf);
        CloseHandle(hFile);
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
    BOOL WINAPI DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
    {
        if (ul_reason_for_call == DLL_PROCESS_DETACH) {
            log_print("[9L] Stopping lib9launcher.. \n");
            DeleteFileA("Z:/dev/shm/9launcher/data.json");
        }
        return true;
    }
}