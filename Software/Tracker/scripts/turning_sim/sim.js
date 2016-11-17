function PID(kp, ki, kd) {
  this.kp = kp;
  this.ki = ki;
  this.kd = kd;
  this.intErr = 0.0;
  this.diffErr = 0.0;
}

PID.prototype.update = function(dt, actual, expected) {
  var err = expected - actual;
  this.intErr = this.intErr + err;
  this.diffErr = err - this.diffErr;
  return this.kp * err + (this.ki * this.intErr) + (this.kd * this.diffErr);
}

var toScreenPosition = function(camera, worldPosition) {
  var w = camera.screen[0];
  var h = camera.screen[1];
  var x = worldPosition[0];
  var y = worldPosition[1];
  var cx = camera.position[0];
  var cy = camera.position[1];
  return [w * 0.5 + (x - cx), h * 0.5 - (y - cy)];
};

var toWorldPosition = function(camera, screenPosition) {
  var w = camera.screen[0];
  var h = camera.screen[1];
  var x = screenPosition[0];
  var y = screenPosition[1];
  var cx = camera.position[0];
  var cy = camera.position[1];
  return [(x - w * 0.5) + cx, -(y - h * 0.5) + cy];
};

var state = {
  time: 0.0,
	mouse: [500, 100]
};

function renderRect(ctx, camera, center, w, h, rotation, fill) {
  var rc = toScreenPosition(camera, center);

  ctx.save();
  ctx.fillStyle = fill;
  ctx.translate(rc[0], rc[1]);
  ctx.rotate(rotation);
  ctx.fillRect(-w * 0.5, -h * 0.5, w, h);
  ctx.restore();
}

function renderRobot(ctx, camera, robot, state) {
  renderRect(ctx, camera, robot.position, 140, 100, robot.rotation, "#cccabc"); 
  renderRect(ctx, camera, robot.position, 70, 24, robot.camera.rotation, "#3e403c");
}

function rotate(p, rotation) {
  return [
    p[0] * Math.cos(rotation) - p[1] * Math.sin(rotation),
    p[0] * Math.sin(rotation) + p[1] * Math.cos(rotation)
  ];
}

function update(time) {
  var dt = (time - state.time) / 1000.0;
  state.time = time;

  var robot = state.robot;
	var target = toWorldPosition(state.camera, state.mouse);
  
  var mx = target[0];
  var my = target[1];

  var diff = [mx - robot.position[0], my - robot.position[1]];
  var rot = Math.atan2(diff[0], diff[1]);
  var camDeg = robot.camera.pid.update(dt, robot.camera.rotation, rot);
  robot.camera.rotation = camDeg;

  var newRobotRot = robot.pid.update(dt, robot.camera.rotation, 0.0);
  var diff = -newRobotRot * 0.05;
  robot.rotation += diff;
  robot.camera.rotation += robot.rotation;


  var ctx = state.ctx;
  ctx.clearRect(0, 0, state.w, state.h);
  renderRobot(ctx, state.camera, state.robot);

  var targetPos = toScreenPosition(state.camera, target);
  ctx.save();
  ctx.fillStyle = "#fff";
  ctx.beginPath();
  ctx.arc(targetPos[0], targetPos[1], 10, 0.0, 2 * Math.PI);
  ctx.fill();
  ctx.restore();

  requestAnimationFrame(update);
}

function Sim() {
  var canvas = document.getElementById("canvas");
  var ctx = canvas.getContext("2d");
  var W = canvas.width;
  var H = canvas.height;
  state.ctx = ctx;
  state.w = W;
  state.h = H;

  state.camera = {
    position: [0, 0],
    screen: [W, H]
  };

  state.robot = {
    position: [0, 0],
    rotation: 0.0,
    pid: new PID(2.0, 0.1, 0.0),
    camera: {
      rotation: 0.0,
      pid: new PID(0.0, 0.5, 0.0)
    }
  };

  window.addEventListener("mousemove", function(evt) {
    var rect = canvas.getBoundingClientRect();
		state.mouse[0] = evt.clientX - rect.left;
		state.mouse[1] = evt.clientY - rect.top;
  }, false);

  requestAnimationFrame(update);
}

(function() {
  if (document.readyState != "loading") {
    Sim();
  } else {
    document.addEventListener("DOMContentLoaded", Sim);
  }
})();
