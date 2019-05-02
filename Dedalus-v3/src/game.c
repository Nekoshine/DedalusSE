
/**
* @file game.c
* @author Chevelu Jonathan (jonathan.chevelu@irisa.fr)
* @brief Dedalus game controller.
* @version 0.1
* @date 2019-01-25
*
* @copyright Copyright (c) 2019
*
*/

#include <stdio.h>   // printf
#include <string.h>  // strcpy, strlen
#include <unistd.h>  // usleep

#include "display.h"  // outputs of the game
#include "game.h"     // public defintions
#include "gps.h"      // pos_t
#include <sys/types.h>
#include <sys/wait.h>


/**********************************/
// Declaration of local functions.

/**
* @brief Structure to store moves propositions.
*
*/
typedef struct moves_prop {
  character_t* c;  ///< Pointer to the character concerned.
  compass_t move;  ///< The move proposition
  bool cheated;    ///< Is this a cheat move.
} moves_prop_t;

/**
* @brief Initialize a moves propositions array.
*
* @param[in] pGame The game considered.
* @param[out] pMoves Allocated array of moves propositions for each character.
* @param[out] pNbChar Size of the array.
* @warning The returned array must be free.
*/
void _game_init_moves_prop(const game_t* pGame,
                           moves_prop_t** pMoves,
                           size_t* pNbChar);

/**
* @brief Get all moves propositions for each character.
*
* @param[in] pGame The game considered.
* @param[in,out] pMoves An initialized vector of propositions.
* @param[in] nbChar Number of character considered.
* @note The array pMoves must be initialized before the call.
*/
void _game_get_moves_propositions(game_t* pGame,
                                  moves_prop_t* pMoves,
                                  size_t nbChar);

/**
* @brief Solve conflicts between an array of move propositions.
*
* @param[in] pGame The game considered.
* @param[in,out] moves The array of moves propositions to clear.
* @param[in] nbChar Number of character considered.
*
* The "moves" arrays will be cleaned to remove conflicts.
* A conflict occurs when two characters want to go at the same position.
* In this version only one character will move (players firsts and in order
* of declaration after).
*/
void _gave_solve_moves_conflicts(const game_t* pGame,
                                 moves_prop_t* moves,
                                 size_t nbChar);

/**
* @brief Set a character as dead for this game.
*
* @param[in,out] pGame The game to change.
* @param[in] pC The character to define has dead.
*/
void _game_death_caractere(game_t* pGame, character_t* pC);

/**
* @brief A player exits the dedalus.
*
* @param[in, out] pGame The game to change.
* @param[in, out] pC The player that exits.
* @note The character must be a player. Otherwise it exits with a fatal error.
*/
void _game_exit_character(game_t* pGame, character_t* pC);

/**
* @brief Says if two character should fight eachother.
*
* @param[in] pC1 A character.
* @param[in] pC2 A character.
* @return true A fight should take place.
* @return false No fight possible.
* @note A fight is possible if the characters are not dead, not of the same
* type (one player vs one minotaur) and if they are at the same or an adjacent
* position.
*/
bool _game_should_fight(const character_t* pC1, const character_t* pC2);

/**
* @brief Run and display a fight between two characters.
*
* @param[in, out] pGame The game to change.
* @param[in, out] pC1 First opponent.
* @param[in, out] pC2 Second opponent.
* @note This function never checks if the two characters should fight.
*/
void _game_fight(game_t* pGame, character_t* pC1, character_t* pC2);

/**
* @brief Check and run all fights for a given character.
*
* @param[in, out] pGame The game to change.
* @param[in, out] pC The concerned character.
*/
void _game_fight_manager_char(game_t* pGame, character_t* pC);

/**
* @brief Check and run all fights for all characters.
*
* @param[in, out] pGame The game to change.
*/
void _game_fight_manager(game_t* pGame);

/**
* @brief Define and set the target for a given character according to the board
* configuration.
*
* @param[in, out] pGame The game to change.
* @param[in] pC The charactere to find a target.
* @return pos_t The position of the target for this character.
* @note The target is the closest opponent if any. Else it is the closest exit
* for a player.
*/
pos_t _game_character_target(game_t* pGame, const character_t* pC);

