/**
 * @file config.h
 * @author Chevelu Jonathan (jonathan.chevelu@irisa.fr)
 * @brief Structure to configure the game.
 * @version 0.1
 * @date 2019-02-27
 *
 * @copyright Copyright (c) 2019
 *
 */
#ifndef CONFIG_H
#define CONFIG_H

#include <stdbool.h>    // bool, true, false
#include <sys/types.h>  // pid_t

// Configuration
extern const bool
    COLOR;  ///< Game in vintage colors if true and in B&W otherwise
extern const bool
    DEBUG;  ///< Game in debug mode if true and in normal mode otherwise

/**
 * @brief Game configuration structure.
 *
 */
typedef struct config {
  const char* mapFile;  ///< Path to the map file.
  size_t mapMaxXSize;   ///< Max width of a map.

  bool color;     ///< Display with color?
  bool debug;     ///< Display in debug mode (no cleaning)?
  bool gameInfo;  ///< Display more game informations?

  int delay;         ///< Time (in us) between frames.
  bool interactive;  ///< Ask for interactive actions from GM.
  int maxMoves;      ///< Maximum number of moves for players.

  size_t nbDisplay;    ///< Number of display for players.
  pid_t* displayPidA;  ///< Array of pid of terminals to display players.

} config_t;

/**
 * @brief Return an initialized configuration with default values.
 *
 * @return config_t A default configuration.
 */
config_t config_init();

/**
 * @brief Clear a configuration.
 *
 * @param[in,out] pConfig The configuration to clear.
 */
void config_delete(config_t* pConfig);

/**
 * @brief Add a display in the configuration
 *
 * @param[in,out] pConfig The configuration to change.
 * @param[in] displayPid The pid of the process to write into (basically a
 * bash).
 */
void config_add_display(config_t* pConfig, pid_t displayPid);

#endif