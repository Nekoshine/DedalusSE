/**
 * @file display.c
 * @author Chevelu Jonathan (jonathan.chevelu@irisa.fr)
 * @brief Methods to display a dedalus.
 * @version 0.1
 * @date 2019-01-24
 *
 * @copyright Copyright (c) 2019
 *
 */

#include <string.h>   // strcpy, strlen
#include <termios.h>  // for waituser (unix only)
#include <unistd.h>   //usleep

#include "ariadneString.h"
#include "display.h"
#include "gps.h"

/**********************************/
// Declaration of local constants.

// ANSI COLOR SCHEME
#define ANSI_COLOR_RED "\x1b[31m"      ///< Start red color
#define ANSI_COLOR_GREEN "\x1b[32m"    ///< Start green color
#define ANSI_COLOR_YELLOW "\x1b[33m"   ///< Start yellow color
#define ANSI_COLOR_BLUE "\x1b[34m"     ///< Start blue color
#define ANSI_COLOR_MAGENTA "\x1b[35m"  ///< Start magenta color
#define ANSI_COLOR_CYAN "\x1b[36m"     ///< Start cyan color
#define ANSI_COLOR_WHITE "\x1b[37m"    ///< Start white color
#define ANSI_COLOR_RESET "\x1b[0m"     ///< back to default color

// GAME COLOR SCHEME
#define PLAYER_COLOR ANSI_COLOR_RED        ///< Default color for players
#define EXIT_COLOR ANSI_COLOR_GREEN        ///< Default color for exits
#define WALL_COLOR ANSI_COLOR_BLUE         ///< Default color for walls
#define PATH_COLOR ANSI_COLOR_YELLOW       ///< Default color for paths
#define MINOTAUR_COLOR ANSI_COLOR_MAGENTA  ///< Default color for Minotaurs
#define DEFAULT_COLOR ANSI_COLOR_RESET     ///< Default color

/**********************************/
// Declaration of local functions.

/**
 * @brief Move cursor up to clear something for example.
 *
 * @param[in,out] stream The considered stream.
 * @param[in] nbRow Number of lines up.
 */
void _display_move_up(FILE* stream, size_t nbRow);

/**
 * @brief Ring the bell of a terminal
 *
 * @param[in,out] stream The considered stream.
 */
void _display_ring_bell(FILE* stream);
/**
 * @brief Remove the map in a stream.
 *
 * @param[in,out] stream Where it should be printed.
 * @param[in] pMap Map to clear.
 */
void _display_clear_map(FILE* stream, const map_t* pMap);

/**
 * @brief Get the string associated to a wall.
 *
 * @param[in] pMap The map to consider.
 * @param[in] p The position of the wall to consider.
 * @param[in] pMask A mask to define what position should be print. NULL to
 * print everything.
 * @return const char* The string for the wall.
 */
const char* _display_wall_to_string(const map_t* pMap,
                                    const map_t* pMask,
                                    const pos_t p);

/**
 * @brief Get the string for a path in the dedalus.
 *
 * @return const char* The string for a path.
 */
const char* _display_path_to_string();

/**
 * @brief Get the string for a player in the dedalus.
 *
 * @return const char* The string for a player.
 */
const char* _display_player_to_string();

/**
 * @brief Get the string for a dead character in the dedalus.
 *
 * @return const char* The string for a dead character.
 */
const char* _display_dead_to_string();

/**
 * @brief Get the string for an exit in the dedalus.
 *
 * @return const char* The string for an exit.
 */
const char* _display_exit_to_string();

/**
 * @brief Get the string for a Minotaur in the dedalus.
 *
 * @return const char* The string for a Minotaur.
 */
const char* _display_minotaur_to_string();

/**
 * @brief Display a map.
 *
 * @param[in,out] stream Where it should be printed.
 * @param[in] pMap The map to print.
 * @param[in] pMask A mask to define what position should be print. NULL to
 * print everything.
 *
 * @note If a mask is provided, positions in the neighborhood of marked
 * positions are displayed.
 */
void _display_map(FILE* stream, const map_t* pMap, const map_t* pMask);

/**
 * @brief Display the ariadne string.
 *
 * @param[in,out] stream Where it should be printed.
 * @param[in] s The ariadne string to display.
 */
void _display_string(FILE* stream, const string s);

/**
 * @brief Display a "Gameover" message.
 *
 * @param[in,out] stream Where it should be printed.
 */
void _display_gameover(FILE* stream);

/**
 * @brief Display a "try again" message.
 *
 * @param[in,out] stream Where it should be printed.
 */
void _display_try_again(FILE* stream);

/**
 * @brief Display a "Congratulation" message
 *
 * @param[in,out] stream Where it should be printed.
 */
