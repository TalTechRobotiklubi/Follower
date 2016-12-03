Follower
===================
General
-----------
Follower is a robot-platform intended to follow a certain person. The long term goal is to develop a platform, which could act as a personal carrier.
Current repository mainly tracks software, but here are also some documentation.

Concept
-----------
Follower's conceptional drawing is presented below

[3d_concept.png]
The robot has differential drive with two wheels inside the body to avoid collision effect while turning. Sensor boards reads ultrasonic sensors, which should detect any obstacles and inertial measurement unit, which helps to detect robots movements in time (turning, acceleration). Vision board controls servo motors to move camera left/right and up/down.

The robot does not have any basket to carry things. It is a first prototype to 
test following concept.