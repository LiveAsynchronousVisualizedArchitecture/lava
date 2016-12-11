//#include <limits.h>
#include "stdarg.h"
//#include <stdio.h>
 
#define UINT_MAX 4294967295U
/* Using an integer's bits as flags for both the conversion flags and length
   modifiers.
*/
#define E_suppressed 1<<0
#define E_char       1<<6
#define E_short      1<<7
#define E_long       1<<8
#define E_llong      1<<9
#define E_intmax     1<<10
#define E_size       1<<11
#define E_ptrdiff    1<<12
#define E_intptr     1<<13
#define E_ldouble    1<<14
#define E_unsigned   1<<16
#define E_float      1<<17
#define E_double     1<<18
 
#define SIZE_MAX 500
typedef unsigned int uintmax_t; 
typedef int intmax_t;
typedef unsigned int size_t;
typedef int ptrdiff_t;
 
#define EOF -1
#define NULL 0
#define bool int
#define true 1
#define false 0
 
char _PDCLIB_digits[] = "0123456789abcdefghijklmnopqrstuvwxyz.";
 
int isspace(int c) { 
  if(c == ' ' ) return true;
  if(c == '\t') return true;
 
  return false;
}
 
/* Status structure required by _PDCLIB_print(). */
struct _PDCLIB_status_t
{
    /* XXX This structure is horrible now. scanf needs its own */
 
    int              base;   /* base to which the value shall be converted   */
    unsigned int flags; /* flags and length modifiers                */
    unsigned         n;      /* print: maximum characters to be written (snprintf) */
                             /* scan:  number matched conversion specifiers  */
    unsigned         i;      /* number of characters read/written            */
    unsigned         current;/* chars read/written in the CURRENT conversion */
    unsigned         width;  /* specified field width                        */
    int              prec;   /* specified field precision                    */
 
    const char *     s;      /* input string for scanf */
 