void _display_congratulations(FILE* stream);

/**
 * @brief Display credits for the dedalus game.
 *
 * @param[in,out] stream Where it should be printed.
 */
void _display_credits(FILE* stream);

/**
 * @brief Display health of a character.
 *
 * @param[in,out] stream Where it should be printed.
 * @param[in] health health to print.
 */
void _display_health(FILE* stream, double health);

/**
 * * @brief Display the GM UI header.
 *
 * @param[in,out] stream Where it should be printed.
 * @param[in] level Level name.
 * @param[in] pMap The map (to display some information about).
 * @param[in] delay Delay between two frames (for information only).
 * @param[in] gameInfo Display more precise game informations.
 */
void _display_ui_gm_header(FILE* stream,
                           const char* level,
                           const map_t* pMap,
                           int delay,
                           bool gameInfo);

/**
 * @brief Display the GM UI body (the map).
 *
 * @param[in,out] stream Where it should be printed.
 * @param[in] pMap The map to display.
 * @param[in] nbPlayerAlive Number of players alive.
 * @param[in] nbPlayerOnBoard Number of player still on the board.
 * @param[in] nbMinotaurAlive Number of Minotaurs alive.
 * @param[in] refresh Should clear a previously printed body.
 */
void _display_ui_gm_body(FILE* stream,
                         const map_t* pMap,
                         size_t nbPlayerAlive,
                         size_t nbPlayerOnBoard,
                         size_t nbMinotaurAlive,
                         bool refresh);

/**
 * @brief Display a player UI header.
 *
 * @param[in,out] stream Where it should be printed.
 * @param[in] level Level name.
 * @param[in] ai AI name.
 * @param[in] pMap The map (to display some information about).
 * @param[in] pPlayer Player considered.
 * @param[in] delay Delay between two frames (for information only).
 * @param[in] maxMoves Max number of moved allowed.
 * @param[in] gameInfo Display more precise game informations.
 */
void _display_ui_player_header(FILE* stream,
                               const char* level,
                               const char* ai,
                               const map_t* pMap,
                               const character_t* pPlayer,
                               int delay,
                               int maxMoves,
                               bool gameInfo);

/**
 * @brief Display a player ui body (the map).
 *
 * @param[in,out] stream Where it should be printed.
 * @param[in] pMap Map to print.
 * @param[in] pPlayer Player considered.
 * @param[in] refresh If the body of the ui should be refresh.
 */
void _display_ui_player_body(FILE* stream,
                             const map_t* pMap,
                             const character_t* pPlayer,
                             bool refresh);

/**
 * @brief Display a ending message for a player.
 *
 * @param[in,out] stream Where it should be printed.
 * @param[in] ending The ending category.
 */
void _display_player_ending(FILE* stream, ending_char_t ending);

/**
 * @brief Display a fight scene.
 *
 * @param[in,out] stream Where it should be printed.
 * @param[in] endingScene Array of the scene.
 * @param[in] sceneLength Size of the scene.
 * @param[in] health Health of the considered character.
 * @param[in] opponent Name of the opponent.
 * @param[in] lastDisplay Is it the last display?
 */
void _display_fight(FILE* stream,
                    char endingScene[],
                    size_t sceneLength,
                    double health,
                    const char* opponent,
                    bool lastDisplay);

/**
 * @brief Display a fight introduction animation.
 *
 * @param[in,out] scene Array of the scene.
 * @param[in] sceneLength Size of the scene.
 * @param[in] pC1 First player in the fight.
 * @param[in] pC2 Second player in the fight.
 * @param[in] delay Delay (in us) between tow frames.
 */
void _display_fight_introduction(char* scene,
                                 size_t sceneLength,
                                 const character_t* pC1,
                                 const character_t* pC2,
                                 size_t delay);

/**
 * @brief Display the final message after a fight. Depending of who win.
 *
 * @param[in] pWinner The winner character.
 * @param[in] pLooser The looser character.
 * @param[in] tied Is it a tied (double KO).
 */
void _display_fight_ending(const character_t* pWinner,
                           const character_t* pLooser,
                           bool tied);

/*****************************/
// Functions implementation.

void _display_move_up(FILE* stream, size_t nbRow) {
  fprintf(stream, "\033[%luA", nbRow);
}

void _display_ring_bell(FILE* stream) {
  fprintf(stream, "\a");
}

