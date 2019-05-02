/**
 * @file map.c
 * @author Chevelu Jonathan (jonathan.chevelu@irisa.fr)
 * @brief Map definition and management.
 * @version 0.1
 * @date 2019-01-24
 *
 * @copyright Copyright (c) 2019
 *
 */

#include <stdio.h>   // printf
#include <stdlib.h>  // malloc, rand, srand
#include <string.h>  // strcpy, strlen

#include "config.h"
#include "display.h"
#include "map.h"

/**********************************/
// Declaration of local functions.

/**
 * @brief Return a decremented unsigned int
 *
 * @param[in] i Value to decrement.
 * @return size_t i - 1 if i > 0 else 0.
 */
size_t _map_decr_unsigned(size_t i);

/**
 * @brief Increment a number but not over a limit.
 *
 * @param[in] i Value to increment.
 * @param[in] limit Limit for i.
 * @return size_t i + 1 if i < limit else limit.
 */
size_t _map_incr_limit(size_t i, size_t limit);

/*****************************/
// Functions implementation.

size_t _map_decr_unsigned(size_t i) {
  if (i > 0) {
    return (i - 1);
  }
  return 0;
}

size_t _map_incr_limit(size_t i, size_t limit) {
  if (i < limit) {
    return (i + 1);
  }
  return limit;
}

/**********************************/
// Public functions implementations.

void map_delete(map_t* pMap) {
  if ((pMap == NULL) || (pMap->y == 0)) {
    return;  // Nothing to do
  }

  for (size_t l = 0; l < pMap->y; ++l) {
    free(pMap->m[l]);
    pMap->m[l] = NULL;
  }
  free(pMap->m);
  pMap->m = NULL;
  pMap->x = 0;
  pMap->y = 0;
}

bool map_reader(const char* filename, map_t* pMap, size_t mapMaxXSize) {
  pMap->m = NULL;  // init the map
  pMap->y = 0;
  pMap->x = 0;

  // open the file
  FILE* pf = fopen(filename, "r");
  if (pf == NULL) {
    // Error
    fprintf(stderr, "No file found for map %s!\n", filename);
    return false;  // return error
  }

  bool mapLoaded = true;  // everything is fine so far
  char buffer[mapMaxXSize];

  if (fscanf(pf, "%s", buffer) == 0) {
    mapLoaded = false;
  } else {
    pMap->x = strlen(buffer);
  }
  size_t nbColumn = pMap->x + 1;

  while (mapLoaded && !feof(pf)) {
    if (pMap->x == strlen(buffer)) {
      pMap->y = pMap->y + 1;
      pMap->m = (char**)realloc(pMap->m, pMap->y * sizeof(char*));
      pMap->m[pMap->y - 1] = (char*)malloc(nbColumn * sizeof(char));
      strncpy((pMap->m)[pMap->y - 1], buffer, nbColumn);
      buffer[0] = '\0';  // flush the buffer
      if ((pMap->m[pMap->y - 1][nbColumn - 1] != '\0') ||
          (fscanf(pf, "%s", buffer) == 0)) {
        fprintf(stderr, "Problem here: %s:%d\n", __FILE__, __LINE__);
        mapLoaded = false;
      }
    } else {
      fprintf(stderr, "Problem with buffer size (%lu, expect %lu) on line %lu here: %s:%d\n",
              pMap->x, strlen(buffer), pMap->y +1, __FILE__, __LINE__);
      mapLoaded = false;
    }
  }
  // If buffer not empty
  if (mapLoaded && (pMap->x == strlen(buffer))) {
    pMap->y = pMap->y + 1;
    pMap->m = (char**)realloc(pMap->m, pMap->y * sizeof(char*));
    pMap->m[pMap->y - 1] = (char*)malloc(nbColumn * sizeof(char));
    strncpy((pMap->m)[pMap->y - 1], buffer, nbColumn);
    if (pMap->m[pMap->y - 1][nbColumn - 1] != '\0') {
      fprintf(stderr, "Problem here: %s:%d\n", __FILE__, __LINE__);
      mapLoaded = false;
    }
  }

  fclose(pf);

  // free map if fail
  if (!mapLoaded) {
    map_delete(pMap);
  }

  return mapLoaded;
}

map_t* map_mask_init(const map_t* pMap) {
  map_t* pMask = (map_t*)malloc(1 * sizeof(map_t));
  pMask->x = pMap->x;
  pMask->y = pMap->y;

  pMask->m = (char**)malloc(pMask->y * sizeof(char*));
  for (size_t i = 0; i < pMask->y; ++i) {
    pMask->m[i] = (char*)calloc(pMask->x, sizeof(char));
  }

  return pMask;
}

void map_mask_add(map_t* pMask, pos_t p) {
  pMask->m[p.y][p.x] = DISPLAY_MASK;

  pMask->m[_map_decr_unsigned(p.y)][p.x] = DISPLAY_MASK;
  pMask->m[p.y][_map_decr_unsigned(p.x)] = DISPLAY_MASK;
  pMask->m[_map_incr_limit(p.y, pMask->y - 1)][p.x] = DISPLAY_MASK;
  pMask->m[p.y][_map_incr_limit(p.x, pMask->x - 1)] = DISPLAY_MASK;

  pMask->m[_map_decr_unsigned(p.y)][_map_decr_unsigned(p.x)] = DISPLAY_MASK;
  pMask->m[_map_decr_unsigned(p.y)][_map_incr_limit(p.x, pMask->x - 1)] =
      DISPLAY_MASK;
  pMask->m[_map_incr_limit(p.y, pMask->y - 1)][_map_decr_unsigned(p.x)] =
      DISPLAY_MASK;
  pMask->m[_map_incr_limit(p.y, pMask->y - 1)]
          [_map_incr_limit(p.x, pMask->x - 1)] = DISPLAY_MASK;
}