/**
* @brief Play a character move
*
* @param[in, out] pGame The game to change.
* @param[in,out] pC The character to play.
* @param[in] move The move to play.
* @param[in] cheated The AI tries to cheat when move proposition was asked.
* @param[in] cheated Says if the AI tries to cheat.
*/
void _game_play_character(game_t* pGame,
                          character_t* pC,
                          compass_t move,
                          bool cheated);

/**
* @brief Play all characters according to a set of moves.
*
* @param[in,out] pGame The game to change.
* @param[in] moves Array of moves to play.
* @param[in] nbChar Number of characters considered.
*/
void _game_play_characters(game_t* pGame,
                           const moves_prop_t* moves,
                           size_t nbChar);

/**
* @brief Refresh display for everybody.
*
* @param[in] pGame The game to display.
*/
void _game_play_refresh_ui(const game_t* pGame);

/**
* @brief Compute the end game status (win or loose) for the game master.
*
* @param[in] pGame The game considered.
* @return ending_t An ending for the GM.
*/
ending_t _game_ending_gm(const game_t* pGame);

/**
* @brief Compute the end game status (win or loose) for a player.
*
* @param[in] pGame The game considered.
* @param[in] pC The player considered.
* @return ending_t An ending for the player.
*
* Returns the global ending i.e. how successful was the player team
* considering the player personal result.
*/
ending_t _game_ending_player(const game_t* pGame, const character_t* pC);

/*****************************/
// Functions implementation.

void _game_init_moves_prop(const game_t* pGame,
                           moves_prop_t** pMoves,
                           size_t* pNbChar) {
  *pNbChar = pGame->nbPlayer + pGame->nbMinotaur;
  *pMoves = (moves_prop_t*)malloc((*pNbChar) * sizeof(moves_prop_t));
  if (*pMoves == NULL) {
    display_fatal_error(stderr, "Error: malloc failed!");
    exit(EXIT_FAILURE);
  }
  
  // Init
  
  for (size_t i = 0; i < pGame->nbPlayer; ++i) {
    (*pMoves)[i].c = &(pGame->playerA[i]);
    (*pMoves)[i].cheated = false;
    (*pMoves)[i].move = Stay;
  }
  size_t offset = pGame->nbPlayer;
  for (size_t i = 0; i < pGame->nbMinotaur; ++i) {
    (*pMoves)[i + offset].c = &(pGame->minotaurA[i]);
    (*pMoves)[i + offset].cheated = false;
    (*pMoves)[i + offset].move = Stay;
  }
}

void _game_death_caractere(game_t* pGame, character_t* pC) {
  switch (pC->type) {
  case PLAYER:
    pGame->nbPlayerAlive--;
    pGame->nbPlayerOnBoard--;
    break;
  case MINOTAUR:
    pGame->nbMinotaurAlive--;
    break;
  default:
    display_fatal_error(DISPLAY, "Try to kill something strange\n");
  exit(EXIT_FAILURE);
  }
  character_is_dead(pGame->pMap, pC);
}

void _game_exit_character(game_t* pGame, character_t* pC) {
  switch (pC->type) {
  case PLAYER:
    pGame->nbPlayerOnBoard--;
    break;
  default:
    display_fatal_error(DISPLAY, "Try to exit something strange\n");
  exit(EXIT_FAILURE);
  }
  character_is_out(pGame->pMap, pC);
}

bool _game_should_fight(const character_t* pC1, const character_t* pC2) {
  // not dead and same position or next to eachother
  return ((pC1->type != DEAD) && (pC2->type != DEAD) &&
          (((pC1->pos.x + 1 == pC2->pos.x) && (pC1->pos.y == pC2->pos.y)) ||
          ((pC1->pos.x - 1 == pC2->pos.x) && (pC1->pos.y == pC2->pos.y)) ||
          ((pC1->pos.x == pC2->pos.x) && (pC1->pos.y + 1 == pC2->pos.y)) ||
          ((pC1->pos.x == pC2->pos.x) && (pC1->pos.y - 1 == pC2->pos.y)) ||
          ((pC1->pos.x == pC2->pos.x) && (pC1->pos.y == pC2->pos.y))));
}

