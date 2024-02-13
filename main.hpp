#define countof(array_type) (sizeof(array_type) / sizeof(array_type[0]))
#define btrstrcmp(str1, str2) (!strcmp(str1, str2))

// Touhou 6
#define TH06_LIVES_ADDR 0x0069d4ba
#define TH06_BOMBS_ADDR 0x0069d4bb
#define TH06_POWER_ADDR 0x0069d4b0
#define TH06_CHAR_ADDR 0x0069d4bd
#define TH06_SHOTTYPE_ADDR 0x0069d4be
#define TH06_SCORE_ADDR 0x0069bca4
#define TH06_STAGE_ADDR 0x0069d6d4
#define TH06_DIFFICULTY_ADDR 0x0069bcb0
#define TH06_GAMESTATE_ADDR 0x006c6ea0

// Touhou 7
#define TH07_GAMEMANAGER_ADDR 0x00626270
#define TH07_GLOBALS_ADDR 0x00626278
#define TH07_CHARACTER_ADDR 0x0062F645
#define TH07_SHOTTYPE_ADDR 0x0062F646
#define TH07_STAGE_ADDR 0x0062F85C
#define TH07_DIFFICULTY_ADDR 0x00626280
#define TH07_GAMESTATE_ADDR 0x0134D9CC