    va_list  arg;    /* argument stack                               */
};
 
 
void * nmemchr( const void * s, int c, size_t n ) {
    const unsigned char * p = (const unsigned char *) s;
    while ( n-- )
    {
        if ( *p == (unsigned char) c )
        {
            return (void *) p;
        }
        ++p;
    }
    return NULL;
}
 
 
/* Helper function to get a character from the string or stream, whatever is
   used for input. When reading from a string, returns EOF on end-of-string
   so that handling of the return value can be uniform for both streams and
   strings.
*/
static int GET( struct _PDCLIB_status_t * status )
{
    int rc = EOF;
    rc = ( *status->s == '\0' ) ? EOF : (unsigned char)*((status->s)++);
    if ( rc != EOF )
    {
        ++(status->i);
        ++(status->current);
    }
    return rc;
}
 
 
/* Helper function to put a read character back into the string or stream,
   whatever is used for input.
*/
static void UNGET( int c, struct _PDCLIB_status_t * status )
{
    --(status->s);
    --(status->i);
    --(status->current);
}
 
 
/* Helper function to check if a character is part of a given scanset */
static bool IN_SCANSET( const char * scanlist, const char * end_scanlist, int rc )
{
    // SOLAR
    int previous = -1;
    while ( scanlist != end_scanlist )
    {
        if ( ( *scanlist == '-' ) && ( previous != -1 ) )
        {
            /* possible scangroup ("a-z") */
            if ( ++scanlist == end_scanlist )
            {
                /* '-' at end of scanlist does not describe a scangroup */
                return rc == '-';
            }
            while ( ++previous <= (unsigned char)*scanlist )
            {
                if ( previous == rc )
                {
                    return true;
                }
            }
            previous = -1;
        }
        else
        {
            /* not a scangroup, check verbatim */
            if ( rc == (unsigned char)*scanlist )
            {
                return true;
            }
            previous = (unsigned char)(*scanlist++);
        }
    }
    return false;
}
 
 
const char * _PDCLIB_scan(const char * spec, struct _PDCLIB_status_t * status )
{
    /* generic input character */
    int rc;
    const char * orig_spec = spec;
    if ( *(++spec) == '%' )
    {
        /* %% -> match single '%' */
        rc = GET( status );
        switch ( rc )
        {
            case EOF:
                /* input error */
                if ( status->n == 0 )
                {
                    status->n = -1;
                }
                return NULL;
            case '%':
                return ++spec;
            default:
                UNGET( rc, status );
                break;
        }
    }
    /* Initializing status structure */
    status->flags = 0;
    status->base = -1;
    status->current = 0;
    status->width = 0;
    status->prec = 0;
 
    /* '*' suppresses assigning parsed value to variable */
    if ( *spec == '*' )
    {
        status->flags |= E_suppressed;
        ++spec;
    }
 
    /* If a width is given, strtol() will return its value. If not given,
       strtol() will return zero. In both cases, endptr will point to the
       rest of the conversion specifier - just what we need.
    */
    char const * prev_spec = spec;
    status->width = (int)strtol( spec, (char**)&spec, 10 );
    if ( spec == prev_spec )
    {
        status->width = UINT_MAX;
    }
 
    /* Optional length modifier
       We step one character ahead in any case, and step back only if we find
       there has been no length modifier (or step ahead another character if it
       has been "hh" or "ll").
    */
    switch ( *(spec++) )
    {
        case 'h':
            if ( *spec == 'h' )
            {
                /* hh -> char */
                status->flags |= E_char;
                ++spec;
            }
            else
            {
                /* h -> short */
                status->flags |= E_short;
            }
            break;
        case 'l':
            if ( *spec == 'l' )
            {
                /* ll -> long long */
                status->flags |= E_llong;
                ++spec;
            }
            else
            {
                /* l -> long */
                status->flags |= E_long;
            }
            break;
        case 'j':
            /* j -> intmax_t, which might or might not be long long */
            status->flags |= E_intmax;
            break;
        case 'z':
            /* z -> size_t, which might or might not be unsigned int */
            status->flags |= E_size;
            break;
        case 't':
            /* t -> ptrdiff_t, which might or might not be long */
            status->flags |= E_ptrdiff;
            break;
        case 'L':
            /* L -> long double */
            status->flags |= E_ldouble;
            break;
        default:
            --spec;
            break;
    }
 
    /* Conversion specifier */
 
    /* whether valid input had been parsed */
    bool value_parsed = false;
 
    switch ( *spec )
    {
        case 'd':
            status->base = 10;
            break;
        case 'i':
            status->base = 0;
            break;
        case 'o':
            status->base = 8;
            status->flags |= E_unsigned;
            break;
        case 'u':
            status->base = 10;
            status->flags |= E_unsigned;
            break;
        case 'x':
            status->base = 16;
            status->flags |= E_unsigned;
            break;
        case 'f':
            status->flags |= E_float;
            status->base = 10;
            break;
        case 'F':
        case 'e':
        case 'E':
        case 'g':
        case 'G':
        case 'a':
        case 'A':
            break;
        case 'c':
        {
            char * c = va_arg( status->arg, char * );
            /* for %c, default width is one */
            //if ( status->width == SIZE_MAX )
            if ( status->width == -1 )
            {
                status->width = 1;
            }
            /* reading until width reached or input exhausted */
            while ( ( status->current < status->width ) &&
                    ( ( rc = GET( status ) ) != EOF ) )
            {
                *(c++) = rc;
                value_parsed = true;
            }
            /* width or input exhausted */
            if ( value_parsed )
            {
                ++status->n;
                return ++spec;
            }
            else
            {
                /* input error, no character read */
                if ( status->n == 0 )
                {
                    status->n = -1;
                }
                return NULL;
            }
        }
        case 's':
        {
            char * c = va_arg( status->arg, char * );
            while ( ( status->current < status->width ) &&
                    ( ( rc = GET( status ) ) != EOF ) )
            {
                if ( isspace( rc ) )
                {
                    UNGET( rc, status );
                    if ( value_parsed )
                    {
                        /* matching sequence terminated by whitespace */
                        *c = '\0';
                        ++status->n;
                        return ++spec;
                    }
                    else
                    {
                        /* matching error */
                        return NULL;
                    }
                }
                else
                {
                    /* match */
                    value_parsed = true;
                    *(c++) = rc;
                }
            }
            /* width or input exhausted */
            if ( value_parsed )
            {
                *c = '\0';
                ++status->n;
                return ++spec;
            }
            else
            {
                /* input error, no character read */
                if ( status->n == 0 )
                {
                    status->n = -1;
                }
                return NULL;
            }
        }
        case '[':
        {
            const char * endspec = spec;
            bool negative_scanlist = false;
            if ( *(++endspec) == '^' )
            {
                negative_scanlist = true;
                ++endspec;
            }
            spec = endspec;
            do
            {
                // TODO: This can run beyond a malformed format string
                ++endspec;
            } while ( *endspec != ']' );
            // read according to scanlist, equiv. to %s above
            char * c = va_arg( status->arg, char * );
            while ( ( status->current < status->width ) &&
                    ( ( rc = GET( status ) ) != EOF ) )
            {
                if ( negative_scanlist )
                {
                    if ( IN_SCANSET( spec, endspec, rc ) )
                    {
                        UNGET( rc, status );
                        break;
                    }
                }
                else
                {
                    if ( ! IN_SCANSET( spec, endspec, rc ) )
                    {
                        UNGET( rc, status );
                        break;
                    }
                }
                value_parsed = true;
                *(c++) = rc;
            }
            if ( value_parsed )
            {
                *c = '\0';
                ++status->n;
                return ++endspec;
            }
            else
            {
                if ( rc == EOF )
                {
                    status->n = -1;
                }
                return NULL;
            }
        }
        case 'p':
            status->base = 16;
            // TODO: Like _PDCLIB_print, E_pointer(?)
            status->flags |= E_unsigned | E_long;
            break;
        case 'n':
        {
            int * val = va_arg( status->arg, int * );
            *val = status->i;
            return ++spec;
        }
        default:
            /* No conversion specifier. Bad conversion. */
            return orig_spec;
    }
 
    if ( status->base != -1 )
    {
        int dp_count=0;
        bool dp_seen = false;
        /* integer conversion */
        uintmax_t value   = 0;         /* absolute value read */
        double    value_f = 0;         /* absolute value read */
        bool prefix_parsed = false;
        int sign = 0;
        while ( ( status->current < status->width ) &&
                ( ( rc = GET( status ) ) != EOF ) )
        {
            if ( isspace( rc ) )
            {
                if ( sign )
                {
                    /* matching sequence terminated by whitespace */
                    UNGET( rc, status );
                    break;
                }
                else
                {
                    /* leading whitespace not counted against width */
                    status->current--;
                }
            }
            else if ( ! sign )
            {
                /* no sign parsed yet */
                switch ( rc )
                {
                    case '-':
                        sign = -1;
                        break;
                    case '+':
                        sign = 1;
                        break;
                    default:
                        /* not a sign; put back character */
                        sign = 1;
                        UNGET( rc, status );
                        break;
                }
            }
            else if ( ! prefix_parsed )
            {
                /* no prefix (0x... for hex, 0... for octal) parsed yet */
                prefix_parsed = true;
                if ( rc != '0' )
                {
                    /* not a prefix; if base not yet set, set to decimal */
                    if ( status->base == 0 )
                    {
                        status->base = 10;
                    }
                    UNGET( rc, status );
                }
                else
                {
                    /* starts with zero, so it might be a prefix. */
                    /* check what follows next (might be 0x...) */
                    if ( ( status->current < status->width ) &&
                         ( ( rc = GET( status ) ) != EOF ) )
                    {
                        if ( tolower( rc ) == 'x' )
                        {
                            /* 0x... would be prefix for hex base... */
                            if ( ( status->base == 0 ) ||
                                 ( status->base == 16 ) )
                            {
                                status->base = 16;
                            }
                            else
                            {
                                /* ...unless already set to other value */
                                UNGET( rc, status );
                                value_parsed = true;
                            }
                        }
                        else
                        {
                            /* 0... but not 0x.... would be octal prefix */
                            UNGET( rc, status );
                            if ( status->base == 0 )
                            {
                                status->base = 8;
                            }
                            /* in any case we have read a zero */
                            value_parsed = true;
                        }
                    }
                    else
                    {
                        /* failed to read beyond the initial zero */
                        value_parsed = true;
                        break;
                    }
                }
            }
            else
            {
 
                char * digitptr = nmemchr( _PDCLIB_digits, tolower( rc ), status->base );
                if(rc != '.') {
                  if ( digitptr == NULL ) {
                      /* end of input item */
                      UNGET( rc, status );
                      break;
                  } else {
                    value *= status->base;
                    value += digitptr - _PDCLIB_digits;
 
                    value_f *= status->base;
                    value_f += digitptr - _PDCLIB_digits;
                  }
                } else {
                  dp_seen = true;
                }
                if(dp_seen) dp_count++;
                value_parsed = true;
            }
        }
        /* width or input exhausted, or non-matching character */
        if ( ! value_parsed )
        {
            /* out of input before anything could be parsed - input error */
            /* FIXME: if first character does not match, value_parsed is not set - but it is NOT an input error */
            if ( ( status->n == 0 ) && ( rc == EOF ) )
            {
                status->n = -1;
            }
            return NULL;
        }
        /* convert value to target type and assign to parameter */
        if ( ! ( status->flags & E_suppressed ) )
        {
            switch ( status->flags & ( E_char | E_short | E_long | E_llong |
                                       E_intmax | E_size | E_ptrdiff | E_float |
                                       E_unsigned ) )
            {
                case E_char:
                    *( va_arg( status->arg,               char * ) ) =               (char)( value * sign );
                    break;
                case E_char | E_unsigned:
                    *( va_arg( status->arg,      unsigned char * ) ) =      (unsigned char)( value * sign );
                    break;
 
                case E_short:
                    *( va_arg( status->arg,              short * ) ) =              (short)( value * sign );
                    break;
                case E_short | E_unsigned:
                    *( va_arg( status->arg,     unsigned short * ) ) =     (unsigned short)( value * sign );
                    break;
 
                case 0:
                    *( va_arg( status->arg,                int * ) ) =                (int)( value * sign );
                    break;
                case E_unsigned:
                    *( va_arg( status->arg,       unsigned int * ) ) =       (unsigned int)( value * sign );
                    break;
 
                case E_long:
                    *( va_arg( status->arg,               long * ) ) =               (long)( value * sign );
                    break;
                case E_long | E_unsigned:
                    *( va_arg( status->arg,      unsigned long * ) ) =      (unsigned long)( value * sign );
                    break;
 
                case E_llong:
                    *( va_arg( status->arg,          long long * ) ) =          (long long)( value * sign );
                    break;
                case E_llong | E_unsigned:
                    *( va_arg( status->arg, unsigned long long * ) ) = (unsigned long long)( value * sign );
                    break;
 
                case E_intmax:
                    *( va_arg( status->arg,           intmax_t * ) ) =           (intmax_t)( value * sign );
                    break;
                case E_float:
                    {
                      int n;
                      for(n=1;n<dp_count;n++) value_f = value_f/10;
                      *( va_arg( status->arg,           float * ) ) =           (float)( (value_f * sign) );
                    }
                    break;
                case E_intmax | E_unsigned:
                    *( va_arg( status->arg,          uintmax_t * ) ) =          (uintmax_t)( value * sign );
                    break;
 
                case E_size:
                    /* E_size always implies unsigned */
                    *( va_arg( status->arg,             size_t * ) ) =             (size_t)( value * sign );
                    break;
 
                case E_ptrdiff:
                    /* E_ptrdiff always implies signed */
                    *( va_arg( status->arg,          ptrdiff_t * ) ) =          (ptrdiff_t)( value * sign );
                    break;
 
                default:
                    //puts( "UNSUPPORTED SCANF FLAG COMBINATION" );
                    return NULL; /* behaviour unspecified */
            }
            ++(status->n);
        }
        return ++spec;
    }
    /* TODO: Floats. */
    return NULL;
}
 