void _game_fight(game_t* pGame, character_t* pC1, character_t* pC2) {
  size_t i = 0;
  while ((pC1->health > 0) && (pC2->health > 0)) {
    display_fight_iteration(pGame->pMap, pC1, pC2, (size_t)pGame->delay, i);
    usleep((unsigned int)pGame->delay);
    ++i;
    --(pC1->health);
    --(pC2->health);
  }
  display_fight_iteration(pGame->pMap, pC1, pC2, (size_t)pGame->delay, i);
  
  if (pC1->health <= 0) {
    _game_death_caractere(pGame, pC1);
  }
  
  if (pC2->health <= 0) {
    _game_death_caractere(pGame, pC2);
  }
  display_wait_user(DISPLAY, "Press any key to continue...",
                    pGame->interactive);
  
  // refresh display
  display_fight_clear(pC1->stream);
  display_ui_player(pGame->gameName, pC1->ai.name, pGame->pMap, pC1,
                    pGame->delay, pGame->maxMoves, pGame->gameInfo, false,
                    false);
  display_fight_clear(pC2->stream);
  display_ui_player(pGame->gameName, pC2->ai.name, pGame->pMap, pC2,
                    pGame->delay, pGame->maxMoves, pGame->gameInfo, false,
                    false);
  
  _game_play_refresh_ui(pGame);
}

void _game_fight_manager_char(game_t* pGame, character_t* pC) {
  character_t* pOpponents = NULL;
  size_t nbOpponents = 0;
  switch (pC->type) {
  case PLAYER:
    pOpponents = pGame->minotaurA;
    nbOpponents = pGame->nbMinotaur;
    break;
  case MINOTAUR:
    pOpponents = pGame->playerA;
    nbOpponents = pGame->nbPlayer;
    break;
  default:
    break;
  // No opponent
  }
  for (size_t i = 0; i < nbOpponents; ++i) {
    if (_game_should_fight(pC, &(pOpponents[i]))) {
      _game_fight(pGame, pC, &(pOpponents[i]));
    }
  }
}

void _game_fight_manager(game_t* pGame) {
  // Players fight
  for (size_t i = 0; i < pGame->nbPlayer; ++i) {
    _game_fight_manager_char(pGame, &(pGame->playerA[i]));
  }
  
  // Minotaur fight (notice : should be useless)
  for (size_t i = 0; i < pGame->nbMinotaur; ++i) {
    _game_fight_manager_char(pGame, &(pGame->minotaurA[i]));
  }
}

pos_t _game_character_target(game_t* pGame, const character_t* pC) {
  pos_t target;
  
  switch (pC->type) {
  case PLAYER:
    if ((pGame->finalLevel) && (pGame->nbMinotaurAlive > 0)) {
      target =
        gps_closest(pC->pos, pGame->minotaurA, pGame->nbMinotaur, MINOTAUR);
    } else {
      target = gps_closest(pC->pos, pGame->exitA,pGame->nbExit, EXIT);
    }
    break;
  case MINOTAUR:
    target = gps_closest(pC->pos, pGame->playerA, pGame->nbPlayer, PLAYER);
    break;
  default:
    // No target
    target = pC->pos;
  }
  return target;
}

/*  Version avec Waitpid avec gain de vitesse BOI */

void _game_get_moves_propositions(game_t* pGame,
                                  moves_prop_t* pMoves,
                                  size_t nbChar) {
  pid_t pid[nbChar];
  //printf("nbChar : %d\n",(int)nbChar);
  compass_t move;
  bool cheated;
  int status;
  for (size_t i=0;i < nbChar;i++) {
    character_t* pC = pMoves[i].c;
    
    pid[i]=fork();
    if(pid[i]==-1){
      printf("ggmp failed \n");
      exit(EXIT_FAILURE);
    }
    if(pid[i]==0){
      //  printf("i : %d\n",(int)i); 
      move = character_propose_move(pC, pGame->pMap, &cheated);
      move=(int)move+cheated*10;
      exit(move);
    }
  }
  for(size_t i=0;i < nbChar;i++){
    //printf("i : %d\n",(int)i); 
    waitpid(pid[i],&status,0);
    if(WIFEXITED(status)){
      
      status=WEXITSTATUS(status);
      //printf("status : %d\n",status);
      pMoves[i].move=status%10;
      pMoves[i].cheated=status/10;
      //printf("move : %d cheated : %d\n",pMoves[i].move,pMoves[i].cheated);
    }
  }
}


