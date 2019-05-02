/**
 * @file ariadneString.h
 * @author Chevelu Jonathan (jonathan.chevelu@irisa.fr)
 * @brief Ariadne string definition and management.
 * @version 0.1
 * @date 2019-02-20
 *
 * @copyright Copyright (c) 2019
 *
 */
#ifndef ARIADNE_STRING_H
#define ARIADNE_STRING_H

#include "gps.h"

/**
 * @brief Linked list used to model the Ariadne string.
 * @warning The last move made is recorded in the first item in the list, and
  the first move made is at the last item in the list. As customary, the list is
  empty when Theseus has not moved yet (ie when positionned at the dedalus
  entrance).
 */
typedef struct link {
  compass_t c;        ///< Last move made
  struct link* next;  ///< Link to the previous move. NULL if beginning of
                      ///< the string.
} link_t;

/**
 * @brief A string is a pointer to a link_t. NULL if the string is empty.
 * 
 */
typedef link_t* string;

/**
 * @brief Create a new string.
 * 
 * @return string An empty string.
 */
string string_new();

/**
 * @brief Delete a string.
 * 
 * @param[in, out] pS The string to delete.
 */
void string_delete(string* pS);

/**
 * @brief Remove the first item of the string (last move) if possible.
 * 
 * @param[in, out] pS The string to modify.
 * @return true The modification was done.
 * @return false The modification was not done (empty string).
 */
bool string_remove_link(string* pS);

/**
 * @brief Add a move in the string.
 * 
 * @param[in, out] pS The string to modify.
 * @param[in] c From where we come from.
 * @return true Modification ok.
 * @return false Internal error. Operation failed.
 */
bool string_add_link(string* pS, compass_t c);

/**
 * @brief Check if the last move was a go back.
 * 
 * @param[in] s The string to check.
 * @return true It was a go back.
 * @return false It was not a go back.
 */
bool string_can_go_back(const string s);

/**
 * @brief Length of the string.
 * 
 * @param[in] s The string to measure.
 * @return size_t Number of link in the string.
 */
size_t string_size(const string s);

#endif  // End of ARIADNE_H
