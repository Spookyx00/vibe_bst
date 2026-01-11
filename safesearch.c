/*************************************************************************************************\
*                                                                                                 *
*    F I L E N A M E :   s a f e s e a r c h . c                                                  *
*                                                                                                 *
*    D E S C R I P T I O N :                                                                      *
*        Implementation of safety-critical binary search primitives.                              *
*        Adheres to C11 standards with strict bounds checking and overflow safety.                *
*                                                                                                 *
\*************************************************************************************************/

#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>
#include <inttypes.h>
#include <string.h>
#include <limits.h>

#include "safesearch.h"

/*-----------------------------------------------------------------------------------------------*/
/*    I M P L E M E N T A T I O N                                                                */
/*-----------------------------------------------------------------------------------------------*/

/*
 *    F U N C T I O N :   l o w e r _ b o u n d _ i 3 2
 *    ---------------------------------------------------------------------------------------
 *    Finds the first position 'i' in [0, n] where a[i] >= key.
 *    Returns OK_NOT_FOUND (success condition) and the index.
 */
search_result_t    lower_bound_i32      ( const int32_t   *a,
                                          size_t           n,
                                          int32_t          key )
{
    search_result_t    res      = { .status = OK_NOT_FOUND, .index = 0 };
    size_t             first    = 0;
    size_t             len      = n;
    size_t             half;
    size_t             mid;

    /*---------------------------------------------------------------------------*/
    /*    P R E C O N D I T I O N   C H E C K S                                  */
    /*---------------------------------------------------------------------------*/
    if ( a == NULL && n > 0 )
    {
        res.status              = ERR_NULL_PTR;
        return res;
    }

    /*---------------------------------------------------------------------------*/
    /*    S T R I D E - B A S E D   S E A R C H   L O O P                        */
    /*---------------------------------------------------------------------------*/
    while ( len > 0 )
    {
        half                    = len >> 1;
        mid                     = first + half;

        if ( a[mid] < key )
        {
            first               = mid + 1;
            len                 = len - half - 1;
        }
        else
        {
            len                 = half;
        }
    }

    res.index                   = first;
    return res;
}


/*
 *    F U N C T I O N :   u p p e r _ b o u n d _ i 3 2
 *    ---------------------------------------------------------------------------------------
 *    Finds the first position 'i' in [0, n] where a[i] > key.
 */
search_result_t    upper_bound_i32      ( const int32_t   *a,
                                          size_t           n,
                                          int32_t          key )
{
    search_result_t    res      = { .status = OK_NOT_FOUND, .index = 0 };
    size_t             first    = 0;
    size_t             len      = n;
    size_t             half;
    size_t             mid;

    /*---------------------------------------------------------------------------*/
    /*    P R E C O N D I T I O N   C H E C K S                                  */
    /*---------------------------------------------------------------------------*/
    if ( a == NULL && n > 0 )
    {
        res.status              = ERR_NULL_PTR;
        return res;
    }

    /*---------------------------------------------------------------------------*/
    /*    S T R I D E - B A S E D   S E A R C H   L O O P                        */
    /*---------------------------------------------------------------------------*/
    while ( len > 0 )
    {
        half                    = len >> 1;
        mid                     = first + half;

        if ( a[mid] <= key )
        {
            first               = mid + 1;
            len                 = len - half - 1;
        }
        else
        {
            len                 = half;
        }
    }

    res.index                   = first;
    return res;
}


/*
 *    F U N C T I O N :   b i n a r y _ s e a r c h _ i 3 2
 *    ---------------------------------------------------------------------------------------
 *    Wraps lower_bound to determine equality.
 */
search_result_t    binary_search_i32    ( const int32_t   *a,
                                          size_t           n,
                                          int32_t          key )
{
    /* Use lower_bound to find the candidate insertion point */
    search_result_t    lb       = lower_bound_i32( a, n, key );
    bool               match    = false;

    /* Propagate errors immediately */
    if ( lb.status != OK_NOT_FOUND && lb.status != OK_FOUND )
    {
        return lb;
    }

    /*---------------------------------------------------------------------------*/
    /*    V E R I F Y   M A T C H                                                */
    /*---------------------------------------------------------------------------*/
    if ( lb.index < n && a != NULL )
    {
        if ( a[lb.index] == key )
        {
            match               = true;
        }
    }

    /* Update status based on findings */
    if ( match )
    {
        lb.status               = OK_FOUND;
    }
    else
    {
        lb.status               = OK_NOT_FOUND;
    }

    return lb;
}


/*===============================================================================================*/
/*                                                                                               */
/*    T E S T   H A R N E S S                                                                    */
/*                                                                                               */
/*===============================================================================================*/

/*-----------------------------------------------------------------------------------------------*/
/*    H E L P E R S                                                                              */
/*-----------------------------------------------------------------------------------------------*/

