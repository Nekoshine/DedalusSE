/**
 * @file ai_shall_not_pass.h
 * @author Chevelu Jonathan (jonathan.chevelu@irisa.fr)
 * @brief Artificial Intelligence who will not move.
 * @version 0.1
 * @date 2019-02-20
 * 
 * @copyright Copyright (c) 2019
 * 
 */
#ifndef AI_SHALL_NOT_PASS_H
#define AI_SHALL_NOT_PASS_H

#include "../ai.h"

/**
 * @brief Get the name of the "shall not pass" AI class.
 * 
 * @return const char* The name of the AI.
 */
const char * ai_shall_not_pass_get_name();

/**
 * @brief Create a new AI that will never move.
 * 
 * @return ai_t The new AI.
 */
ai_t ai_shall_not_pass_new();

#endif // End of AI_SHALL_NOT_PASS_H