local MAX_TTL = 1.5

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
      local k = closest.kinectPosition
      local p = closest.metricPosition
      target = {
        timeToLive = MAX_TTL,
        kinect = {x = k.x, y = k.y},
        position = {x = p.x, y = p.y, z = p.z}
      }
    end
  else
    if world.numDetections > 0 then
      local closest_index = -1
      local closest_dist = nil
      for i = 0, world.numDetections - 1, 1 do    
        local dist = distance(target.position, world.detections[i].metricPosition)
        if dist < 0.8 then
          if closest_index == -1 then
            closest_index = i
            closest_dist = dist
          elseif dist < closest_dist then
            closest_index = i
            closest_dist = dist
          end
        end
      end
      if closest_index ~= -1 then
        local k = world.detections[closest_index].kinectPosition
        local p = world.detections[closest_index].metricPosition
        target.kinect = {x = k.x, y = k.y}
        target.position = {x = p.x, y = p.y, z = p.z}
        target.timeToLive = MAX_TTL
      end
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
    -- target.position.y is float. 0 point is in the middle
    --remote_log("target: y %f, angle %d\n", target.position.y, angle)
    
    local camera_x_acc = 0.25  -- change it depending on how fast is human detection
    local camera_y_acc = 0.25
    local max_degrees = 45

    if angle > 4 then
      curr_camera_x = curr_camera_x + camera_x_acc
      if curr_camera_x > max_degrees then
        curr_camera_x = max_degrees
      end
      state.camera.x = curr_camera_x
    elseif angle < -4 then
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
   remote_log("target: y %f, camera set %d\n", target.position.y, curr_camera_y)
  else
    tracking.numTargets = 0
    tracking.activeTarget = -1
    state.rotationSpeed = 0.0
    state.speed = 0.0
  end

  
end
