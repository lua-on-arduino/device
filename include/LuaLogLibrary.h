#ifndef LuaLogLibrary_h
#define LuaLogLibrary_h

#include <Logger.h>
#include <LuaWrapper.h>

namespace LuaLogLibrary {
Lua *lua;
Logger *logger;

int _log(lua_State *L) {
  auto type = static_cast<Logger::LogType>(lua_tointeger(L, -2) - 1);
  const char *text = lua_tostring(L, -1);
  logger->log(type, text);
  return 0;
}

int _flush(lua_State *L) {
  logger->flush();
  return 0;
}

void install(Lua *lua, Logger *logger) {
  LuaLogLibrary::lua = lua;
  LuaLogLibrary::logger = logger;

  luaL_Reg library[] = {{"_log", _log}, {"_flush", _flush}, {NULL, NULL}};

  lua->registerLibrary("Log", library);
}
} // namespace LuaLogLibrary

#endif