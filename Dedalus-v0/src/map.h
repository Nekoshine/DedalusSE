/**
 * @file map.h
 * @author Chevelu Jonathan (jonathan.chevelu@irisa.fr)
 * @brief Map definition and management.
 * @version 0.1
 * @date 2019-01-24
 *
 * @copyright Copyright (c) 2019
 *
 */
#ifndef MAP_H
#define MAP_H

#include <stdbool.h>
#include <stdlib.h>

/**
 * @brief coordinate in a map.
 *
 */
typedef struct pos {
  size_t x;  ///< column coordinate.
  size_t y;  ///< row coordinate.
} pos_t;

/**
 * @brief List of symbols that can occur in a map or in a mask map.
 *
 */
typedef enum map_content {
  END_ROW = '\0',      ///< Symbol for the end of a row.
  PLAYER = '@',        ///< Symbol for a player.
  EXIT = '?',          ///< Symbol for an exit.
  WALL = '*',          ///< Symbol for a wall.
  PATH = '.',          ///< Symbol for an empty path in the dedalus.
  MINOTAUR = '&',      ///< Symbol for a minotaur.
  DEAD = '+',          ///< Symbol for a dead character.
  DISPLAY_MASK = 'X',  ///< Symbol in a mask map to ask for a display.
} map_content_t;

/**
 * @brief A map of the dedalus (one level).
 *
 * @note The matrix contains char instead of map_content_t in order to be wild
 * enough to store all kind of characters.
 */
typedef struct map {
  char** m;  ///< Matrix of caracters.
  size_t x;  ///< Number of columns.
  size_t y;  ///< Number of rows.
} map_t;

/**
 * @brief Read a map from a file.
 *
 * @param[in] filename Path to the file that contains the map in a text format.
 * @param[out] pMap The loaded map if loading succeed, *NULL else).
 * @param[in] mapMaxXSize Maximum width of a board.
 * @return true Loading succeed.
 * @return false  Loading failed (wrong format or missing content)
 */
bool map_reader(const char* filename, map_t* pMap, size_t mapMaxXSize);

/**
 * @brief Clear the content of the map.
 *
 * @param[in,out] pMap Pointer to the map to clear.
 * @note The map itself is not free, only its content.
 */
void map_delete(map_t* pMap);

/**
 * @brief Build an empty display mask from a map.
 *
 * @param[in] pMap The map associated to the mask to create.
 * @return map_t* An empty map mask (displays nothing).
 */
map_t* map_mask_init(const map_t* pMap);

/**
 * @brief Add a position to display in a map mask.
 *
 * @param[in, out] pMask
 * @param[in] p Position to add in the mask (i.e. to display).
 * @note Add the given position and the neighborhood.
 */
void map_mask_add(map_t* pMask, pos_t p);

#endif  // End of MAP_H
