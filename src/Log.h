#pragma once 
#include <cstdio>
/*usage:
* #define LOG
* #include "Log.h"
*
*/


#define ERROR
//------------don't modify anything below--------------------------

#define __COL_END__ "\033[0m"
#define __COL_RED__ "\033[1;31m"
#define __COL_GREEN__ "\033[1;32m"
#define __COL_BLUE__ "\033[1;34m"
#define __COL_YELLOW__ "\033[1;33m"


#if defined(LOG) || defined(DEBUG) || defined(INFO) || defined(WARN) || defined(ERROR)
	#define _Log(pre, fmt, args...) printf("%s (%s:%d %s) "fmt" \n", pre, __FILE__, __LINE__, __FUNCTION__,##args)
#else
	#define _Log(pre, fmt, args...) NULL
#endif

#if not defined(INFO) && not defined(WARN) && not defined(ERROR)
	#define debug(fmt, args...) _Log(__COL_BLUE__"[DEBUG]", fmt __COL_END__, ##args)
#else 
	#define debug(fmt, args...) NULL
#endif

#if not defined(WARN) && not defined(ERROR)
	#define info(fmt, args...) _Log(__COL_GREEN__"[INFO]", fmt __COL_END__, ##args)
#else
	#define info(fmt, args...) NULL
#endif

#if not defined(ERROR)
	#define warn(fmt, args...) _Log(__COL_YELLOW__"[WARN]", fmt __COL_END__, ##args)
#else
	#define warn(fmt, args...) NULL
#endif

#define err(fmt, args...) _Log(__COL_RED__"[ERROR]", fmt __COL_END__, ##args)

#define _D(fmt, args...) debug(fmt, ##args)
#define _I(fmt, args...) info(fmt, ##args)
#define _W(fmt, args...) warn(fmt, ##args)
#define _E(fmt, args...) err(fmt, ##args)

#define _F_IN_() debug("in")
#define _F_OUT_() debug("out")
