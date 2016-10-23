function user_decide(dt, world, state)
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
  end
  return state
end