/* Version avec Wait sans gain de vitesse  */
/*
void _game_get_moves_propositions(game_t* pGame,
                                  moves_prop_t* pMoves,
size_t nbChar) {
pid_t pid[nbChar];
//printf("nbChar : %d\n",(int)nbChar);
compass_t move;
bool cheated;
int status;
for (size_t i=0;i < nbChar;i++) {
character_t* pC = pMoves[i].c;

pid[i]=fork();
if(pid[i]==-1){
printf("ggmp failed \n");
exit(EXIT_FAILURE);
}
if(pid[i]==0){
//  printf("i : %d\n",(int)i); 
move = character_propose_move(pC, pGame->pMap, &cheated);
move=(int)move+cheated*10;
exit(move);
}
}
for(size_t i=0;i < nbChar;i++){
//printf("i : %d\n",(int)i); 
wait(&status);
if(WIFEXITED(status)){
status=WEXITSTATUS(status);
//printf("status : %d\n",status);
pMoves[i].move=status%10;
pMoves[i].cheated=status/10;
//printf("move : %d cheated : %d\n",pMoves[i].move,pMoves[i].cheated);
}
}
}
*/

void _gave_solve_moves_conflicts(const game_t* pGame,
                                 moves_prop_t* moves,
                                 size_t nbChar) {
  (void)pGame;  // unused
  
  // Solve all conflicts
  bool conflictFound = false;
  do {
    pos_t* usedPositions = NULL;
    size_t nbPosUsed = 0;
    // Fix position of staying char
    for (size_t i = 0; i < nbChar; ++i) {
      character_t* pC = moves[i].c;
      // Only alive characters are interested
      if ((moves[i].move == Stay) &&
          ((pC->type == PLAYER) || (pC->type == MINOTAUR))) {
        // reserve position. We are sure there is no conflict.
        ++nbPosUsed;
        usedPositions =
        (pos_t*)realloc(usedPositions, nbPosUsed * sizeof(pos_t));
        if (usedPositions == NULL) {
          display_fatal_error(stderr, "Error: can not realloc usedPosition!");
          exit(EXIT_FAILURE);
        }
        usedPositions[nbPosUsed - 1] = pC->pos;
      }
    }
    
    // Search conflict for others but stop at first conflict
    // Just one conflict at a time in order to be sure that refusing a move for
    // a character will not create another conflict.
    conflictFound = false;
    size_t i = 0;
    while ((!conflictFound) && (i < nbChar)) {
      character_t* pC = moves[i].c;
      // Only alive characters are interested
      // We only consider moving characters since those staying are already
      // managed.
      if ((moves[i].move != Stay) &&
          ((pC->type == PLAYER) || (pC->type == MINOTAUR))) {
        // Compute target position
        pos_t nextPos = gps_compute_move(pC->pos, moves[i].move);
        
        // Check if we have a conflict
        size_t j = 0;
        while ((!conflictFound) && (j < nbPosUsed)) {
          conflictFound = ((usedPositions[j].x == nextPos.x) &&
            (usedPositions[j].y == nextPos.y));
          ++j;
        }
        ++nbPosUsed;
        usedPositions =
        (pos_t*)realloc(usedPositions, nbPosUsed * sizeof(pos_t));
        if (usedPositions == NULL) {
          display_fatal_error(stderr, "Error: can not realloc usedPosition!");
          exit(EXIT_FAILURE);
        }
        if (conflictFound) {
          moves[i].move = Stay;
          usedPositions[nbPosUsed - 1] = pC->pos;
        } else {
          // Reserve the position
          usedPositions[nbPosUsed - 1] = nextPos;
        }
      }
      ++i;
    }
    
    free(usedPositions);
  } while (conflictFound);
}

