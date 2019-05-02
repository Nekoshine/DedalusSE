/**
 * @file display.h
 * @author Chevelu Jonathan (jonathan.chevelu@irisa.fr)
 * @brief Methods to display a dedalus.
 * @version 0.1
 * @date 2019-02-19
 *
 * @copyright Copyright (c) 2019
 *
 */

#ifndef DISPLAY_H
#define DISPLAY_H

#include <stdio.h>

#include "character.h"
#include "config.h"
#include "map.h"

/**
 * @brief Set of possible endings for players and game master.
 *
 */
typedef enum ending {
  EG_LOOSE,                   ///< Player completely failed.
  EG_NEXT_LEVEL_AND_ALIVE,    ///< The player found the exit.
  EG_NEXT_LEVEL_BUT_DEAD,     ///< An other player found the exit.
  EG_WIN_AND_ALIVE,           ///< Minotaur killed and player exits.
  EG_KILL_MINOTAUR_BUT_DEAD,  ///< Minotaur dead but player dead.
  EG_WIN_BUT_DEAD,            ///< Minotaur dead and some other players exits.
  EG_GM_WIN,                  ///< Game master won (all players are dead).
  EG_GM_LOOSE                 ///< Game master failed (some players escaped).
} ending_t;

/**
 * @brief Display the game master interface.
 *
 * @param[in,out] stream Where it should be printed.
 * @param[in] level Name of the level.
 * @param[in] pMap Map to print.
 * @param[in] nbPlayerAlive Number of player still alive.
 * @param[in] nbPlayerOnBoard Number of player on board.
 * @param[in] nbMinotaurAlive Number of Minotaurs still alive.
 * @param[in] delay Delay between two frames (for information only).
 * @param[in] gameInfo Display more precise game informations.
 * @param[in] refresh If only the body of the ui should be refresh.
 */
void display_ui_gm(FILE* stream,
                   const char* level,
                   const map_t* pMap,
                   size_t nbPlayerAlive,
                   size_t nbPlayerOnBoard,
                   size_t nbMinotaurAlive,
                   int delay,
                   bool gameInfo,
                   bool refresh);

/**
 * @brief Display a player interface.
 *
 * @param[in] level Level name.
 * @param[in] ia IA name.
 * @param[in] pMap Map to print.
 * @param[in] pPlayer Player considered for the point of view.
 * @param[in] delay Delay between two frames (for information only).
 * @param[in] maxMoves Max number of moved allowed.
 * @param[in] gameInfo Display more precise game informations.
 * @param[in] header If the header of the ui should be printed.
 * @param[in] refresh If the body of the ui should be refresh.
 *
 * @note The output stream is defined inside the player.
 */
void display_ui_player(const char* level,
                       const char* ia,
                       const map_t* pMap,
                       const character_t* pPlayer,
                       int delay,
                       int maxMoves,
                       bool gameInfo,
                       bool header,
                       bool refresh);

/**
 * @brief Display an ending message.
 *
 * @param[in,out] stream Where it should be printed.
 * @param[in] ending The ending type to display.
 */
void display_ending(FILE* stream, ending_t ending);

/**
 * @brief Display on iteration of a fight.
 *
 * @param[in] pMap The map to clear for the first iteration.
 * @param[in] pC1 First character for the fight.
 * @param[in] pC2 Second character for the fight.
 * @param[in] delay Delay between two frame of the introduction step.
 * @param[in] iteration Step of the fight.
 *
 * @note The iteration is used of the animation. Basically it is a counter that
 * should be incremented by one between each frame.
 */
void display_fight_iteration(const map_t* pMap,
                             const character_t* pC1,
                             const character_t* pC2,
                             size_t delay,
                             size_t iteration);

/**
 * @brief Clear the display of a finished fight.
 *
 * @param[in,out] stream The stream to clean
 */
void display_fight_clear(FILE* stream);

/**
 * @brief Display a fatal error.
 *
 * @param[in,out] stream Where the message should be printed.
 * @param[in] msg Message to print.
 * @note For debug only.
 */
void display_fatal_error(FILE* stream, const char* msg);

/**
 * @brief Display an information message.
 *
 * @param[in,out] stream Where the message should be printed.
 * @param[in] msg The message to display.
 */
void display_info(FILE* stream, const char* msg);

/**
 * @brief Wait a user interaction.
 *
 * @param[in,out] stream The stream from where the interaction should happen.
 * @param[in] preMsg A one line message that could be printed to ask for
 * interaction. It is removed after the interaction. NULL to display nothin.
 * @param[in] waitUser If this call really wait an interaction or just one
 * second.
 */
void display_wait_user(FILE* stream, const char* preMsg, bool waitUser);

#endif  // End of DISPLAY_H