void _display_clear_map(FILE* stream, const map_t* pMap) {
  size_t offset = 2 + 2;
  if (!DEBUG) {
    _display_move_up(stream, pMap->y + offset);
  }
  size_t nbCol = 50;
  if (nbCol < pMap->x) {
    nbCol = pMap->x;
  }
  for (size_t l = 0; l < pMap->y + 3; ++l) {
    for (size_t c = 0; c < nbCol; ++c) {
      fprintf(stream, " ");
    }
    fprintf(stream, "\n");
  }
  if (!DEBUG) {
    _display_move_up(stream, pMap->y + offset - 1);
  }
}

const char* _display_wall_to_string(const map_t* pMap,
                                    const map_t* pMask,
                                    const pos_t p) {
  bool wN = (p.y > 0) && (pMap->m[p.y - 1][p.x] == WALL) &&
            ((pMask == NULL) || (pMask->m[p.y - 1][p.x] == DISPLAY_MASK));
  bool wE = (p.x < (pMap->x - 1)) && (pMap->m[p.y][p.x + 1] == WALL) &&
            ((pMask == NULL) || (pMask->m[p.y][p.x + 1] == DISPLAY_MASK));
  bool wS = (p.y < (pMap->y - 1)) && (pMap->m[p.y + 1][p.x] == WALL) &&
            ((pMask == NULL) || (pMask->m[p.y + 1][p.x] == DISPLAY_MASK));
  bool wW = (p.x > 0) && (pMap->m[p.y][p.x - 1] == WALL) &&
            ((pMask == NULL) || (pMask->m[p.y][p.x - 1] == DISPLAY_MASK));

  if (wN && wS && wE && wW) {
    return "╬";
  } else if (wN && wS && wE) {
    return "╠";
  } else if (wN && wS && wW) {
    return "╣";
  } else if (wN && wE && wW) {
    return "╩";
  } else if (wS && wE && wW) {
    return "╦";
  } else if (wS && wE) {
    return "╔";
  } else if (wS && wW) {
    return "╗";
  } else if (wN && wE) {
    return "╚";
  } else if (wN && wW) {
    return "╝";
  } else if (wN && wS) {
    return "║";
  } else if (wE && wW) {
    return "═";
  } else if (wN) {
    return "╨";
  } else if (wS) {
    return "╥";
  } else if (wE) {
    return "╞";
  } else if (wW) {
    return "╡";
  } else {
    return "╬";
  }
  // return WALL; // Simple version
}

const char* _display_path_to_string() {
  // return "•";
  // return " ";
  return ".";
}

const char* _display_player_to_string() {
  //  return "⚲";
  // return "ጰ";
  // return "ᗧ";
  return "@";
}

const char* _display_dead_to_string() {
  // return "⚰️";
  // return "☠";
  // return "⚱";
  return "+";
}

const char* _display_exit_to_string() {
  // return "⎗";
  return "?";
}

const char* _display_minotaur_to_string() {
  // return "♞";
  return "&";
}

void _display_map(FILE* stream, const map_t* pMap, const map_t* pMask) {
  for (size_t l = 0; l < pMap->y; ++l) {
    for (size_t c = 0; c < pMap->x; ++c) {
      pos_t p;
      p.y = l;
      p.x = c;

      bool display = (pMask == NULL) || (pMask->m[l][c] == DISPLAY_MASK);

      if (display) {
        if (COLOR) {
          switch (pMap->m[l][c]) {
            case WALL:
              fprintf(stream, ANSI_COLOR_BLUE "%s" ANSI_COLOR_RESET,
                      _display_wall_to_string(pMap, pMask, p));
              break;
            case PATH:
              fprintf(stream, ANSI_COLOR_YELLOW "%s" ANSI_COLOR_RESET,
                      _display_path_to_string());
              break;
            case PLAYER:
              fprintf(stream, ANSI_COLOR_RED "%s" ANSI_COLOR_RESET,
                      _display_player_to_string());
              break;
            case EXIT:
              fprintf(stream, ANSI_COLOR_GREEN "%s" ANSI_COLOR_RESET,
                      _display_exit_to_string());
              break;
            case MINOTAUR:
              fprintf(stream, ANSI_COLOR_MAGENTA "%s" ANSI_COLOR_RESET,
                      _display_minotaur_to_string());
              break;
            case DEAD:
              fprintf(stream, "%s", _display_dead_to_string());
              break;
            default:
              fprintf(stream, "%c", pMap->m[l][c]);
          }
        } else {
          fprintf(stream, "%c", pMap->m[l][c]);
        }
      } else {
        fprintf(stream, " ");
      }
    }
    fprintf(stream, "\n");
  }
}

void _display_string(FILE* stream, const string s) {
  string tS = s;
  fprintf(stream, "%s", _display_player_to_string());
  while (tS != NULL) {
    fprintf(stream, "-%s", gps_compass_to_string(tS->c));
    tS = tS->next;
  }
}

