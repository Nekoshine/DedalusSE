/**
 * @file ai_random.c
 * @author Chevelu Jonathan (jonathan.chevelu@irisa.fr)
 * @brief Random Artificial Intelligence.
 * @version 0.1
 * @date 2019-02-20
 *
 * @copyright Copyright (c) 2019
 *
 */

#include <stdbool.h>  // bool, true, false
#include <stdlib.h>   // rand
#include <time.h>     // time
#include <unistd.h>   // usleep

#include "ai_random.h"

/**
 * @brief Select the next move randomly between all possible moves.
 *
 * @param[in] north Is it possible to go north?
 * @param[in] east Is it possible to go east?
 * @param[in] south Is it possible to go south?
 * @param[in] west Is it possible to go west?
 * @param[in] comp Unused parameter.
 * @param[in] distance Unused parameter.
 * @param[in] ariane Unused parameter.
 * @return compass_t The selected direction.
 */
compass_t _ai_random_policy(bool north,
                            bool east,
                            bool south,
                            bool west,
                            compass_t comp,
                            float distance,
                            string ariane) {
  // remove unused param
  (void)comp;
  (void)distance;
  (void)ariane;

  // reinit the srand
  srand((unsigned int) (time(NULL) * getpid() * rand()));

  // Random thinking
  usleep((unsigned int) ((rand() % 100) * 1000));

  if (!north && !east && !south && !west) {
    return Stay;
  }

  enum compass move;
  bool ok = false;

  do {
    switch (rand() % 4) {
      case 0:
        move = North;
        ok = north;
        break;
      case 1:
        move = East;
        ok = east;
        break;
      case 2:
        move = South;
        ok = south;
        break;
      case 3:
        move = West;
        ok = west;
        break;
      default:
        ok = false;
    }
  } while (!ok);

  return move;
}

const char* ai_random_get_name() {
  return "Random";
}

ai_t ai_random_new() {
  ai_t res;
  res.policy = &_ai_random_policy;
  res.name = ai_random_get_name();
  return res;
}
