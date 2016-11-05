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

local target = nil
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
      local k = closest.kinectPosition
      local p = closest.metricPosition
      target = {
        timeToLive = MAX_TTL,
        kinect = {x = k.x, y = k.y},
        position = {x = p.x, y = p.y, z = p.z}
      }
    end
  else
    for i = 0, world.numDetections - 1 do
      local detection = world.detections[i]
      local p = detection.metricPosition
      local dist = distance(target.position, detection.metricPosition)
      if dist < 0.8 then
        local k = detection.kinectPosition
        target.kinect = {x = k.x, y = k.y}
        target.position = {x = p.x, y = p.y, z = p.z}
        target.timeToLive = MAX_TTL
        break
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

    if angle > 10.0 then
      state.rotationSpeed = rspeed
      state.speed = 0.0
    elseif angle < -10.0 then
      state.rotationSpeed = -rspeed
      state.speed = 0.0
    else 
      state.rotationSpeed = 0.0
      if target.position.z > 1.4 then
        state.speed = 200.0
      else
        state.speed = 0.0
      end
    end
  else
    tracking.numTargets = 0
    tracking.activeTarget = -1
    state.rotationSpeed = 0.0
    state.speed = 0.0
  end

  state.camera.y = 20.0
end
