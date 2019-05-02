/**
 * @file dedalus.c
 * @author Francois Goasdoue
 * @author Chevelu Jonathan (jonathan.chevelu@irisa.fr)
 * @brief A dedalus game.
 * @version 0.1
 * @date 2019-02-19
 *
 * @copyright Copyright (c) 2019
 *
 */

#include <getopt.h>   /* Getopt */
#include <limits.h>   // INT_MAX
#include <stdbool.h>  // bool, true, false
#include <stdio.h>    // printf
#include <stdlib.h>   // malloc, rand, srand
#include <string.h>   // strcpy, strlen
#include <time.h>     // time

#include <unistd.h>
#include <sys/errno.h>
#include <sys/wait.h>

#include "ai.h"
#include "ai/ai_random.h"
#include "ai/ai_shall_not_pass.h"
#include "config.h"   // configuration
#include "display.h"  // Wait user and info
#include "game.h"     // game setting
#include "map.h"      // manage the map

/**
 * @brief Parse parameters to build the configuration of the game.
 *
 * @param[in] argc Number of parameters.
 * @param[in] argv Array of parameters.
 * @param[out] pConfig The configuration structure (must be allocated).
 */
void read_parameters(int argc, char* argv[], config_t* pConfig);

/**
 * @brief Display the program usage.
 *
 */
void usage();

/**
 * @brief Main of Dedalus
 *
 * @param[in] argc Number of parameters.
 * @param[in] argv Array of parameters.
 * @return int Dedalus success.
 */
int main(int argc, char* argv[]) {
  // INIT Srand
  srand((unsigned int)time(NULL));

  // acquiring program parameters
  config_t config = config_init();
  read_parameters(argc, argv, &config);
  
  map_t map;
  bool mapLoaded = map_reader(config.mapFile, &map, config.mapMaxXSize);
  if (!mapLoaded) {
    display_fatal_error(stderr, "Invalid map !\n");
    return EXIT_FAILURE;
  }

  // Init game from initial map
  game_t game;
  if (!game_init(&game, &config, config.mapFile, &map, 100,
                 ai_new(ai_random_get_name()), 10,
                 ai_new(ai_random_get_name()))) {
    map_delete(&map);
    display_fatal_error(stderr, "Wrong map (no player or no exit)!\n");
    return EXIT_FAILURE;
  }
  
  // Start game
  display_wait_user(stdout, "Press any key to start...", game.interactive);
  game_start(&game);

  display_wait_user(stdout, "Press any key to finish...", game.interactive);

  // Clear at the end
  game_delete(&game);
  config_delete(&config);
  /*for(i=0;i<nbPlayer;i++){
    kill(pid_fils[i],SIGKILL);
  }*/
  execlp("killall","killall","xterm",NULL);
  return EXIT_SUCCESS;
}

void read_parameters(int argc, char* argv[], config_t* pConfig) {
  /* Parameters parsing */
  int c = 0;
  int errflg = 0;
  int mandatory = 0;
  unsigned long tmp = 0;

  while ((c = getopt(argc, argv, "had:m:M:p:")) != -1) {
    switch (c) {
      case 'h':  // help.
        usage();
        exit(EXIT_SUCCESS);
        break;
      case 'm':  // map file.
        pConfig->mapFile = optarg;
        ++mandatory;
        break;
      case 'M':  // max moves for players.
        tmp = strtoul(optarg, NULL, 0);
        if (tmp > INT_MAX) {
          ++errflg;
        }
        pConfig->maxMoves = (int)tmp;
        break;
      case 'd':  // delay between frames.
        tmp = strtoul(optarg, NULL, 0);
        if (tmp > INT_MAX) {
          ++errflg;
        }
        pConfig->delay = (int)tmp;
        break;
      case 'a':  // automatic mode (not interactive).
        pConfig->interactive = false;
        break;
      case ':': /* option without operand */
        fprintf(stderr, "Option -%c requires an operand\n", optopt);
        ++errflg;
        break;
      case '?':
        fprintf(stderr, "Unrecognized option: -%c\n", optopt);
        ++errflg;
    }
  }
  if (mandatory != 1) {
    fprintf(stderr, "ERROR: mandatory option is missing (-m).\n");
    ++errflg;
  }

  if (errflg) {
    usage();
    exit(EXIT_FAILURE);
  }
}

void usage() {
  fprintf(stderr,
          "Usage: ./Dedalus [-h] -m arg [-M arg] [-d arg] [-a] [-p arg -p arg  "
          "...]    \n");
  fprintf(stderr, "\n");
  fprintf(stderr, "Options: \n");
  fprintf(stderr, "\t -m arg \t (Mandatory) file of the map.\n");
  fprintf(stderr, "\t -p arg \t (Multiple) PID of the player terminal.\n");
  fprintf(stderr, "\t -d arg \t [100000] Delay (in μs).\n");
  fprintf(stderr, "\t -a     \t [false] Automatic mode (not interactive).\n");
  fprintf(stderr, "\t -M arg \t [1000] Maximum number of steps for players.\n");
  fprintf(stderr, "\t -h     \t Display this message.	\n");
}
