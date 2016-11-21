local ffi = require("ffi")
local inspect = require("inspect")
local PID = require("pid")

local MAX_TTL = 2.5
local MAX_JUMP = 0.5

function filter(a, f)
  local r = {}
  for i, v in pairs(a) do
    if f(v) then
      r[i] = v
    end
  end
  return r
end

function clamp(a, min, max)
  if a < min then
    return min
  elseif a > max then
    return max
  else
    return a
  end
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

local target = nil
local cam_pid = PID.new(0.0, 0.5, 0.0)
local turn_pid = PID.new(0.1, 0.1, 0.0)
local forward_pid = PID.new(0.1, 0.1, 0.0)
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
        local hdist = ffi.C.HistogramDistance(target.histogram, world.detections[i].histogram, HIST_SIZE)
        if dist < MAX_JUMP then
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
      target.timeToLive = math.min(MAX_TTL, target.timeToLive + 0.1)
      ffi.copy(target.histogram, world.detections[closest_index].histogram, ffi.sizeof(target.histogram))
    end
  end

  local rspeed = 40.0
  if target ~= nil then
    tracking.numTargets = 1
    tracking.activeTarget = 0
    local t = tracking.targets[tracking.activeTarget]
    t.kinect = target.kinect
    t.position = target.position
    t.weight = target.timeToLive / MAX_TTL

    local angle = math.deg(math.atan2(-target.position.x, target.position.z))

    local current_angle = state.camera.x
    local new_angle = cam_pid:update(current_angle, angle)
    new_angle = clamp(new_angle, -45.0, 45.0)
    state.camera.x = new_angle
    local new_robot_rot = turn_pid:update(new_angle, 0.0)
    local diff = clamp(-new_robot_rot * 2.0, -rspeed, rspeed)
    state.rotationSpeed = diff

    -- This probably isn't right. Test turning first!
    local z = target.position.z
    local ns = forward_pid:update(state.speed, 200.0 * math.min(z / 1.4, 1.0))
    state.speed = ns

  else
    tracking.numTargets = 0
    tracking.activeTarget = -1
    state.rotationSpeed = 0.0
    state.speed = 0.0
  end

  state.camera.y = 20.0
end
