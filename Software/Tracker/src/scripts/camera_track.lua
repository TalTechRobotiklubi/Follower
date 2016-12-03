local ffi = require("ffi")
local MAX_TTL = 0.1
local MAX_JUMP = 0.7
local TURN_DECAY = 0.5
local MAX_TIME_WO_TARGET = 3.0

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

local decide_init = true
local target = nil
local curr_camera_x = 0.0
local default_camera_y = 20.0
local curr_camera_y = default_camera_y
local curr_rotation_speed = 0

function decide(dt, world, state, tracking)
  if decide_init == true then
    curr_camera_y = default_camera_y
    state.camera.y = curr_camera_y
    decide_init = false
  end

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
      target.timeToLive = math.min(MAX_TTL, target.timeToLive + 0.5)
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

    local angle = math.deg(math.atan(-target.position.x / target.position.z))
    
    local camera_x_acc = 0.8  -- change it depending on how fast is human detection
    local camera_y_acc = 0.8
    local max_degrees = 45

    -- range -45 ... 45
    if angle > 8 then
      curr_camera_x = curr_camera_x + camera_x_acc
      if curr_camera_x > max_degrees then
        curr_camera_x = max_degrees
      end
      state.camera.x = curr_camera_x
    elseif angle < -8 then
      curr_camera_x = curr_camera_x - camera_x_acc
      if curr_camera_x < -max_degrees then
        curr_camera_x = -max_degrees
      end
      state.camera.x = curr_camera_x
    end
    
    -- range -1.2 ... 1.2
    if target.position.y > 0.14 then
      curr_camera_y = curr_camera_y + camera_y_acc
      if curr_camera_y > max_degrees then
        curr_camera_y = max_degrees
      end
      state.camera.y = curr_camera_y
    elseif target.position.y < -0.14 then
      curr_camera_y = curr_camera_y - camera_y_acc
      if curr_camera_y < -max_degrees then
        curr_camera_y = -max_degrees
      end
      state.camera.y = curr_camera_y
    end
  
    local rotate_acc = 1
    local max_rot = 40
    if curr_camera_x > 6 then
      curr_rotation_speed = curr_rotation_speed + rotate_acc
      if curr_rotation_speed > max_rot then
        curr_rotation_speed = max_rot
      end
      state.rotationSpeed = curr_rotation_speed
    elseif curr_camera_x < 6 then
      curr_rotation_speed = curr_rotation_speed - rotate_acc
      if curr_rotation_speed < -max_rot then
        curr_rotation_speed = -max_rot
      end
      state.rotationSpeed = curr_rotation_speed
    end
  else
    tracking.numTargets = 0
    tracking.activeTarget = -1
    state.rotationSpeed = 0.0
    state.speed = 0.0
  end

  
end
