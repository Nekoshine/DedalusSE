/**
 * @file gps.h
 * @author Chevelu Jonathan (jonathan.chevelu@irisa.fr)
 * @brief Orientation (GPS) definition and management.
 * @version 0.1
 * @date 2019-01-24
 *
 * @copyright Copyright (c) 2019
 *
 */
#ifndef GPS_H
#define GPS_H

#include "map.h"

/**
 * @brief Enumeration of the different directions that the compass can
 * indicate.
 *
 * @note Values are forced to help display.
 */
typedef enum compass {
  North = 0,
  NorthEast = 1,
  East = 2,
  SouthEast = 3,
  South = 4,
  SouthWest = 5,
  West = 6,
  NorthWest = 7,
  Stay = 8
} compass_t;

/**
 * @brief Returns the number of objects of a given type in a map and their
 * position.
 *
 * @param[in] pMap The map to explore.
 * @param[in] object The type of object to search (player for instance).
 * @param[out] pAPos Array of positions found (must be free outside).
 * @return size_t Number of objects found.
 */
size_t gps_locator(const map_t* pMap, map_content_t object, pos_t** pAPos);

/**
 * @brief Return the direction and the distance between tow positions.
 *
 * @param[in] source Source position.
 * @param[in] target Target position.
 * @param[out] pC The direction of the target from the source.
 * @param[out] pD Distance between the two positions.
 */
void gps_direction(pos_t source, pos_t target, compass_t* pC, float* pD);

/**
 * @brief From a position, return the direction of the closest object in an
 * array.
 *
 * @param[in] source Source position.
 * @param[in] targets A vector to search in. Could be a vector of pos_t or of
 * character_t.
 * @param[in] nbTargets Number of items in the vector "targets".
 * @param[in] arrayType Type of the content in "targets" (pos_t or character_t).
 * @return pos_t Position of the closet item of "targets".
 */
pos_t gps_closest(pos_t source,
                  const void* targets,
                  size_t nbTargets,
                  map_content_t arrayType);

/**
 * @brief Convert a compass into a string for display.
 *
 * @param[in] c Direction to convert.
 * @return const char* Constant string associated to the direction.
 * @note May be a unicode string.
 */
const char* gps_compass_to_string(compass_t c);

/**
 * @brief Compute the new positions after a move.
 *
 * @param[in] pos The originale position.
 * @param[in] move The move to do.
 * @return pos_t The new position.
 */
pos_t gps_compute_move(pos_t pos, compass_t move);

#endif  // End GPS_H