void _display_gameover(FILE* stream) {
  if (COLOR) {
    fprintf(stream, ANSI_COLOR_MAGENTA);
  }
  fprintf(stream, " _____                        _____                \n");
  fprintf(stream, "|  __ \\                      |  _  |               \n");
  fprintf(stream, "| |  \\/ __ _ _ __ ___   ___  | | | |_   _____ _ __ \n");
  fprintf(stream, "| | __ / _` | '_ ` _ \\ / _ \\ | | | \\ \\ / / _ \\ '__|\n");
  fprintf(stream, "| |_\\ \\ (_| | | | | | |  __/ \\ \\_/ /\\ V /  __/ |   \n");
  fprintf(stream,
          " \\____/\\__,_|_| |_| |_|\\___|  \\___/  \\_/ \\___|_|   \n");
  if (COLOR) {
    fprintf(stream, ANSI_COLOR_RESET);
  }
}

void _display_try_again(FILE* stream) {
  if (COLOR) {
    fprintf(stream, ANSI_COLOR_BLUE);
  }
  fprintf(stream, " _______                               _        \n");
  fprintf(stream, "|__   __|             /\\              (_)       \n");
  fprintf(stream, "   | |_ __ _   _     /  \\   __ _  __ _ _ _ __   \n");
  fprintf(stream, "   | | '__| | | |   / /\\ \\ / _` |/ _` | | '_ \\  \n");
  fprintf(stream, "   | | |  | |_| |  / ____ \\ (_| | (_| | | | | | \n");
  fprintf(stream, "   |_|_|   \\__, | /_/    \\_\\__, |\\__,_|_|_| |_| \n");
  fprintf(stream, "            __/ |           __/ |               \n");
  fprintf(stream, "           |___/           |___/           \n");
  if (COLOR) {
    fprintf(stream, ANSI_COLOR_RESET);
  }
}
void _display_congratulations(FILE* stream) {
  if (COLOR) {
    fprintf(stream, ANSI_COLOR_GREEN);
  }
  fprintf(
      stream,
      " _____                             _         _       _   _              "
      "   _ \n");
  fprintf(
      stream,
      "/  __ \\                           | |       | |     | | (_)            "
      "   | |\n");
  fprintf(
      stream,
      "| /  \\/ ___  _ __   __ _ _ __ __ _| |_ _   _| | __ _| |_ _  ___  _ __  "
      "___| |\n");
  fprintf(
      stream,
      "| |    / _ \\| '_ \\ / _` | '__/ _` | __| | | | |/ _` | __| |/ _ \\| '_ "
      "\\/ __| |\n");
  fprintf(
      stream,
      "| \\__/\\ (_) | | | | (_| | | | (_| | |_| |_| | | (_| | |_| | (_) | | | "
      "\\__ \\_|\n");
  fprintf(stream,
          " \\____/\\___/|_| |_|\\__, |_|  "
          "\\__,_|\\__|\\__,_|_|\\__,_|\\__|_|\\___/|_| |_|___(_)\n");
  fprintf(
      stream,
      "                    __/ |                                               "
      "     \n");
  fprintf(
      stream,
      "                   |___/                                                "
      "     \n");
  if (COLOR) {
    fprintf(stream, ANSI_COLOR_RESET);
  }
}

void _display_credits(FILE* stream) {
  if (COLOR) {
    fprintf(stream, ANSI_COLOR_RED);
  }
  fprintf(stream, "                                   \n");
  fprintf(stream, "______         _       _           \n");
  fprintf(stream, "|  _  \\       | |     | |          \n");
  fprintf(stream, "| | | |___  __| | __ _| |_   _ ___ \n");
  fprintf(stream, "| | | / _ \\/ _` |/ _` | | | | / __|\n");
  fprintf(stream, "| |/ /  __/ (_| | (_| | | |_| \\__ \\\n");
  fprintf(stream,
          "|___/ \\___|\\__,_|\\__,_|_|\\__,_|___/v2*3.14159265359...\n");
  fprintf(stream,
          "by Pr Francois Goasdoue, Shaman team, ENSSAT-IRISA, Univ Rennes\n");
  fprintf(stream,
          "update by Dr Jonathan Chevelu, Expression team, ENSSAT-IRISA, Univ "
          "Rennes\n");
  if (COLOR) {
    fprintf(stream, ANSI_COLOR_RESET);
  }
}

