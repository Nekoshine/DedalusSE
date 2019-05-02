/**
 * @file config.c
 * @author Chevelu Jonathan (jonathan.chevelu@irisa.fr)
 * @brief Structure to configure the game.
 * @version 0.1
 * @date 2019-02-27
 *
 * @copyright Copyright (c) 2019
 *
 */

#include "config.h"
#include "display.h"

#include <stdlib.h>

// Configuration
const bool COLOR = true;
// const bool DEBUG = true;
const bool DEBUG = false;

config_t config_init() {
  config_t conf;

  conf.mapFile = NULL;
  conf.mapMaxXSize = 1000;

  conf.color = COLOR;
  conf.debug = DEBUG;
  conf.gameInfo = false;

  conf.delay = 100000;
  conf.interactive = true;
  conf.maxMoves = 1000;

  conf.displayPidA = NULL;
  conf.nbDisplay = 0;

  return conf;
}

void config_delete(config_t* pConfig) {
  free(pConfig->displayPidA);
  pConfig->displayPidA = NULL;
  pConfig->nbDisplay = 0;
}

void config_add_display(config_t* pConfig, pid_t displayPid) {
  if (pConfig->nbDisplay == 0) {
    pConfig->displayPidA = (pid_t*)malloc(1 * sizeof(pid_t));
  } else {
    pConfig->displayPidA = (pid_t*)realloc(
        pConfig->displayPidA, (pConfig->nbDisplay + 1) * sizeof(pid_t));
  }
  pConfig->displayPidA[pConfig->nbDisplay] = displayPid;

  ++pConfig->nbDisplay;
}
