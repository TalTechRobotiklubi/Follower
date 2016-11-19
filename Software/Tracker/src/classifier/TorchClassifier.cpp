#include "TorchClassifier.h"
#include <fhd_candidate.h>
#include <lua.hpp>
#include "Constants.h"

namespace {
const char* const detectionScript = R"(
    require 'nn';
    local ffi = require("ffi");
    torch.setdefaulttensortype('torch.FloatTensor')
    print("Torch threads: " .. torch.getnumthreads())

    function load_torch_classifier(name)
      local netprops = torch.load(name, "ascii")
      local net = netprops.net
      local mean = netprops.mean
      local stddev = netprops.stddev

      return function(depth_image)
        local arr = ffi.cast("float*", depth_image)
        local w = 64
        local h = 128
        local t = torch.Tensor(h, w)
        local s = t:storage()
        for i = 1, s:size() do
          s[i] = arr[i - 1]
        end

        t:add(-mean)
        t:div(stddev)

        local p = net:forward(t:view(1, h, w))
        return p[2] > p[1]
      end
    end
  )";
}

TorchClassifier::TorchClassifier(const char* file)
    : L(luaL_newstate()),
      fileName(file),
      depthBuffer(kCandidateWidth * kCandidateHeight) {
  fhd_image_init(&bufferImg, kCandidateWidth, kCandidateHeight);
}

TorchClassifier::~TorchClassifier() {
  if (L) lua_close(L);
  fhd_image_destroy(&bufferImg);
}

bool TorchClassifier::Init() {
  if (!L) {
    printf("TorchClassifier: failed to create lua instance\n");
    return false;
  }

  luaL_openlibs(L);

  if (luaL_dostring(L, detectionScript) != 0) {
    printf("TorchClassifier: failed to load detection script %s\n",
           lua_tostring(L, -1));
    return false;
  }

  lua_getglobal(L, "load_torch_classifier");
  lua_pushstring(L, fileName.c_str());

  if (lua_pcall(L, 1, 1, 0) != 0) {
    printf("TorchClassifier: failed to load %s\n", fileName.c_str());
    return false;
  } else {
    classifierFn = luaL_ref(L, LUA_REGISTRYINDEX);
  }

  return true;
}

bool TorchClassifier::Classify(const fhd_candidate* candidate) {
  fhd_image_region srcReg;
  srcReg.x = 1;
  srcReg.y = 1;
  srcReg.width = candidate->depth.width - 2;
  srcReg.height = candidate->depth.height - 2;

  fhd_image_region dstReg;
  dstReg.x = 0;
  dstReg.y = 0;
  dstReg.width = kCandidateWidth;
  dstReg.height = kCandidateHeight;

  fhd_copy_sub_image(&candidate->depth, &srcReg, &bufferImg, &dstReg);

  for (int i = 0; i < bufferImg.len; i++) {
    depthBuffer[i] = float(bufferImg.data[i]);
  }

  lua_rawgeti(L, LUA_REGISTRYINDEX, classifierFn);
  lua_pushlightuserdata(L, &depthBuffer[0]);

  if (lua_pcall(L, 1, 1, 0) != 0) {
    printf("TorchClassifier: failed to call classify: %s\n",
           lua_tostring(L, -1));
    return false;
  }

  int res = lua_toboolean(L, -1);
  lua_pop(L, 1);
  return res > 0;
}
