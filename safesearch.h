/*************************************************************************************************\
*                                                                                                 *
*    F I L E N A M E :   s a f e s e a r c h . h                                                  *
*                                                                                                 *
*    D E S C R I P T I O N :                                                                      *
*        Header definitions for the safety-critical binary search library.                        *
*        Defines return status codes and result structures with strict alignment.                 *
*                                                                                                 *
\*************************************************************************************************/

#ifndef SAFESEARCH_H
#define SAFESEARCH_H

#include <stddef.h>
#include <stdint.h>

/*-----------------------------------------------------------------------------------------------*/
/*    T Y P E   D E F I N I T I O N S                                                            */
/*-----------------------------------------------------------------------------------------------*/

typedef enum
{
    OK_FOUND     = 0,     /* Key found at index                               */
    OK_NOT_FOUND = 1,     /* Key not found; index is insertion point          */
    ERR_NULL_PTR = 2,     /* Input array pointer was NULL (and n > 0)         */
    ERR_BAD_LEN  = 3      /* Invalid length provided (reserved)               */
} status_t;

typedef struct
{
    status_t    status;   /* Result status of the operation                   */
    size_t      index;    /* Found index or insertion point                   */
} search_result_t;

/*-----------------------------------------------------------------------------------------------*/
/*    F U N C T I O N   P R O T O T Y P E S                                                      */
/*-----------------------------------------------------------------------------------------------*/

/*
 * Finds the first position where element >= key.
 */
search_result_t    lower_bound_i32      ( const int32_t   *a,
                                          size_t           n,
                                          int32_t          key );

/*
 * Finds the first position where element > key.
 */
search_result_t    upper_bound_i32      ( const int32_t   *a,
                                          size_t           n,
                                          int32_t          key );

/*
 * Checks for existence of key. Returns index if found, else insertion point.
 */
search_result_t    binary_search_i32    ( const int32_t   *a,
                                          size_t           n,
                                          int32_t          key );

#endif /* SAFESEARCH_H */
