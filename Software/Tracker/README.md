Needed 3rd party libraries: 
* OpenCV 2.4.11
* Boost 1.55

Zotac build

```
cmake -G "Visual Studio 12 Win64" -DCMAKE_PREFIX_PATH="C:\boost_1_55_0_x64;C:\oss\bgfx;C:\oss\bx;C:\Program Files\Microsoft SDKs\Kinect\v2.0_1409;C:\opencv-2.4.11\build\install" -DCMAKE_BUILD_TYPE=Release ..
```
