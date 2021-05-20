#ifndef LuaLogLibrary_h
#define LuaLogLibrary_h

#include <LuaWrapper.h>
#include <Logger.h>

namespace LuaLogLibrary {
  Lua *lua;
  Logger *logger;

  int _dump(lua_State *L) {
    const char* text = lua_tostring(L, 1);
    logger->dump(text);
    return 0;
  }

  int error(lua_State *L) {
    const char* text = lua_tostring(L, 1);
    logger->error(text);
    return 0;
  }

  int warning(lua_State *L) {
    const char* text = lua_tostring(L, 1);
    logger->warning(text);
    return 0;
  }

  int info(lua_State *L) {
    const char* text = lua_tostring(L, 1);
    logger->info(text);
    return 0;
  }

  int flush(lua_State *L) {
    logger->flush();
    return 0;
  }

  void install(Lua *lua, Logger *logger) {
    LuaLogLibrary::lua = lua;
    LuaLogLibrary::logger = logger;

    luaL_Reg library[] = {
      { "error", error },
      { "warning", warning },
      { "info", info },
      { "flush", flush },
      { "_dump", _dump },
      { NULL, NULL }
    };

    lua->registerLibrary("Log", library);
  }
}

#endif