void _display_health(FILE* stream, double health) {
  fprintf(stream, "Health: ");
  if (health > 80) {
    if (COLOR)
      fprintf(stream, ANSI_COLOR_GREEN);
    fprintf(stream, "%3.0f%% (excellent)", health);
    if (COLOR)
      fprintf(stream, ANSI_COLOR_RESET);
  } else if (health > 60) {
    if (COLOR)
      fprintf(stream, ANSI_COLOR_YELLOW);
    fprintf(stream, "%3.0f%% (good)", health);
    if (COLOR)
      fprintf(stream, ANSI_COLOR_RESET);
  } else if (health > 40) {
    if (COLOR)
      fprintf(stream, ANSI_COLOR_MAGENTA);
    fprintf(stream, "%3.0f%% (average)", health);
    if (COLOR)
      fprintf(stream, ANSI_COLOR_RESET);
  } else if (health > 20) {
    if (COLOR)
      fprintf(stream, ANSI_COLOR_BLUE);
    fprintf(stream, "%3.0f%% (poor)", health);
    if (COLOR)
      fprintf(stream, ANSI_COLOR_RESET);
  } else if (health > 0) {
    if (COLOR)
      fprintf(stream, ANSI_COLOR_RED);
    fprintf(stream, "%3.0f%% (danger)", health);
    if (COLOR)
      fprintf(stream, ANSI_COLOR_RESET);
  } else
    fprintf(stream, "%3.0f%% (dead)", health);
  fprintf(stream, "                      \n");
}

void _display_ui_gm_header(FILE* stream,
                           const char* level,
                           const map_t* pMap,
                           int delay,
                           bool gameInfo) {
  // Print header
  _display_credits(stream);
  if (gameInfo) {
    if (COLOR) {
      fprintf(stream, ANSI_COLOR_MAGENTA);
    }
    fprintf(stream, "[Map: %s]\n", level);
    fprintf(stream, "[Map: size is %lu x %lu]\n", pMap->x, pMap->y);
    fprintf(stream, "[Game settings: %d µs round delay]\n", delay);

    if (COLOR) {
      fprintf(stream, ANSI_COLOR_RESET);
    }
    fprintf(stream, "\nLet's play Game Master!\n");
  } else {
    fprintf(stream, "Let's play ");
    if (COLOR) {
      fprintf(stream, ANSI_COLOR_CYAN);
    }
    fprintf(stream, "Game Master\n\n\n");
    if (COLOR) {
      fprintf(stream, ANSI_COLOR_RESET);
    }
  }
}

void _display_ui_gm_body(FILE* stream,
                         const map_t* pMap,
                         size_t nbPlayerAlive,
                         size_t nbPlayerOnBoard,
                         size_t nbMinotaurAlive,
                         bool refresh) {
  if (refresh && !DEBUG) {
    _display_move_up(stream, pMap->y + 3);
  }
  fprintf(stream, "Number of player alive:                   %lu.      \n",
          nbPlayerAlive);
  fprintf(stream, "Number of player still in the dedalus:    %lu.      \n",
          nbPlayerOnBoard);
  fprintf(stream, "Number of minotaurs still in the dedalus: %lu.      \n",
          nbMinotaurAlive);
  _display_map(stream, pMap, NULL);
}

void _display_ui_player_header(FILE* stream,
                               const char* level,
                               const char* ai,
                               const map_t* pMap,
                               const character_t* pPlayer,
                               int delay,
                               int maxMoves,
                               bool gameInfo) {
  // Print header
  _display_credits(stream);
  if (gameInfo) {
    if (COLOR) {
      fprintf(stream, ANSI_COLOR_MAGENTA);
    }
    fprintf(stream, "[Map: %s]\n", level);
    fprintf(stream, "[Map: size is %lu x %lu]\n", pMap->x, pMap->y);
    fprintf(stream, "[Map: Player @ at: (%lu,%lu)]\n", pPlayer->pos.x,
            pPlayer->pos.y);
    fprintf(stream, "[Game settings: %d steps max]\n", maxMoves);
    fprintf(stream, "[Game settings: %d µs round delay]\n", delay);
    if (COLOR) {
      fprintf(stream, ANSI_COLOR_RESET);
    }
    fprintf(stream, "\nLet's play %s!\n", ai);
  } else {
    fprintf(stream, "Artificial Intelligence by ");
    if (COLOR) {
      fprintf(stream, ANSI_COLOR_CYAN);
    }
    fprintf(stream, "%s\n\n\n", ai);
    if (COLOR) {
      fprintf(stream, ANSI_COLOR_RESET);
    }
  }
}