void _game_play_character(game_t* pGame,
                          character_t* pC,
                          compass_t move,
                          bool cheated) {
  bool exited = false;
  
  if (cheated) {
    // Deal with cheater
    _game_death_caractere(pGame, pC);
    if (pGame->nbMinotaurAlive > 0) {
      pC->ending = EC_CHEAT_MINOTAUR;
    } else {
      pC->ending = EC_CHEAT_NO_MINOTAUR;
    }
  } else {
    // Move character
    pos_t target = _game_character_target(pGame, pC);
    character_play(pC, move, target, pGame->pMap, pGame->steps, pGame->maxMoves,
                   &exited);
    
    if ((pC->type != DEAD) && (pC->health <= 0)) {
      // Deal with exhaustion
      _game_death_caractere(pGame, pC);
      if (pGame->nbMinotaurAlive > 0) {
        pC->ending = EC_STARVE_MINOTAUR;
      } else {
        pC->ending = EC_STARVE_NO_MINOTAUR;
      }
    }
  }
  
  // Deal with exit
  if (exited && (pC->type == PLAYER) && (pGame->nbMinotaurAlive <= 0)) {
    // NOTE: if you kill the last Minotaur on an exit tile, you have to move
    // twice to exit.
    
    // NOTE: you can't exit if you are dead.
    _game_exit_character(pGame, pC);
    if (pGame->finalLevel) {
      pC->ending = EC_WIN;
    } else {
      pC->ending = EC_ESCAPE;
    }
  }
}

void _game_play_characters(game_t* pGame,
                           const moves_prop_t* moves,
                           size_t nbChar) {
  // Play all characters
  for (size_t i = 0; i < nbChar; ++i) {
    _game_play_character(pGame, moves[i].c, moves[i].move, moves[i].cheated);
  }
}

void _game_play_refresh_ui(const game_t* pGame) {
  // Play all characters
  
  // Display current state for game master
  display_ui_gm(DISPLAY, pGame->gameName, pGame->pMap, pGame->nbPlayerAlive,
                pGame->nbPlayerOnBoard, pGame->nbMinotaurAlive, pGame->delay,
                pGame->gameInfo, true);
  for (size_t i = 0; i < pGame->nbPlayer; ++i) {
    const character_t* pC = &(pGame->playerA[i]);
    // Display for current player
    display_ui_player(pGame->gameName, pC->ai.name, pGame->pMap, pC,
                      pGame->delay, pGame->maxMoves, pGame->gameInfo, false,
                      true);
  }
}

ending_t _game_ending_gm(const game_t* pGame) {
  if (pGame->nbPlayerAlive > 0) {
    return EG_GM_LOOSE;
  }
  return EG_GM_WIN;
}

ending_t _game_ending_player(const game_t* pGame, const character_t* pC) {
  ending_t ending = EG_LOOSE;
  if (pC->type == EXIT) {
    if (pGame->finalLevel) {
      ending = EG_WIN_AND_ALIVE;
    } else {
      ending = EG_NEXT_LEVEL_AND_ALIVE;
    }
  } else if (pGame->nbPlayerAlive > 0) {
    if (!pGame->finalLevel) {
      ending = EG_NEXT_LEVEL_BUT_DEAD;
    } else if (pGame->nbMinotaurAlive <= 0) {
      ending = EG_WIN_BUT_DEAD;
    }  // else {} //  LOOSE
  } else if ((pGame->nbMinotaurAlive <= 0) && (pGame->finalLevel)) {
    ending = EG_KILL_MINOTAUR_BUT_DEAD;
  }
  return ending;
}

/**********************************/
// Public functions implementations.

