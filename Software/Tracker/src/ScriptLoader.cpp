#include "ScriptLoader.h"
#include "CoreObj.h"
#include <lua.hpp>
#include <stdio.h>
#include <string>

static const char* const initScript = R"(
  local ffi = require("ffi")
  ffi.cdef[[
    typedef struct { float x, y; } vec2;
    typedef struct { float x, y, z; } vec3;
    typedef struct {
      vec2 kinectPosition;
      vec3 metricPosition;
      float weight;
    } Detection;
    typedef struct {
      double timestamp;
      int32_t numDetections;
      Detection detections[16];
    } World;
    typedef struct {
      vec2 camera;
    } ControlState;
  ]]

  context = {}

  function decide(dt, world, state)
    world = ffi.cast("World*", world)
    state = ffi.cast("ControlState*", state)
    if user_decide ~= nil then
      local ok, new_state = pcall(user_decide, dt, world, state)
      if ok then
        return new_state
      else
        io.write("error calling user code: ", new_state, "\n")
      end
    end
  end
)";

ScriptLoader::~ScriptLoader() { lua_close(lua); }

bool ScriptLoaderInit(ScriptLoader* loader) {
  loader->lua = luaL_newstate();
  luaL_openlibs(loader->lua);

  if (luaL_dostring(loader->lua, initScript)) {
    printf("failed to load wrapper script: %s\n",
           lua_tostring(loader->lua, -1));
    return false;
  }

  return true;
}

const char* ScriptLoaderUpdate(ScriptLoader* loader, double dt, World* world,
                               ControlState* state) {
  lua_State* L = loader->lua;
  lua_getglobal(L, "decide");
  lua_pushnumber(L, dt);
  lua_pushlightuserdata(L, world);
  lua_pushlightuserdata(L, state);
  if (lua_pcall(L, 3, 1, 0) != 0) {
    return lua_tostring(L, -1);
  }

  return nullptr;
}

bool ScriptLoaderExecFile(ScriptLoader* loader, const char* file) {
  FILE* f = fopen(file, "rb");

  if (!f) {
    return false;
  }

  fseek(f, 0, SEEK_END);
  int64_t size = ftell(f);
  fseek(f, 0, SEEK_SET);

  std::string content(size, 0);
  fread(&content[0], 1, size, f);
  fclose(f);

  return ScriptLoaderExec(loader, content.c_str());
}

bool ScriptLoaderExec(ScriptLoader* loader, const char* script) {
  int res = luaL_dostring(loader->lua, script);
  return !bool(res);
}

const char* ScriptLoaderGetError(ScriptLoader* loader) {
  return lua_tostring(loader->lua, -1); 
}
