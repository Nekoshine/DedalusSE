/**
 * @file gps.c
 * @author Chevelu Jonathan (jonathan.chevelu@irisa.fr)
 * @brief Orientation (GPS) definition and management.
 * @version 0.1
 * @date 2019-01-24
 *
 * @copyright Copyright (c) 2019
 *
 */
#include <math.h>  // sqrt

#include "character.h"
#include "display.h"  // for fatal error
#include "gps.h"

/**********************************/
// Declaration of local constants.

/**
 * @brief Internal array to convert compass to a string. Text version.
 * @note Unused so far.
 *
 */
static const char* const _gps_directions_txt[] = {
    "N", "NE", "E", "SE", "S", "SW", "W", "NW", "Broken"};

/**
 * @brief Internal array to convert compass to a string. Arrow version.
 *
 */
static const char* const _gps_directions_arrows[] = {"↑", "↗", "→", "↘", "↓",
                                                     "↙", "←", "↖", "•"};

/**********************************/
// Public functions implementations.

size_t gps_locator(const map_t* pMap, map_content_t object, pos_t** pAPos) {
  size_t nbFound = 0;

  *pAPos = NULL;

  for (size_t y = 0; y < pMap->y; ++y) {
    for (size_t x = 0; x < pMap->x; ++x) {
      if (pMap->m[y][x] == (char)object) {
        if (nbFound == 0) {
          *pAPos = (pos_t*)malloc(1 * sizeof(pos_t));
        } else {
          *pAPos = (pos_t*)realloc(*pAPos, (nbFound + 1) * sizeof(pos_t));
        }
        (*pAPos)[nbFound].x = x;
        (*pAPos)[nbFound].y = y;
        ++nbFound;
      }
    }
  }
  return nbFound;
}

void gps_direction(pos_t source, pos_t target, compass_t* pC, float* pD) {
  long long int dx = (long long int)(source.x - target.x);
  long long int dy = (long long int)(source.y - target.y);

  *pC = Stay;
  if (dx == 0 && dy > 0) {
    *pC = North;
  } else if (dx < 0 && dy > 0) {
    *pC = NorthEast;
  } else if (dx < 0 && dy == 0) {
    *pC = East;
  } else if (dx < 0 && dy < 0) {
    *pC = SouthEast;
  } else if (dx == 0 && dy < 0) {
    *pC = South;
  } else if (dx > 0 && dy < 0) {
    *pC = SouthWest;
  } else if (dx > 0 && dy == 0) {
    *pC = West;
  } else if (dx > 0 && dy > 0) {
    *pC = NorthWest;
  }

  *pD = (float)sqrt(pow((double)dx, 2) + pow((double)dy, 2)) * 10;
}

const char* gps_compass_to_string(compass_t c) {
  const char* const* dir_array = _gps_directions_arrows;
  (void)_gps_directions_txt;  // unused

  return dir_array[c];
}

pos_t gps_closest(pos_t source,
                  const void* targets,
                  size_t nbTargets,
                  map_content_t arrayType) {
  float bestDist = -1;
  pos_t closest;
  closest.x = 0;
  closest.y = 0;

  for (size_t i = 0; i < nbTargets; ++i) {
    compass_t cComp;
    float cDist = -1;
    pos_t cPos;

    switch (arrayType) {
      case EXIT:
        cPos = ((pos_t*)(targets))[i];
        break;
      case PLAYER:
      case MINOTAUR:
        cPos = ((character_t*)(targets))[i].pos;
        break;
      default:
        display_fatal_error(stderr,
                            "Error (gps_closest): Unknow array type.\n");
        exit(EXIT_FAILURE);
        break;
    }
    gps_direction(source, cPos, &cComp, &cDist);
    if ((bestDist < 0) || (cDist < bestDist)) {
      bestDist = cDist;
      closest = cPos;
    }
  }

  return closest;
}

pos_t gps_compute_move(pos_t pos, compass_t move) {
  pos_t res;
  res.x = pos.x;
  res.y = pos.y;
  switch (move) {
    case North:
      res.y = pos.y - 1;
      break;
    case East:
      res.x = pos.x + 1;
      break;
    case West:
      res.x = pos.x - 1;
      break;
    case South:
      res.y = pos.y + 1;
      break;
    default:
      break;
  }
  return res;
}