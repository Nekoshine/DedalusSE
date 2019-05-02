/**
 * @file ariadneString.c
 * @author Chevelu Jonathan (jonathan.chevelu@irisa.fr)
 * @brief Ariadne string definition and management.
 * @version 0.1
 * @date 2019-02-20
 *
 * @copyright Copyright (c) 2019
 *
 */

#include "ariadneString.h"
#include "display.h"
#include "gps.h"

#include <stdio.h>   // printf
#include <stdlib.h>  // malloc, rand, srand
#include <string.h>  // strcpy, strlen

string string_new() {
  return NULL;
}

void string_delete(string* pS) {
  if (pS == NULL) {
    // error
    return;
  }
  while (*pS != NULL) {
    link_t* pHead = *pS;
    *pS = pHead->next;
    free(pHead);
  }
}

bool string_remove_link(string* pS) {
  string plink = *pS;

  if (*pS == NULL) {
    return false;
  }

  *pS = (*pS)->next;
  free(plink);
  return true;
}

bool string_add_link(string* pS, compass_t c) {
  string plink = (string)malloc(sizeof(link_t));

  if (plink == NULL) {
    return false;
  }

  plink->c = c;
  plink->next = *pS;
  *pS = plink;
  return true;
}

bool string_can_go_back(const string s) {
  bool res = false;

  if (string_size(s) >= 2) {
    if (s->c == North && (s->next)->c == South) {
      res = true;
    } else if (s->c == South && (s->next)->c == North) {
      res = true;
    } else if (s->c == East && (s->next)->c == West) {
      res = true;
    } else if (s->c == West && (s->next)->c == East) {
      res = true;
    }
  }

  return res;
}

size_t string_size(const string s) {
  size_t size = 0;
  string tmp = s;

  while (tmp != NULL) {
    ++size;
    tmp = tmp->next;
  }

  return size;
}
