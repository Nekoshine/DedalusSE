/**
 * @file character.c
 * @author Chevelu Jonathan (jonathan.chevelu@irisa.fr)
 * @brief Character definition and management.
 * @version 0.1
 * @date 2019-02-20
 *
 * @copyright Copyright (c) 2019
 *
 */

#include <limits.h>  //INT_MAX
#include <unistd.h>  //usleep

#include "ariadneString.h"
#include "character.h"
#include "display.h"
#include "gps.h"

/**********************************/
// Declaration of local functions.

/**
 * @brief Is it possible to go in a given direction?
 *
 * @param[in] map The considered map.
 * @param[in] pos The stating position.
 * @param[in] c The considered direction.
 * @return true Yes you can.
 * @return false No you can not.
 */
bool _character_can_go(map_t map, pos_t pos, compass_t c);

/**
 * @brief Is a move provided by an AI valid, considering the valid directions?
 *
 * @param[in] c The selected direction.
 * @param[in] n Is it possible to go north?
 * @param[in] e Is it possible to go east?
 * @param[in] s Is it possible to go south?
 * @param[in] w Is it possible to go west?
 * @return true It is valide move.
 * @return false The AI try to cheat.
 */
bool _character_is_valid_move(compass_t c, bool n, bool e, bool s, bool w);

/**
 * @brief Move a character
 *
 * @param[in,out] pMap The considered map.
 * @param[in,out] pC The considered character.
 * @param[in] c The wanted direction for the move.
 * @param[out] pExited Does the character reach an exit?
 */
void _character_make_move(map_t* pMap,
                          character_t* pC,
                          compass_t c,
                          bool* pExited);

/**
 * @brief Get the pid of the child of a given pid.
 *
 * @param[in] pid The parent pid.
 * @return pid_t The child pid.
 * @note Usefull if you only have the pid of a Xterm and you want the pid of the
 * bash.
 */
pid_t _character_get_child_pid(pid_t pid);

/**********************************/
//  Local functions implementation

bool _character_can_go(map_t map, pos_t pos, compass_t c) {
  size_t x = pos.x;
  size_t y = pos.y;

  char e = WALL;

  switch (c) {
    case North:
      if (pos.y > 0) {
        e = map.m[y - 1][x];
      }
      break;
    case East:
      if (x < map.x - 1) {
        e = map.m[y][x + 1];
      }
      break;
    case South:
      if (y < map.y - 1) {
        e = map.m[y + 1][x];
      }
      break;
    case West:
      if (x > 0) {
        e = map.m[y][x - 1];
      }
      break;
    default:
      break;
  }
  return (e == PATH || e == EXIT || e == DEAD);
}

bool _character_is_valid_move(compass_t c, bool n, bool e, bool s, bool w) {
  return ((c == North && n) || (c == East && e) || (c == South && s) ||
          (c == West && w) || (c == Stay));
}

void _character_make_move(map_t* pMap,
                          character_t* pC,
                          compass_t c,
                          bool* pExited) {
  // Restore tile
  if ((pC->type == PLAYER) || (pC->type == MINOTAUR)) {
    pMap->m[pC->pos.y][pC->pos.x] = (char)pC->walkOn;
  }

  // Move
  pC->pos = gps_compute_move(pC->pos, c);
  // Check exit
  if (pMap->m[pC->pos.y][pC->pos.x] == EXIT) {
    *pExited = true;
  }

  // Save target tile
  pC->walkOn = (map_content_t)pMap->m[pC->pos.y][pC->pos.x];
  //  Set character
  // Deads are below
  if ((pC->type != DEAD) ||
      ((pC->walkOn != PLAYER) && (pC->walkOn != MINOTAUR))) {
    pMap->m[pC->pos.y][pC->pos.x] = (char)pC->type;
  }

  if (pC->pMask != NULL) {
    map_mask_add(pC->pMask, pC->pos);
  }
}

pid_t _character_get_child_pid(pid_t pid) {
  const size_t nbTry = 50;
  bool found = false;
  size_t it = 0;

  pid_t res = 0;

  while ((!found) && (it < nbTry)) {
    // Build de command.
    const size_t maxCmd = 1024;
    char cmd[maxCmd];
    int nbW =
        snprintf(cmd, maxCmd, "ps -o pid --ppid %d | grep -o [0-9]*", pid);

    cmd[maxCmd - 1] = '\0';
    if ((size_t) nbW >= maxCmd) {
      display_fatal_error(
          stderr, "Error in _character_get_child_pid: command to long.\n");
      exit(EXIT_FAILURE);
    }

    // Open the command for reading.
    FILE* fp = popen(cmd, "r");
    if (fp == NULL) {
      display_fatal_error(stderr, "Error: Failed to run command.\n");
      exit(EXIT_FAILURE);
    }

    // Read the output a line at a time - output it.
    const int maxBuffer = 1024;
    char resBuffer[maxBuffer];
    resBuffer[0] = '\0';
    if (fgets(resBuffer, maxBuffer - 1, fp) != NULL) {
      long int tmp = strtol(resBuffer, NULL, 0);
      if (tmp > INT_MAX) {
        display_fatal_error(stderr, "Something strange");
        exit(EXIT_FAILURE);
      }
      res = (int)tmp;
      if (res != 0) {
        found = true;
      }
    }

    /* close */
    if (pclose(fp) != 0) {
      display_fatal_error(stderr, " Error: Failed to close command stream.\n");
      exit(EXIT_FAILURE);
    }

    if (!found) {
      usleep(1000);  // wait 1ms
    }
    ++it;
  }

  if (res == 0) {
    display_fatal_error(stderr, " Error: Pid not found.\n");
    exit(EXIT_FAILURE);
  }

  return res;
}

