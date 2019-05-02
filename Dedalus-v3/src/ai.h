/**
 * @file ai.h
 * @author Chevelu Jonathan (jonathan.chevelu@irisa.fr)
 * @brief AI definition and factory.
 * @version 0.1
 * @date 2019-01-28
 *
 * @copyright Copyright (c) 2019
 *
 */
#ifndef IA_H
#define IA_H

#include "ariadneString.h"

/**
 * @brief Structure of a AI for a character.
 *
 */
typedef struct ai {
  const char* name;  ///< Name of the AI.
  compass_t (*policy)(bool north,
                      bool east,
                      bool south,
                      bool west,
                      compass_t comp,
                      float distance,
                      string ariane);  ///< Policy of the AI.
} ai_t;

/**
 * @brief AI factory.
 *
 * @param[in] name Name of the AI to build.
 * @return ai_t The build AI. If name unknow, return the default AI (A Shall not
 * pass AI).
 */
ai_t ai_new(const char* name);

#endif  // End of AI_H