static uint64_t    prng_state   = 0xCAFEBABE;

static void    seed_prng            ( uint64_t         seed )
{
    prng_state                  = seed;
}

static uint32_t    next_u32         ( void )
{
    prng_state                  = prng_state * 6364136223846793005ULL + 1442695040888963407ULL;
    return (uint32_t)(prng_state >> 32);
}

static int32_t     random_i32_range ( int32_t          min,
                                      int32_t          max )
{
    uint32_t           range    = (uint32_t)((int64_t)max - min + 1);
    return min + (int32_t)(next_u32() % range);
}

static int         compare_i32      ( const void      *a,
                                      const void      *b )
{
    int32_t            arg1     = *(const int32_t*)a;
    int32_t            arg2     = *(const int32_t*)b;
    
    if ( arg1 < arg2 ) return -1;
    if ( arg1 > arg2 ) return 1;
    return 0;
}

static size_t      linear_lower_bound ( const int32_t   *a,
                                        size_t           n,
                                        int32_t          key )
{
    for ( size_t i = 0; i < n; ++i )
    {
        if ( a[i] >= key ) return i;
    }
    return n;
}

static size_t      linear_upper_bound ( const int32_t   *a,
                                        size_t           n,
                                        int32_t          key )
{
    for ( size_t i = 0; i < n; ++i )
    {
        if ( a[i] > key ) return i;
    }
    return n;
}

static int         tests_run        = 0;
static int         tests_passed     = 0;

static void        check            ( bool             condition,
                                      const char      *msg )
{
    tests_run++;
    if ( condition )
    {
        tests_passed++;
    }
    else
    {
        fprintf( stderr, "FAIL: %s\n", msg );
        exit( 1 );
    }
}


/*-----------------------------------------------------------------------------------------------*/
/*    T E S T   C A S E S                                                                        */
/*-----------------------------------------------------------------------------------------------*/

static void        test_fixed_vectors ( void )
{
    printf( "Running Fixed Vector Tests...\n" );

    /* 1. Empty array */
    {
        int32_t            empty[]  = {0};
        search_result_t    res;

        res                         = lower_bound_i32( empty, 0, 5 );
        check( res.status == OK_NOT_FOUND,  "Empty array LB status" );
        check( res.index  == 0,             "Empty array LB index" );

        res                         = upper_bound_i32( empty, 0, 5 );
        check( res.status == OK_NOT_FOUND,  "Empty array UB status" );
        check( res.index  == 0,             "Empty array UB index" );

        res                         = binary_search_i32( empty, 0, 5 );
        check( res.status == OK_NOT_FOUND,  "Empty array BS status" );
        check( res.index  == 0,             "Empty array BS index" );
    }

    /* 2. Single element */
    {
        int32_t            arr[]    = {10};
        search_result_t    res;

        /* Key < Element */
        res                         = lower_bound_i32( arr, 1, 5 );
        check( res.index == 0,              "Single element, key < elem, LB" );
        
        res                         = upper_bound_i32( arr, 1, 5 );
        check( res.index == 0,              "Single element, key < elem, UB" );

        /* Key == Element */
        res                         = lower_bound_i32( arr, 1, 10 );
        check( res.index == 0,              "Single element, key == elem, LB" );

        res                         = upper_bound_i32( arr, 1, 10 );
        check( res.index == 1,              "Single element, key == elem, UB" );

        res                         = binary_search_i32( arr, 1, 10 );
        check( res.status == OK_FOUND && res.index == 0, "Single element, key == elem, BS" );

        /* Key > Element */
        res                         = lower_bound_i32( arr, 1, 15 );
        check( res.index == 1,              "Single element, key > elem, LB" );

        res                         = upper_bound_i32( arr, 1, 15 );
        check( res.index == 1,              "Single element, key > elem, UB" );

        res                         = binary_search_i32( arr, 1, 15 );
        check( res.status == OK_NOT_FOUND && res.index == 1, "Single element, key > elem, BS" );
    }

    /* 3. All elements equal */
    {
        int32_t            arr[]    = {5, 5, 5, 5};
        search_result_t    res;

        res                         = lower_bound_i32( arr, 4, 5 );
        check( res.index == 0,              "All equal LB" );

        res                         = upper_bound_i32( arr, 4, 5 );
        check( res.index == 4,              "All equal UB" );

        res                         = binary_search_i32( arr, 4, 5 );
        check( res.status == OK_FOUND && res.index == 0, "All equal BS" );
    }

    /* 4. Extreme values */
    {
        int32_t            arr[]    = {INT32_MIN, 0, INT32_MAX};
        search_result_t    res;

        res                         = lower_bound_i32( arr, 3, INT32_MIN );
        check( res.index == 0,              "Min int LB" );
        
        res                         = upper_bound_i32( arr, 3, INT32_MIN );
        check( res.index == 1,              "Min int UB" );

        res                         = lower_bound_i32( arr, 3, INT32_MAX );
        check( res.index == 2,              "Max int LB" );
        
        res                         = upper_bound_i32( arr, 3, INT32_MAX );
        check( res.index == 3,              "Max int UB" );
    }

    /* 5. NULL handling */
    {
        search_result_t    res;

        res                         = lower_bound_i32( NULL, 10, 5 );
        check( res.status == ERR_NULL_PTR,  "NULL ptr with size > 0" );
        
        res                         = lower_bound_i32( NULL, 0, 5 );
        check( res.status == OK_NOT_FOUND,  "NULL ptr with size 0 -> OK" );
        check( res.index  == 0,             "NULL ptr with size 0 -> index 0" );
    }
}