void _display_ui_player_body(FILE* stream,
                             const map_t* pMap,
                             const character_t* pPlayer,
                             bool refresh) {
  if (refresh && !DEBUG) {
    size_t offset = 2 + 2;
    _display_move_up(stream, pMap->y + offset);
  }
  fprintf(stream, "Your target is (%s) at %.0f m                           \n",
          gps_compass_to_string(pPlayer->targetCompass),
          pPlayer->targetDistance);
  _display_health(stream, pPlayer->health);
  _display_map(stream, pMap, pPlayer->pMask);
  if (DEBUG) {
    _display_string(stream, pPlayer->ariadne);
    fprintf(stream, "\nAriadne' string size=%lu\n",
            string_size(pPlayer->ariadne));
  }
  _display_player_ending(stream, pPlayer->ending);
}

void _display_player_ending(FILE* stream, ending_char_t ending) {
  if (COLOR) {
    fprintf(stream, ANSI_COLOR_YELLOW);
  }
  _display_ring_bell(stream);
  switch (ending) {
    case EC_NO_ENDING:
      fprintf(stream, "\n\n");
      break;
    case EC_KILL_BY_M:
      // message come from the fight
      break;
    case EC_STARVE_NO_MINOTAUR:
      fprintf(stream, "You are exhausted...\n...You starve!\n");
      break;
    case EC_STARVE_MINOTAUR:
      fprintf(stream,
              "You are exhausted...\n...The Minotaur finds you unconscious and "
              "eats you!\n");
      break;
    case EC_CHEAT_MINOTAUR:
      fprintf(
          stream,
          "You bumped into a wall (invalid move)...\n...The Minotaur finds you "
          "knocked-out and eats you!\n");
      break;
    case EC_CHEAT_NO_MINOTAUR:
      fprintf(stream,
              "You bumped into a wall (invalid move)...\n...You starve!\n");
      break;
    case EC_ESCAPE:
      fprintf(stream, "You found the way to the next level!\n\n");
      break;
    case EC_WIN:
      fprintf(stream, "No more Minotaur and you found the exit!\n\n");
      break;
  }
  if (COLOR) {
    fprintf(stream, ANSI_COLOR_RESET);
  }
}

void _display_fight(FILE* stream,
                    char endingScene[],
                    size_t sceneLength,
                    double health,
                    const char* opponent,
                    bool lastDisplay) {
  _display_health(stream, health);
  if (COLOR) {
    fprintf(stream, ANSI_COLOR_YELLOW);
  }
  fprintf(stream, "You are facing %s! May enough force be with you...\n",
          opponent);
  if (COLOR) {
    fprintf(stream, ANSI_COLOR_RESET);
  }
  for (size_t i = 0; i < sceneLength; i++) {
    if (COLOR) {
      switch (endingScene[i]) {
        case PATH:
          fprintf(stream, ANSI_COLOR_YELLOW "%s" ANSI_COLOR_RESET,
                  _display_path_to_string());
          break;
        case PLAYER:
          fprintf(stream, ANSI_COLOR_RED "%s" ANSI_COLOR_RESET,
                  _display_player_to_string());
          break;
        case EXIT:
          fprintf(stream, ANSI_COLOR_GREEN "%s" ANSI_COLOR_RESET,
                  _display_exit_to_string());
          break;
        case MINOTAUR:
          fprintf(stream, ANSI_COLOR_MAGENTA "%s" ANSI_COLOR_RESET,
                  _display_minotaur_to_string());
          break;
        case DEAD:
          fprintf(stream, "%s", _display_dead_to_string());
          break;
        default:
          fprintf(stream, ANSI_COLOR_CYAN "%c" ANSI_COLOR_RESET,
                  endingScene[i]);
      }
    } else {
      fprintf(stream, "%c", endingScene[i]);
    }
  }
  fprintf(stream, "\n");
  if (!DEBUG && !lastDisplay) {
    _display_move_up(stream, 3);
  }
  _display_ring_bell(stream);
}

void _display_fight_introduction(char* scene,
                                 size_t sceneLength,
                                 const character_t* pC1,
                                 const character_t* pC2,
                                 size_t delay) {
  scene[0] = (char)pC1->type;
  scene[sceneLength - 1] = (char)pC2->type;
  // introduction
  for (size_t i = 1; i < (sceneLength / 2) - 1; ++i) {
    usleep((unsigned int)delay);
    scene[i - 1] = PATH;
    scene[i] = (char)pC1->type;
    scene[sceneLength - 1 - i] = (char)pC2->type;
    scene[sceneLength - i] = PATH;
    _display_fight(pC1->stream, scene, sceneLength, pC1->health, pC2->name,
                   false);
    _display_fight(pC2->stream, scene, sceneLength, pC2->health, pC1->name,
                   false);
  }
}

