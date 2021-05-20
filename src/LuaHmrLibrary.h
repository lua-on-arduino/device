#ifndef LuaHmrLibrary_h
#define LuaHmrLibrary_h

#include <LuaWrapper.h>

namespace LuaHmrLibrary {
  Lua *lua;

  const char* updateFile(const char *modulePath) {
    if (strcmp(modulePath, "lua/loa_firmware/init.lua") == 0) {
      lua->runFile(modulePath);
      return "reload";
    } else if (lua->getFunction("HMR", "update")) {
      lua->push(modulePath);
      lua->call(1, 1);

      const char* updateType = lua_tostring(lua->L, -1);
      lua_pop(lua->L, 1);

      return updateType;
    } else {
      return "error";
    }
  }

  void install(Lua *lua) {
    LuaHmrLibrary::lua = lua;
  }
}

#endif