bool game_init(game_t* pGame,
               config_t* pConf,
               const char* gameName,
               map_t* pMap,
               int pDefHealth,
               ai_t pAi,
               int mDefHealth,
               ai_t mAi) {
  pGame->gameName = gameName;
  pGame->pMap = pMap;
  pGame->maxMoves = pConf->maxMoves;
  pGame->gameInfo = pConf->gameInfo;
  pGame->steps = 0;
  pGame->delay = pConf->delay;
  pGame->interactive = pConf->interactive;
  
  bool ok = true;
  
  // Load exit(s)
  pGame->nbExit = gps_locator(pMap, EXIT, &(pGame->exitA));
  bool noExit = (pGame->nbExit == 0);
  
  // Load a Minotaur(s)
  pos_t* mAPos = NULL;
  pGame->nbMinotaur = gps_locator(pMap, MINOTAUR, &mAPos);
  pGame->nbMinotaurAlive = pGame->nbMinotaur;
  pGame->finalLevel = (pGame->nbMinotaur > 0);
  if ((pGame->nbExit == 0) && (!pGame->finalLevel)) {
    // if no exit, then it must be a final level
    ok = false;
  }
  // NOTE: malloc(0) return NULL so it is ok
  pGame->minotaurA =
    (character_t*)malloc(pGame->nbMinotaur * sizeof(character_t));
  for (size_t i = 0; i < pGame->nbMinotaur; ++i) {
    pGame->minotaurA[i] = character_init(MINOTAUR, (pid_t)(-i - 1), false,
                                         "Minotaur", mDefHealth, mAi);
    pGame->minotaurA[i].pos = mAPos[i];
    pGame->minotaurA[i].pMask = map_mask_init(pMap);
    map_mask_add(pGame->minotaurA[i].pMask, mAPos[i]);
    
    // NOTE: Targets are not set yet
  }
  free(mAPos);
  mAPos = NULL;
  
  // Load a player(s)
  pos_t* pAPos = NULL;
  pGame->nbPlayer = gps_locator(pMap, PLAYER, &pAPos);
  pGame->nbPlayerAlive = pGame->nbPlayer;
  pGame->nbPlayerOnBoard = pGame->nbPlayer;
  if (pGame->nbPlayer == 0) {
    // Need at least one player
    ok = false;
  }
  pGame->playerA = (character_t*)malloc(pGame->nbPlayer * sizeof(character_t));
  config_t config=config_init(); //-------------------------
  pid_t display[pGame->nbPlayer]; //------------------------------------
  for (size_t i = 0; i < pGame->nbPlayer; ++i) {
    // Get the display to use with this player.
    usleep((unsigned int)pGame->delay);
    display[i]=fork();
    if(display[i]==-1){
      return EXIT_FAILURE;
    }
    if(display[i]==0){
      char geometry[20];
      if(i<4){
        sprintf(geometry,"80x23+%d+0",(int)(i%4)*400);
      }else{
        sprintf(geometry,"80x23+%d-0",(int)(i%4)*400);
      }
      execlp("xterm","xterm","-geometry",geometry,NULL);
    }
    usleep((unsigned int)pGame->delay);
    config_add_display(&config, display[i]+2); //------------------------------------
    
    // Init player
    pGame->playerA[i] =
      character_init(PLAYER, display[i], true, "Theseus", pDefHealth, pAi);
    pGame->playerA[i].pos = pAPos[i];
    pGame->playerA[i].pMask = map_mask_init(pMap);
    map_mask_add(pGame->playerA[i].pMask, pAPos[i]);
    // NOTE: Targets are not set yet
  }
  free(pAPos);
  pAPos = NULL;
  
  // Add missing exit (if final)
  if (ok && noExit && pGame->finalLevel) {
    for (size_t i = 0; i < pGame->nbPlayer; ++i) {
      if ((pGame->playerA[i].pos.x == 0) || (pGame->playerA[i].pos.y == 0)) {
        // add an exit
        if (pGame->nbExit == 0) {
          pGame->exitA = (pos_t*)malloc(1 * sizeof(pos_t));
        } else {
          pGame->exitA = (pos_t*)realloc(pGame->exitA,
                          (pGame->nbExit + 1) * sizeof(pos_t));
        }
        pGame->exitA[pGame->nbExit].x = pGame->playerA[i].pos.x;
        pGame->exitA[pGame->nbExit].y = pGame->playerA[i].pos.y;
        pGame->playerA[i].walkOn = EXIT;
        ++(pGame->nbExit);
      }
    }
  }
  if (pGame->nbExit == 0) {
    ok = false;
  }
  
  // Init targets
  for (size_t i = 0; i < pGame->nbPlayer; ++i) {
    character_t* pC = &(pGame->playerA[i]);
    gps_direction(pC->pos, _game_character_target(pGame, pC),
                  &(pC->targetCompass), &(pC->targetDistance));
  }
  for (size_t i = 0; i < pGame->nbMinotaur; ++i) {
    character_t* pC = &(pGame->minotaurA[i]);
    gps_direction(pC->pos, _game_character_target(pGame, pC),
                  &(pC->targetCompass), &(pC->targetDistance));
  }
  
  return ok;
}