int nsscanf( const char *s, const char *format, ... ) {
    /* TODO: This function should interpret format as multibyte characters.  */
    struct _PDCLIB_status_t status;
    status.base = 0;
    status.flags = 0;
    status.n = 0;
    status.i = 0;
    status.current = 0;
    status.s = (char *) s;
    status.width = 0;
    status.prec = 0;
 
    va_list ap;
    va_start(ap,format);
    va_copy( status.arg, ap );
 
    while ( *format != '\0' )
    {
        const char * rc;
        if ( ( *format != '%' ) || ( ( rc = _PDCLIB_scan( format, &status ) ) == format ) )
        {
            /* No conversion specifier, match verbatim */
            if ( isspace( *format ) )
            {
                /* Whitespace char in format string: Skip all whitespaces */
                /* No whitespaces in input do not result in matching error */
                while ( isspace( *status.s ) )
                {
                    ++status.s;
                    ++status.i;
                }
            }
            else
            {
                /* Non-whitespace char in format string: Match verbatim */
                if ( *status.s != *format )
                {
                    if ( *status.s == '\0' && status.n == 0 )
                    {
                        /* Early input error */
                        return EOF;
                    }
                    /* Matching error */
                    return status.n;
                }
                else
                {
                    ++status.s;
                    ++status.i;
                }
            }
            ++format;
        }
        else
        {
            /* NULL return code indicates error */
            if ( rc == NULL )
            {
                if ( ( *status.s == '\n' ) && ( status.n == 0 ) )
                {
                    status.n = EOF;
                }
                break;
            }
            /* Continue parsing after conversion specifier */
            format = rc;
        }
    }
    va_end( status.arg );
    return status.n;
}


