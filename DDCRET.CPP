/*==========================================================================

	 ddcret.cpp  -  Don Cross, February 1993.

	 Returns a string for any given DDCRET value.

Revision history:

1993 February 16 [Don Cross]
	 Initial revision.  Contains 'const char *DDCRET_String(DDCRET)'.

==========================================================================*/

#include <stdio.h>
#include "ddc.h"

const char *DDCRET_String ( DDCRET retcode )
{
   switch ( retcode )
   {
	  case DDC_SUCCESS:          return "DDC_SUCCESS";
	  case DDC_FAILURE:          return "DDC_FAILURE";
	  case DDC_OUT_OF_MEMORY:    return "DDC_OUT_OF_MEMORY";
	  case DDC_FILE_ERROR:       return "DDC_FILE_ERROR";
	  case DDC_INVALID_CALL:     return "DDC_INVALID_CALL";
	  case DDC_USER_ABORT:       return "DDC_USER_ABORT";
	  case DDC_INVALID_FILE:     return "DDC_INVALID_FILE";
   }

   // If we get here, it is not a known retcode.

   static char buffer [80];

   sprintf ( buffer, "DDCRET(%d)", int(retcode) );

   return buffer;
}

/*--- end of file ddcret.cpp ---*/