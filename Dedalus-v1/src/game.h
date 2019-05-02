/**
 * @file game.h
 * @author Chevelu Jonathan (jonathan.chevelu@irisa.fr)
 * @brief Dedalus game controller.
 * @version 0.1
 * @date 2019-01-25
 *
 * @copyright Copyright (c) 2019
 *
 */
#ifndef GAME_H
#define GAME_H

#include "character.h"
#include "config.h"
#include "map.h"

/**
 * @brief Default display stream for the Game Master.
 *
 */
#define DISPLAY stdout

/**
 * @brief Structure of a game.
 *
 */
typedef struct game {
  const char* gameName;  ///< The name of the level (i.e. the map name).
  map_t* pMap;           ///< The map.
  pos_t* exitA;          ///< Array of exits positions.
  size_t nbExit;         ///< Number of exits.
  character_t* playerA;  ///< Array of players.
  size_t nbPlayerAlive;  ///< Number of players still alive.
  size_t
      nbPlayerOnBoard;  ///< Number of players on board (not dead and not out).
  size_t nbPlayer;      ///< Number of players.
  character_t* minotaurA;  ///< Array of minotaurs.
  size_t nbMinotaur;       ///< Number of minotaurs.
  size_t nbMinotaurAlive;  ///< Number of minotaurs still alive.
  bool finalLevel;         ///< Is this level a final level (contains minotaurs).
  bool gameInfo;           ///< Display more precise game informations.
  int maxMoves;            ///< Maximum number of moves per player.
  int steps;               ///< Number of steps since the beginning of the game.
  int delay;               ///< Delay (in us) between tow steps.
  bool interactive;        ///< Ask for interactive actions from GM.
} game_t;

/**
 * @brief Initialize a game.
 *
 * @param[out] pGame The game to initialise.
 * @param[in,out] pConf Configuration structure for the game.
 * @param[in] gameName Name for the game.
 * @param[in,out] pMap Initial map of the game.
 * @param[in] pDefHealth Default health for players.
 * @param[in] pAi Default AI for players.
 * @param[in] mDefHealth Default health for minotaurs.
 * @param[in] mAi Default AI for minotaurs.
 * @return true If the game is correctly initialised.
 * @return false If there is some mistakes (no exit, no player ...).
 */
bool game_init(game_t* pGame,
               config_t* pConf,
               const char* gameName,
               map_t* pMap,
               int pDefHealth,
               ai_t pAi,
               int mDefHealth,
               ai_t mAi);

/**
 * @brief Run an initialised game.
 *
 * @param[in, out] pGame The game to start.
 */
void game_start(game_t* pGame);

/**
 * @brief Clear all allocated content of a game.
 *
 * @param[in, out] pGame The game to clear.
 */
void game_delete(game_t* pGame);

#endif  // End GAME_H
