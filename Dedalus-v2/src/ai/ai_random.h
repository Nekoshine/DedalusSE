/**
 * @file ai_random.h
 * @author Chevelu Jonathan (jonathan.chevelu@irisa.fr)
 * @brief Random Artificial Intelligence.
 * @version 0.1
 * @date 2019-02-20
 *
 * @copyright Copyright (c) 2019
 *
 */
#ifndef AI_RANDOM_H
#define AI_RANDOM_H

#include "../ai.h"

/**
 * @brief Get the name of the random AI class.
 *
 * @return const char* The name of the AI.
 */
const char* ai_random_get_name();

/**
 * @brief Create a new AI that will move randomly.
 *
 * @return ai_t The new AI.
 */
ai_t ai_random_new();

#endif  // End of AI_RANDOM_H