/**********************************/
//  Public functions implementation

character_t character_init(map_content_t type,
                           pid_t id,
                           bool dispInChild,
                           const char* name,
                           int health,
                           ai_t ai) {
  character_t c;
  c.type = type;
  c.name = name;
  c.id = id;
  c.health = health;
  c.pos.x = 0;
  c.pos.y = 0;

  c.targetCompass = Stay;
  c.targetDistance = 0;

  c.ariadne = NULL;
  c.ai = ai;
  c.walkOn = PATH;

  // need a ps -o pid --ppid 2484 | grep -o [0-9]* in a popen
  if (id > 0) {
    pid_t p = id;
    if (dispInChild) {
      // Get a child pid
      p = _character_get_child_pid(id);
    }

    const size_t maxPid = 50;
    char buffer[maxPid];
    int nbW = snprintf(buffer, maxPid, "/proc/%d/fd/1", p);
    buffer[maxPid - 1] = '\0';
    if (nbW < 0 || (size_t)nbW >= maxPid) {
      display_fatal_error(stderr, "Error in character_init: pid to long!\n");
      exit(EXIT_FAILURE);
    }

    c.stream = fopen(buffer, "a+");
    if (c.stream == NULL) {
      display_fatal_error(stderr,
                          "Error: Unable to open display for player.\n");
      exit(EXIT_FAILURE);
    }
  } else {
    c.stream = fopen("/dev/null", "a+");
    if (c.stream == NULL) {
      display_fatal_error(stderr, "Error: Unable to open /dev/null (?).\n");
      exit(EXIT_FAILURE);
    }
  }

  c.ending = EC_NO_ENDING;

  c.pMask = NULL;
  return c;
}

compass_t character_propose_move(const character_t* pC,
                                 const map_t* pMap,
                                 bool* pCheated) {
  bool goNorth = _character_can_go(*pMap, pC->pos, North);
  bool goEast = _character_can_go(*pMap, pC->pos, East);
  bool goSouth = _character_can_go(*pMap, pC->pos, South);
  bool goWest = _character_can_go(*pMap, pC->pos, West);

  compass_t move =
      (*(pC->ai.policy))(goNorth, goEast, goSouth, goWest, pC->targetCompass,
                         pC->targetDistance, pC->ariadne);
  *pCheated = !_character_is_valid_move(move, goNorth, goEast, goSouth, goWest);
  return move;
}

void character_play(character_t* pC,
                    compass_t move,
                    pos_t targetPos,
                    map_t* pMap,
                    int steps,
                    int maxMoves,
                    bool* pExited) {
  (void)steps;  // Unused;

  _character_make_move(pMap, pC, move, pExited);

  if (string_can_go_back(pC->ariadne)) {
    string_remove_link(&(pC->ariadne));
    string_remove_link(&(pC->ariadne));
  }
  string_add_link(&(pC->ariadne), move);

  // Update info
  gps_direction(pC->pos, targetPos, &(pC->targetCompass),
                &(pC->targetDistance));

  // Update Health
  switch (pC->type) {
    case PLAYER:
      pC->health -= 100.0 / maxMoves;
      if (pC->health < 0) {
        pC->health = 0;
      };
      break;
    default:
      break;
  }
}

void character_is_dead(map_t* pMap, character_t* pC) {
  pC->type = DEAD;
  pC->ai = ai_new("dead");
  pC->health = 0;
  pMap->m[pC->pos.y][pC->pos.x] = DEAD;
}

void character_is_out(map_t* pMap, character_t* pC) {
  pC->type = EXIT;
  pC->ai = ai_new("out");
  pMap->m[pC->pos.y][pC->pos.x] = EXIT;
}

void character_delete(map_t* pMap, character_t* pC) {
  pC->name = NULL;
  pC->id = -1;
  pC->health = -1;

  pMap->m[pC->pos.y][pC->pos.x] = (char)pC->walkOn;

  string_delete(&(pC->ariadne));

  map_delete(pC->pMask);
  free(pC->pMask);

  fclose(pC->stream);
}
