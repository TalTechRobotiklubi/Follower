local targets = {}
function decide(dt, world, state, tracking)
  local closest_detection
  for i = 0, world.numDetections do
    local detection = world.detections[i]
    if detection.weight >= 1.0 then
      if closest_detection == nil then
        closest_detection = detection
      else
        if detection.metricPosition.z < closest_detection.metricPosition.z then
          closest_detection = detection
        end
      end
    end
  end

  if closest_detection ~= nil then
    local p = closest_detection.metricPosition
    local angle = math.atan(-p.x / p.z)
    state.camera.x = math.deg(angle)
    tracking.activeTarget = 0
    tracking.numTargets = 1
    local t = tracking.targets[0]
    t.kinect = closest_detection.kinectPosition
    t.position = closest_detection.metricPosition
    t.weight = 1.0
  end
  return state
end
