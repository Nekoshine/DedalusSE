/**
 * @file ai.c
 * @author Chevelu Jonathan (jonathan.chevelu@irisa.fr)
 * @brief AI definition and factory.
 * @version 0.1
 * @date 2019-01-28
 * 
 * @copyright Copyright (c) 2019
 * 
 */
#include <string.h>

#include "ai.h"
#include "ai/ai_random.h"
#include "ai/ai_shall_not_pass.h"

ai_t ai_new(const char* name) {
  ai_t res;
  size_t len = strlen(name);
  if (strncmp(name, ai_random_get_name(), len) == 0) {
    res = ai_random_new();
  } else if (strncmp(name, ai_shall_not_pass_get_name(), len) == 0) {
    res = ai_shall_not_pass_new();
  } else {
    res = ai_shall_not_pass_new();
  }

  return res;
}