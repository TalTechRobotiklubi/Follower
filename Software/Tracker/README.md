Needed tools:
CMake 3.x.x
Visual Studio 2012

Needed 3rd party libraries: 
* OpenCV 2.4.11
* Boost 1.55

Zotac build: 
1) run in bin/build folder the command

2) cmake -G "Visual Studio 12 Win64" -DCMAKE_PREFIX_PATH="C:\boost_1_55_0_x64;C:\Program Files\Microsoft SDKs\Kinect\v2.0_1409;C:\opencv-2.4.11\build\install" -DCMAKE_BUILD_TYPE=Release ..
or
cmake -G "Visual Studio 12 Win64" -DCMAKE_PREFIX_PATH="C:\boost_1_55_0_x64\boost;C:\Opencv\Opencv2.4.11\opencv\build" -DCMAKE_BUILD_TYPE=Release ..

3) run Follower.sln