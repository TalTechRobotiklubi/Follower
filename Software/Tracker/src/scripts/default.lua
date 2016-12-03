local ffi = require("ffi")
local MAX_TTL = 1.0
local MAX_JUMP = 0.7
local TURN_DECAY = 0.5
local MAX_TIME_WO_TARGET = 3.0

local RESTING = 0
local SEARCHING = 1
local FOLLOWING = 2
local CRY = 3

-- 2 - right
-- 1 - right-45
-- 4 - frontleft
-- 3 - left
local sensors = {1, 2, 3, 4}

function filter(a, f)
  local r = {}
  for i, v in pairs(a) do
    if f(v) then
      r[i] = v
    end
  end
  return r
end

function distance(a, b)
  local x = a.x - b.x
  local z = a.z - b.z
  return math.sqrt(x^2.0 + z^2.0)
end

function find_closest_detection(detections, n, point)
  local closest_detection
  local closest_dist = 10000.0
  for i = 0, n - 1 do
    local detection = detections[i]
    if closest_detection == nil then
      closest_detection = detection
    else
      local dist = distance(detection.metricPosition, point)
      if dist < closest_dist then
        closest_detection = detection
        closest_dist = dist
      end
    end
  end

  return closest_detection
end

function fwd_speed(target_dist)
  if target_dist > 1.0 then
    return 100.0 + 100 * math.max(target_dist, 1.0)
  end
  
  return 1.0
end

function rot_speed(angle)
  local rspeed = 35.0
  if angle > 10.0 then
    return rspeed
  elseif angle < -10.0 then
    return -rspeed
  end

  return 0.0
end

local target = nil
local time_no_target = MAX_TIME_WO_TARGET
function decide(dt, world, state, tracking)
  if target ~= nil then
    target.timeToLive = target.timeToLive - dt
    if target.timeToLive < 0.0 then
      target = nil
    end
  end

  if target == nil then
    local closest = find_closest_detection(world.detections, world.numDetections, {x = 0.0, z = 0.0})
    if closest ~= nil then
      local tl = closest.depthTopLeft
      local br = closest.depthBotRight
      local p = closest.metricPosition
      local hist = ffi.new("float[768]", {})
      ffi.copy(hist, closest.histogram, ffi.sizeof(hist))
      target = {
        timeToLive = 0.5,
        kinect = {x = (br.x + tl.x) * 0.5, y = (br.y + tl.y) * 0.5},
        position = {x = p.x, y = p.y, z = p.z},
        histogram = hist
      }
    end
  else
    local closest_index = -1
    if world.numDetections == 1 then
      local dist = distance(target.position, world.detections[0].metricPosition)
      local hdist = ffi.C.HistogramDistance(target.histogram, world.detections[0].histogram, HIST_SIZE)
      if dist < MAX_JUMP and hdist < 0.2 then
        closest_index = 0
      end
    elseif world.numDetections > 1 then
      local closest_dist = nil
      for i = 0, world.numDetections - 1 do    
        local dist = distance(target.position, world.detections[i].metricPosition)
        if dist < MAX_JUMP then
          local hdist = ffi.C.HistogramDistance(target.histogram, world.detections[i].histogram, HIST_SIZE)
          if closest_index == -1 then
            closest_index = i
            closest_dist = hdist
          elseif hdist < closest_dist then
            closest_index = i
            closest_dist = hdist
          end
        end
      end
    end

    if closest_index ~= -1 then
      local detection = world.detections[closest_index]
      local tl = detection.depthTopLeft
      local br = detection.depthBotRight
      local p = detection.metricPosition
      target.kinect = {x = (br.x + tl.x) * 0.5, y = (br.y + tl.y) * 0.5}
      target.position = {x = p.x, y = p.y, z = p.z}
      target.timeToLive = math.min(MAX_TTL, target.timeToLive + 1.0)
      ffi.copy(target.histogram, world.detections[closest_index].histogram, ffi.sizeof(target.histogram))
    end
  end

  if target ~= nil then
    time_no_target = MAX_TIME_WO_TARGET
    tracking.numTargets = 1
    tracking.activeTarget = 0
    local t = tracking.targets[tracking.activeTarget]
    t.kinect = target.kinect
    t.position = target.position
    t.weight = target.timeToLive / MAX_TTL

    local angle = math.deg(math.atan(-target.position.x / target.position.z))
    state.rotationSpeed = rot_speed(angle)
    state.speed = fwd_speed(target.position.z)
    tracking.activity = FOLLOWING
  else
    state.rotationSpeed = 0.0
    tracking.numTargets = 0
    tracking.activeTarget = -1
    state.speed = 1.0
    tracking.activity = CRY

    time_no_target = time_no_target - dt
  end

  if time_no_target < 0.0 then
    state.rotationSpeed = 50.0
    state.speed = 0.0
    tracking.activity = SEARCHING
  end

  for i = 0, 3 do
    if world.distance_sensors[i] < 25 then
      state.speed = 1.0
    end
  end

  state.camera.y = 20.0
end
