//
//  luawithlib.h
//  luawithlib
//
//  Created by yingkun zhang on 2019/1/19.
//  Copyright © 2019 yingkun zhang. All rights reserved.
//

#ifndef luawithlib_h
#define luawithlib_h

#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

/*-------------------------------------------------------------------------*\
 * This macro prefixes all exported API functions
 \*-------------------------------------------------------------------------*/
#ifndef LUAWITHLIB_API
#define LUAWITHLIB_API LUA_API
#endif

/*-------------------------------------------------------------------------*\
 * Initializes the lua socket library.
 \*-------------------------------------------------------------------------*/
LUAWITHLIB_API int luaopen_socket_core(lua_State *L);

#endif // luawithlib_h
