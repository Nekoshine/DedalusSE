/**
 * @file character.h
 * @author Chevelu Jonathan (jonathan.chevelu@irisa.fr)
 * @brief Character definition and management.
 * @version 0.1
 * @date 2019-02-20
 *
 * @copyright Copyright (c) 2019
 *
 */
#ifndef CHARACTER_H
#define CHARACTER_H

#include <stdio.h>
#include <sys/types.h>

#include "ai.h"
#include "ariadneString.h"
#include "gps.h"
#include "map.h"

/**
 * @brief Possible endings of a character.
 *
 */
typedef enum ending_char {
  EC_NO_ENDING,           ///< It is not finish.
  EC_KILL_BY_M,           ///< Character was killed by a Minotaur.
  EC_STARVE_MINOTAUR,     ///< Character starve and is kill by a Minotaur.
  EC_STARVE_NO_MINOTAUR,  ///< Character just starve.
  EC_CHEAT_MINOTAUR,      ///< Character cheat and then killed by a Minotaur.
  EC_CHEAT_NO_MINOTAUR,   ///< Character just cheat.
  EC_ESCAPE,              ///< Character escaped.
  EC_WIN                  ///< Character escaped after Minotaurs death.
} ending_char_t;

/**
 * @brief A character (player, Minotaur, dead, ...)
 *
 */
typedef struct character {
  const char* name;    ///< Name of the character.
  map_content_t type;  ///< Type of character (player, Minotaur, dead, ...).
  pid_t id;            ///< Character identifier.
  double health;       ///< Health of the character.
  pos_t pos;           ///< Current position of the character.
  compass_t targetCompass;  ///< Direction of the character target.
  float targetDistance;     ///< Distance to the character target.
  string ariadne;           ///< Ariadne string for the character.
  map_content_t walkOn;     ///< What is below the character (exit, dead, ...)
  ai_t ai;                  ///< AI for the character
  FILE* stream;             ///< The stream for the character display.
  ending_char_t ending;     ///< What is the ending for the character.
  map_t* pMask;             ///< Mask of what is seen by the character.
} character_t;

/**
 * @brief Initialize a character.
 *
 * @param[in] type Type of character (player, Minotaur, ...).
 * @param[in] id The id of the character.
 * @param[in] dispInChild Display in the child of the PID process id.
 * @param[in] name Name of the character.
 * @param[in] health Initial health of the character.
 * @param[in] ai AI of the character.
 * @return character_t An initialized character (ready to play).
 */
character_t character_init(map_content_t type,
                           pid_t id,
                           bool dispInChild,
                           const char* name,
                           int health,
                           ai_t ai);

/**
 * @brief Ask a character the move she wants to play.
 *
 * @param[in] pC The considered character.
 * @param[in] pMap The map.
 * @param[out] pCheated Says if the AI tries to cheat?
 * @return compass_t The desired move.
 */
compass_t character_propose_move(const character_t* pC,
                                 const map_t* pMap,
                                 bool* pCheated);

/**
 * @brief Play a move for a character.
 *
 * @param[in,out] pC The considered character.
 * @param[in] move The desired move.
 * @param[in] targetPos Position of the character target.
 * @param[in,out] pMap The map.
 * @param[in] steps Number of steps from the begining.
 * @param[in] maxMoves Maximum number of steps before death.
 * @param[out] pExited Says if the charactere is on an exit after the move.
 */
void character_play(character_t* pC,
                    compass_t move,
                    pos_t targetPos,
                    map_t* pMap,
                    int steps,
                    int maxMoves,
                    bool* pExited);

/**
 * @brief Set a character as dead.
 *
 * @param[in,out] pMap The map.
 * @param[in,out] pC The dying character.
 *
 * He will not move and change its type.
 */
void character_is_dead(map_t* pMap, character_t* pC);

/**
 * @brief A Charater exit the maze.
 *
 * @param[in,out] pMap The map.
 * @param[in,out] pC The character that exit.
 *
 * He will not move and his position will change.
 */
void character_is_out(map_t* pMap, character_t* pC);

/**
 * @brief Clear a character structure.
 *
 * @param[in,out] pMap The map.
 * @param[in,out] pC The character to clear.
 */
void character_delete(map_t* pMap, character_t* pC);

#endif
