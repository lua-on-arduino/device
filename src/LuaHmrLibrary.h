#ifndef LuaHmrLibrary_h
#define LuaHmrLibrary_h

#include <LuaWrapper.h>

namespace LuaHmrLibrary {
  Lua *lua;

  bool updateFile(const char *modulePath) {
    const char *entry = "lua/init.lua";
    bool moduleIsEntry = strcmp(modulePath, entry) == 0;
    bool usedHmr = false;

    if (!moduleIsEntry && lua->getFunction("HMR", "update")) {
      lua->push(modulePath);
      lua->call(1, 1);

      usedHmr = lua_toboolean(lua->L, -1);
      lua_pop(lua->L, 1);
    }

    return usedHmr;
  }

  void install(Lua *lua) {
    LuaHmrLibrary::lua = lua;
  }
}

#endif