void _display_fight_ending(const character_t* pWinner,
                           const character_t* pLooser,
                           bool tied) {
  if (tied) {
    if (COLOR) {
      fprintf(pWinner->stream, ANSI_COLOR_MAGENTA);
      fprintf(pLooser->stream, ANSI_COLOR_MAGENTA);
    }
    fprintf(pWinner->stream,
            "Double KO! You died but at least not alone... RIP!\n");
    fprintf(pLooser->stream,
            "Double KO! You died but at least not alone... RIP!\n");
  } else {
    if (COLOR) {
      fprintf(pLooser->stream, ANSI_COLOR_MAGENTA);
      fprintf(pWinner->stream, ANSI_COLOR_GREEN);
    }
    fprintf(pWinner->stream, "You have just defeated your opponent!\n");
    fprintf(
        pLooser->stream,
        "Unfortunately, you were too weak to defeat your opponent... RIP!\n");
  }
  if (COLOR) {
    fprintf(pWinner->stream, ANSI_COLOR_RESET);
    fprintf(pLooser->stream, ANSI_COLOR_RESET);
  }
}

/**********************************/
// Public functions implementations.

void display_ui_gm(FILE* stream,
                   const char* level,
                   const map_t* pMap,
                   size_t nbPlayerAlive,
                   size_t nbPlayerOnBoard,
                   size_t nbMinotaurAlive,
                   int delay,
                   bool gameInfo,
                   bool refresh) {
  if (!refresh) {
    _display_ui_gm_header(stream, level, pMap, delay, gameInfo);
  }
  _display_ui_gm_body(stream, pMap, nbPlayerAlive, nbPlayerOnBoard,
                      nbMinotaurAlive, refresh);
}

void display_ui_player(const char* level,
                       const char* ia,
                       const map_t* pMap,
                       const character_t* pPlayer,
                       int delay,
                       int maxMoves,
                       bool gameInfo,
                       bool header,
                       bool refresh) {
  if (header) {
    _display_ui_player_header(pPlayer->stream, level, ia, pMap, pPlayer, delay,
                              maxMoves, gameInfo);
  }
  _display_ui_player_body(pPlayer->stream, pMap, pPlayer, refresh);
}

void display_ending(FILE* stream, ending_t ending) {
  bool gameover = false;
  bool congrate = false;
  bool tryAgain = false;
  if (COLOR) {
    fprintf(stream, ANSI_COLOR_YELLOW);
  }
  switch (ending) {
    case EG_LOOSE:
      fprintf(stream, "It is a complete failure!\n");
      gameover = true;
      break;
    case EG_NEXT_LEVEL_AND_ALIVE:
      congrate = true;
      break;
    case EG_NEXT_LEVEL_BUT_DEAD:
      fprintf(stream,
              "At least, some teamates found a path to the next level!\n");
      tryAgain = true;
      break;
    case EG_GM_WIN:
      fprintf(stream, "You defeat them all!\n");
      congrate = true;
      break;
    case EG_GM_LOOSE:
      fprintf(stream, "Some players escaped your evil plan!\n");
      gameover = true;
      break;
    case EG_WIN_AND_ALIVE:
      congrate = true;
      break;
    case EG_KILL_MINOTAUR_BUT_DEAD:
      fprintf(stream, "At least, the minotaur is dead!\n");
      tryAgain = true;
      break;
    case EG_WIN_BUT_DEAD:
      fprintf(stream,
              "At least, the minotaur is dead and some teamates survived!\n");
      tryAgain = true;
      break;
  }
  if (COLOR) {
    fprintf(stream, ANSI_COLOR_RESET);
  }

  if (gameover) {
    _display_gameover(stream);
  }
  if (congrate) {
    _display_congratulations(stream);
  }
  if (tryAgain) {
    _display_try_again(stream);
  }
}

