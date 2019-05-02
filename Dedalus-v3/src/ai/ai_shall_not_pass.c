/**
 * @file ai_shall_not_pass.c
 * @author Chevelu Jonathan (jonathan.chevelu@irisa.fr)
 * @brief Artificial Intelligence who will not move.
 * @version 0.1
 * @date 2019-02-20
 * 
 * @copyright Copyright (c) 2019
 * 
 */
#include <stdbool.h>  // bool, true, false
#include <stdlib.h>   // rand

#include "ai_shall_not_pass.h"

/**
 * @brief The next move is stay here(!).
 *
 * @param[in] north Unused parameter.
 * @param[in] east Unused parameter.
 * @param[in] south Unused parameter.
 * @param[in] west Unused parameter.
 * @param[in] comp Unused parameter.
 * @param[in] distance Unused parameter.
 * @param[in] ariane Unused parameter.
 * @return compass_t I stay here!
 */
compass_t _ai_shall_not_pass_policy(bool north,
                                    bool east,
                                    bool south,
                                    bool west,
                                    compass_t comp,
                                    float distance,
                                    string ariane) {
  // remove unused param
  (void)north;
  (void)east;
  (void)south;
  (void)west;
  (void)distance;
  (void)ariane;
  (void)comp;
  return Stay;
}

const char* ai_shall_not_pass_get_name() {
  return "You Shall Not Pass";
}

ai_t ai_shall_not_pass_new() {
  ai_t res;
  res.policy = &_ai_shall_not_pass_policy;
  res.name = ai_shall_not_pass_get_name();
  return res;
}
