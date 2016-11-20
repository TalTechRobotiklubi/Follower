#include "ScriptLoader.h"
#include "CoreObj.h"
#include <lua.hpp>
#include <stdio.h>
#include <string>

static int RemoteLog(lua_State* L) {
	ScriptLoader* s = (ScriptLoader*)lua_topointer(L, lua_upvalueindex(1));
	const char* text = lua_tostring(L, -1);

	if (text && s->log) {
		s->log(text, s->user);
	}
	
	lua_pop(L, 1);
	return 0;
}

static const char* const initScript = R"(
  package.path = package.path .. ";./scripts/?.lua"
	local ffi = require("ffi")
  ffi.cdef[[
		float HistogramDistance(const float* a, const float* b, int length);
    typedef struct { float x, y; } vec2;
    typedef struct { int x, y; } vec2i;
    typedef struct { float x, y, z; } vec3;

		typedef struct {
			int width;
			int height;
			int channels;
			int stride;
			int bytes;
			uint8_t* data;
		} RgbaImage;

    typedef struct {
      vec2i depthTopLeft;
      vec2i depthBotRight;
      vec3 metricPosition;
      float weight;
			float histogram[768];
			RgbaImage color;
    } Detection;

    typedef struct {
      double timestamp;
      int32_t numDetections;
      Detection detections[16];
      uint8_t distance_sensors[8];
    } World;

    typedef struct {
      float weight;
      vec2 kinect;
      vec3 position;
			int32_t detectionIndex;
    } Target;

    typedef enum {
      Error = 0,
      Resting = 1,
      Searching = 2,
      Following = 3,
    } Activity;

    typedef struct {
      int32_t activeTarget;
      int32_t numTargets;
      Target targets[16];
      Activity activity;
    } TrackingState;

    typedef struct {
      vec2 camera;
      float rotationSpeed;
      float speed;
    } ControlState;
  ]]

	HIST_SIZE = 768

	function remote_log(fmt, ...)
		local r = string.format(fmt, ...)
		remote_log_internal(r)
	end

  function do_decide(dt, world, state, tracking)
    world = ffi.cast("World*", world)
    state = ffi.cast("ControlState*", state)
    tracking = ffi.cast("TrackingState*", tracking)
    if decide ~= nil then
      local ok, err = pcall(decide, dt, world, state, tracking)
      if not ok then
        io.write("error calling user code: ", err, "\n")
      end
    end
  end
)";

ScriptLoader::~ScriptLoader() { if (lua) lua_close(lua); }

bool ScriptLoaderInit(ScriptLoader* loader) {
  loader->lua = luaL_newstate();
  if (!loader->lua) {
    return false;
  }
  luaL_openlibs(loader->lua);

  if (luaL_dostring(loader->lua, initScript)) {
    printf("failed to load wrapper script: %s\n",
           lua_tostring(loader->lua, -1));
    return false;
  }

	lua_pushlightuserdata(loader->lua, loader);
	lua_pushcclosure(loader->lua, RemoteLog, 1);
	lua_setglobal(loader->lua, "remote_log_internal");

  return true;
}

const char* ScriptLoaderUpdate(ScriptLoader* loader, double dt, World* world,
                               ControlState* state, TrackingState* tracking) {
  lua_State* L = loader->lua;
  lua_getglobal(L, "do_decide");
  lua_pushnumber(L, dt);
  lua_pushlightuserdata(L, world);
  lua_pushlightuserdata(L, state);
  lua_pushlightuserdata(L, tracking);
  if (lua_pcall(L, 4, 0, 0) != 0) {
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
  return luaL_dostring(loader->lua, script) == 0;
}

void ScriptLoaderSetLogCallback(ScriptLoader* loader, void(*cb)(const char*, void*), void* user) {
	loader->log = cb;
	loader->user = user;
}

const char* ScriptLoaderGetError(ScriptLoader* loader) {
  return lua_tostring(loader->lua, -1); 
}
