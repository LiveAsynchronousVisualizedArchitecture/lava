//==========================================
// LIBCTINY - Matt Pietrek 2001
// MSDN Magazine, January 2001
//==========================================
#include <stdlib.h>
#include <ctype.h>

extern "C" long __cdecl atol ( const char * pstr )
{
    // The current character.
    int  cCurr ;
    // The running total.
    long lTotal ;
    // Holds the '-' sign.
    int  iIsNeg ;

    // Slide past any whitespace.
    while ( isspace ( *pstr ) )
    {
        ++pstr ;
    }

    // Get the current character.
    cCurr = *pstr++ ;
    // Save the negative sign.
    iIsNeg = cCurr ;
    if ( ( '-' == cCurr ) || ( '+' == cCurr ) )
    {
        // We have a sign, so skip it.
        cCurr = *pstr++ ;
    }

    // Initialize the total.
    lTotal = 0 ;

    // While we have digits, addem up.
    while ( isdigit ( cCurr ) )
    {
        // Add this digit to the total.
        lTotal = 10 * lTotal + ( cCurr - '0' ) ;
        // Do the next character.
        cCurr = *pstr++ ;
    }

    // If we have a negative sign, convert the value.
    if ( '-' == iIsNeg )
    {
        return ( -lTotal ) ;
    }
    else
    {
        return ( lTotal ) ;
    }
}

extern "C" int __cdecl atoi ( const char * pstr )
{
    return ( (int)atol ( pstr ) ) ;
}

