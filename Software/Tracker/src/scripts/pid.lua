local M = {}

local PID = {}

function PID:update(actual, expected, dt)
  dt = dt or 1.0
  local err = expected - actual;
  self.intErr = self.intErr + err;
  self.diffErr = err - self.diffErr;
  return self.kp * err + (self.ki * self.intErr * dt) + (self.kd * self.diffErr / dt);
end

function M.new(kp, ki, kd)
  local p = {
    kp = kp,
    ki = ki,
    kd = kd,
    intErr = 0.0,
    diffErr = 0.0
  }
  setmetatable(p, {__index = PID})
  return p
end

return M