static void        test_property_based ( void )
{
    const int          iterations   = 5000;
    const int          max_size     = 200;
    int32_t           *buffer;
    
    printf( "Running Property-Based Tests...\n" );
    seed_prng( 123456789 );

    buffer                          = (int32_t*)malloc( max_size * sizeof(int32_t) );
    if ( !buffer )
    {
        perror("malloc");
        exit(1);
    }

    for ( int it = 0; it < iterations; ++it )
    {
        /* Generate random array */
        size_t         n            = next_u32() % ( max_size + 1 );
        int32_t        val_range    = ( next_u32() % 2 == 0 ) ? 20 : 100000;
        
        for ( size_t i = 0; i < n; ++i )
        {
            buffer[i]               = random_i32_range( -val_range, val_range );
        }

        qsort( buffer, n, sizeof(int32_t), compare_i32 );

        /* Generate random keys */
        int            key_trials   = 20;
        
        for ( int k = 0; k < key_trials; ++k )
        {
            int32_t        key;
            
            if ( n > 0 && ( next_u32() % 2 == 0 ) )
            {
                key                 = buffer[ next_u32() % n ];
            }
            else
            {
                key                 = random_i32_range( -val_range - 10, val_range + 10 );
            }

            /* Oracle checks */
            size_t             oracle_lb    = linear_lower_bound( buffer, n, key );
            size_t             oracle_ub    = linear_upper_bound( buffer, n, key );

            /* Implementation checks */
            search_result_t    impl_lb      = lower_bound_i32( buffer, n, key );
            search_result_t    impl_ub      = upper_bound_i32( buffer, n, key );
            search_result_t    impl_bs      = binary_search_i32( buffer, n, key );

            /* Verify LB */
            check( impl_lb.status == OK_NOT_FOUND, "LB Property status" );
            if ( impl_lb.index != oracle_lb )
            {
                fprintf( stderr, "LB mismatch: size=%zu key=%d | exp %zu got %zu\n",
                         n, key, oracle_lb, impl_lb.index );
                exit( 1 );
            }

            /* Verify UB */
            check( impl_ub.status == OK_NOT_FOUND, "UB Property status" );
            if ( impl_ub.index != oracle_ub )
            {
                fprintf( stderr, "UB mismatch: size=%zu key=%d | exp %zu got %zu\n",
                         n, key, oracle_ub, impl_ub.index );
                exit( 1 );
            }

            /* Verify BS */
            bool           key_present = ( oracle_lb < n && buffer[oracle_lb] == key );
            
            if ( key_present )
            {
                if ( impl_bs.status != OK_FOUND )
                {
                    fprintf( stderr, "BS failed to find key: size=%zu key=%d\n", n, key );
                    exit( 1 );
                }
                if ( impl_bs.index != oracle_lb )
                { 
                    fprintf( stderr, "BS found wrong index: size=%zu key=%d | exp %zu got %zu\n",
                             n, key, oracle_lb, impl_bs.index );
                    exit( 1 );
                }
            }
            else
            {
                if ( impl_bs.status != OK_NOT_FOUND )
                {
                    fprintf( stderr, "BS found non-existent key: size=%zu key=%d\n", n, key );
                    exit( 1 );
                }
                if ( impl_bs.index != oracle_lb )
                {
                     fprintf( stderr, "BS wrong insertion point: size=%zu key=%d | exp %zu got %zu\n",
                             n, key, oracle_lb, impl_bs.index );
                    exit( 1 );                   
                }
            }
        }
    }

    free( buffer );
}


/*-----------------------------------------------------------------------------------------------*/
/*    E N T R Y   P O I N T                                                                      */
/*-----------------------------------------------------------------------------------------------*/

#ifndef SAFE_SEARCH_LIB

int        main             ( void )
{
    test_fixed_vectors();
    test_property_based();

    printf( "\nSUMMARY: PASS\n" );
    printf( "Tests Run:    %d\n", tests_run );
    printf( "Tests Passed: %d\n", tests_passed );
    
    return 0;
}

#endif