void display_fight_iteration(const map_t* pMap,
                             const character_t* pC1,
                             const character_t* pC2,
                             size_t delay,
                             size_t iteration) {
  // Setup fight
  char scene[] = {PATH, PATH, PATH, PATH, PATH, PATH, PATH, PATH,
                  PATH, PATH, PATH, PATH, PATH, PATH, '\0'};
  size_t sceneLength = strlen(scene);
  if (iteration == 0) {
    // Introduction step
    _display_clear_map(pC1->stream, pMap);
    _display_clear_map(pC2->stream, pMap);

    _display_fight_introduction(scene, sceneLength, pC1, pC2, delay);
  } else if ((pC1->health > 0) && (pC2->health > 0)) {
    // Real fight
    char aniC1[] = {'|', '/', '-', '/', '\0'};
    size_t aniC1Len = strlen(aniC1);
    size_t aniC1Id = iteration % aniC1Len;
    char aniC2[] = {'\\', '-', '\\', '|', '\0'};
    size_t aniC2Len = strlen(aniC2);
    size_t aniC2Id = iteration % aniC2Len;

    scene[(sceneLength / 2) - 2] = (char) pC1->type;
    scene[(sceneLength / 2) - 1] = aniC1[aniC1Id];
    scene[(sceneLength / 2)] = aniC2[aniC2Id];
    scene[(sceneLength / 2) + 1] = (char)pC2->type;
    _display_fight(pC1->stream, scene, sceneLength, pC1->health, pC2->name,
                   false);
    _display_fight(pC2->stream, scene, sceneLength, pC2->health, pC1->name,
                   false);
  } else {  // End of the fight
    bool draw = false;
    const character_t* pWinner = pC1;  // Default winner
    const character_t* pLooser = pC2;  // Default looser
    // How win ?
    if ((pC1->health <= 0) && (pC2->health <= 0)) {
      // Tied
      draw = true;
      scene[(sceneLength / 2) - 2] = DEAD;
      scene[(sceneLength / 2) + 1] = DEAD;
    } else if ((pC1->health <= 0)) {
      // P2 win
      pWinner = pC2;
      pLooser = pC1;
      scene[(sceneLength / 2) - 2] = DEAD;
      scene[(sceneLength / 2)] = '\\';
      scene[(sceneLength / 2) + 1] = (char)pC2->type;
      scene[(sceneLength / 2) + 2] = '/';
    } else {
      // P1 win
      scene[(sceneLength / 2) - 3] = '\\';
      scene[(sceneLength / 2) - 2] = (char)pC1->type;
      scene[(sceneLength / 2) - 1] = '/';
      scene[(sceneLength / 2) + 1] = DEAD;
    }
    _display_fight(pC1->stream, scene, sceneLength, pC1->health, pC2->name,
                   true);
    _display_fight(pC2->stream, scene, sceneLength, pC2->health, pC1->name,
                   true);
    _display_fight_ending(pWinner, pLooser, draw);
  }
}

void display_fight_clear(FILE* stream) {
  if (!DEBUG) {
    size_t offset = 4;
    _display_move_up(stream, offset);
    for (size_t i = 0; i < offset; ++i) {
      fprintf(stream,
              "                                                                "
              "  \n");
    }
    _display_move_up(stream, offset);
  }
}
void display_fatal_error(FILE* stream, const char* msg) {
  if (COLOR) {
    fprintf(stream, ANSI_COLOR_RED);
  }
  fprintf(stream,
          " ______   _______       _        ______ _____  _____   ____  "
          "_____  \n");
  fprintf(stream,
          "|  ____/\\|__   __|/\\   | |      |  ____|  __ \\|  __ \\ / __ \\|  "
          "__ \\ \n");
  fprintf(stream,
          "| |__ /  \\  | |  /  \\  | |      | |__  | |__) | |__) | |  | | "
          "|__) |\n");
  fprintf(stream,
          "|  __/ /\\ \\ | | / /\\ \\ | |      |  __| |  _  /|  _  /| |  | |  "
          "_  / \n");
  fprintf(stream,
          "| | / ____ \\| |/ ____ \\| |____  | |____| | \\ \\| | \\ \\| |__| | "
          "| \\ \\ \n");
  fprintf(stream,
          "|_|/_/    \\_\\_/_/    \\_\\______| |______|_|  \\_\\_|  "
          "\\_\\\\____/|_|  \\_\\\n");
  fprintf(stream,
          "                                                                    "
          " \n");
  fprintf(stream, "%s", msg);
  if (COLOR) {
    fprintf(stream, ANSI_COLOR_RESET);
  }
}

void display_info(FILE* stream, const char* msg) {
  if (COLOR) {
    fprintf(stream, ANSI_COLOR_GREEN);
  }
  fprintf(stream, "%s", msg);
  if (COLOR) {
    fprintf(stream, ANSI_COLOR_RESET);
  }
}

void display_wait_user(FILE* stream, const char* preMsg, bool waitUser) {
  if (waitUser) {
    if (preMsg != NULL) {
      display_info(stream, preMsg);
      fprintf(stream, "\n");
    }

    int fd = fileno(stream);
    struct termios oldT;
    struct termios newT;

    tcgetattr(fd, &oldT);
    newT = oldT;
    newT.c_lflag &= (unsigned int)( ~(ICANON | ECHO));
    tcsetattr(fd, TCSANOW, &newT);
    (void)getchar();
    tcsetattr(fd, TCSANOW, &oldT);
    if (preMsg != NULL) {
      _display_move_up(stream, 1);  // suppose that msg contains only one line
      fprintf(stream,
              "                                                                "
              "  \n");
      _display_move_up(stream, 1);  // suppose that msg contains only one line
    }
  } else {
    sleep(1);
  }
}