void game_start(game_t* pGame) {
  // Print UI
  
  // Initial display for game master
  display_ui_gm(DISPLAY, pGame->gameName, pGame->pMap, pGame->nbPlayerAlive,
                pGame->nbPlayerOnBoard, pGame->nbMinotaurAlive, pGame->delay,
                pGame->gameInfo, false);
  
  // Initial display for each player
  for (size_t i = 0; i < pGame->nbPlayer; ++i) {
    const character_t* pC = &(pGame->playerA[i]);
    display_ui_player(pGame->gameName, pC->ai.name, pGame->pMap, pC,
                      pGame->delay, pGame->maxMoves, pGame->gameInfo, true,
                      false);
  }
  
  size_t nbChar = 0;
  moves_prop_t* moves = NULL;
  _game_init_moves_prop(pGame, &moves, &nbChar);
  
  
  pid_t tab[nbChar];  //Tableau d etat des persos
  for(size_t i=0;i<nbChar;i++){
    tab[i]=0;
  }
  
  do {
    int status;
    for (size_t i=0;i<nbChar;i++){
      if(tab[i]==0){
        tab[i]=fork();
        if (tab[i]==-1){
          exit(EXIT_FAILURE);
        }
        if(tab[i]==0){
          getchar();
          // Play characters
          pGame->steps += 1;
          usleep((unsigned int)pGame->delay);
          
          // Fights
          _game_fight_manager(pGame);
          // Get characters move propositions
          _game_get_moves_propositions(pGame, moves, nbChar);
          
          // Solve confilcts
          _gave_solve_moves_conflicts(pGame, moves, nbChar);
          
          // Play characters
          _game_play_characters(pGame, moves, nbChar);
          
          // refresh displays
          _game_play_refresh_ui(pGame);
          
          // Fights
          _game_fight_manager(pGame);
          printf("je suis %d",(int)i);
          exit(tab[i]);
        }else{
          wait(&status);
          status=WEXITSTATUS(status);
          tab[i]=0;
          printf("status %d\n ",status);
        }
      }
    }
  } while (pGame->nbPlayerOnBoard > 0);
  
  free(moves);
  nbChar = 0;
  
  // The end
  for (size_t i = 0; i < pGame->nbPlayer; ++i) {
    const character_t* pC = &(pGame->playerA[i]);
    // Last display for each player
    display_ending(pGame->playerA[i].stream, _game_ending_player(pGame, pC));
  }
  //fclose(f);
  display_ending(DISPLAY, _game_ending_gm(pGame));
}

void game_delete(game_t* pGame) {
  pGame->gameName = NULL;
  
  free(pGame->exitA);
  pGame->exitA = NULL;
  pGame->nbExit = 0;
  
  for (size_t i = 0; i < pGame->nbPlayer; ++i) {
    character_delete(pGame->pMap, &(pGame->playerA[i]));
  }
  free(pGame->playerA);
  pGame->playerA = NULL;
  pGame->nbPlayer = 0;
  pGame->nbPlayerAlive = 0;
  pGame->nbPlayerOnBoard = 0;
  
  for (size_t i = 0; i < pGame->nbMinotaur; ++i) {
    character_delete(pGame->pMap, &(pGame->minotaurA[i]));
  }
  free(pGame->minotaurA);
  pGame->minotaurA = NULL;
  pGame->nbMinotaur = 0;
  pGame->nbMinotaurAlive = 0;
  
  pGame->finalLevel = false;
  pGame->maxMoves = 0;
  pGame->steps = 0;
  pGame->delay = 0;
  
  map_delete(pGame->pMap);
  pGame->pMap = NULL;
}
  