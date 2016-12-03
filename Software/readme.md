Software
========
General
-----------
Software is divided into four folder.
  - **LowLevel** - here is software for motor controller, sensor board, vision board and remote stick. All of them running the software on STM32 micro-controllers.
  - **Remote** - it is a remote PC software to communicate with sensor board. It could be used as a remote control or as a tool to do diagnostics for the robot.
  - **Tools** - it contains CaseCatcher tool to generate C code for defined message fields and messages. 
  - **Tracker** - it is a core software running in PC, which is on the follower robot. This folder contains also client software to communicate via core software.

Communication
----------------------
Follower robot has several communication interfaces. See the picture below.

![communication diagram](https://gitlab.com/TTYRobotiklubi/Follower/raw/master/Documents/Images/communication.png)

It is used CAN bus between motor controllers, sensor board and vision board. It is mainly used to drive motors and receive feedback from robot. There is XRF radio module connected to sensor board. It provides a control and feedback possibility with low level robot without running the on-board PC. CAN bus and PC are connected via vision board, which passes messages from CAN bus to USB and vice-versa. Video and following algorithm feedback is passed to remote computer via WiFi.