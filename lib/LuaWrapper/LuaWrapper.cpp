#include <LuaWrapper.h>

Lua::Lua(Stream *serial) {
  this->serial = serial;
}

Lua::~Lua() {
  lua_close(L);
}

/**
 * Create a new lua state and setup libraries
 */
void Lua::begin() {
  // Enable printf/sprintf to print floats for Teensy.
  asm(".global _printf_float");
  L = luaL_newstate();
  luaL_openlibs(L);
  lua_settop(L, 0);
}

void Lua::reset() {
  if (L != NULL) lua_close(L);
  begin();
}

/**
 * Check for an error (and log it).
 */
bool Lua::check(int luaHasError) {
  if (luaHasError) {
    #ifndef UNIT_TEST
      errorBegin();
      serial->print(lua_tostring(L, -1));
      errorEnd();
    #endif
    lua_pop(L, 1);
  }
  return luaHasError ? false : true;
}

/**
 * Execute a lua string.
 */
bool Lua::execute(const char *string) {
  return check(luaL_dostring(L, string));
}

/**
 * Execute a lua file.
 */
bool Lua::executeFile(const char* fileName) {
  return check(luaL_dofile(L, fileName));
}

/**
 * Register a (light) function.
 */
void Lua::registerFunction(const char *name, const lua_CFunction function) {
  lua_register(L, name, function);
}

/**
 * Register a set of (light) functions on a lua table.
 */
void Lua::registerLibrary(const char *name, const luaL_reg library[]) {
  luaL_register_light(L, name, library);
}

/**
 * Push a lua function on the stack.
 */
bool Lua::getFunction(const char *name, bool logError) {
  lua_getglobal(L, name);

  if (!lua_isfunction(L, -1) && !lua_islightfunction(L, -1)) {
    if (logError) {
      #ifndef UNIT_TEST
      errorBegin();
      serial->printf(F("Can't find function `%s`."), name);
      errorEnd();
      #endif
    }
    lua_pop(L, 1);
    return false;
  }

  return true;  
}

/**
 * Push a lua function on the stack and log an error if it doesn't exist.
 */
bool Lua::getFunction(const char *name) {
  return getFunction(name, true);
}

/**
 * Push a lua function in a table on the stack.
 */ 
bool Lua::getFunction(const char *table, const char *name, bool logError) {
  lua_getglobal(L, table);
  if (!lua_istable(L, -1)) {
    if (logError) {
      #ifndef UNIT_TEST
      errorBegin();
      serial->printf(F("Can't find table `%s`."), table);
      errorEnd();
      #endif
    }
    lua_pop(L, 1);
    return false;
  }

  lua_getfield(L, -1, name);
  lua_remove(L, -2);

  if (!lua_isfunction(L, -1) && !lua_islightfunction(L, -1)) {
    if (logError) {
      #ifndef UNIT_TEST
      errorBegin();
      serial->printf(F("Can't find function `%s` in table `%s`."), name, table);
      errorEnd();
      #endif
    }
    lua_pop(L, 1);
    return false;
  }

  return true;
}

/**
 * Push a lua function in a table on the stack and log an error if the table or
 * the function doesn't exist. 
 */
bool Lua::getFunction(const char *table, const char *name) {
  return getFunction(table, name, true);
}

void Lua::push(lua_Number number) { lua_pushnumber(L, number); }

void Lua::push(const char *string) { lua_pushstring(L, string); }

bool Lua::call(byte argsCount = 0, byte resultsCount = 0) {
  return check(lua_pcall(L, argsCount, resultsCount, 0));
}

// Compatability functions for serial output and SD file operations:
StdioStream file;

extern "C" {
  void lua_compat_print(const char *string) {
    Serial.print(string);
  }

  int lua_compat_fopen(const char *fileName) {
    return file.fopen(fileName, "r") ? 1 : 0;
  }

  void lua_compat_fclose() {
    file.fclose();
  }

  int lua_compat_feof() {
    return file.feof();
  }

  size_t lua_compat_fread(void* ptr, size_t size, size_t count) {
    return file.fread(ptr, size, count);
  }

  int lua_compat_ferror() {
    return file.ferror();
  }
}