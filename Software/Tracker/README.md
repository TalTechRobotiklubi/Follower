Building without Kinect:

```
cmake -G "Visual Studio 12 Win64" -DCMAKE_PREFIX_PATH="C:\boost_1_55_0_x64;C:\opencv-2.4.11\build\install" ..
```

Building with Kinect (Library paths on Zotac are the same as here):

```
cmake -G "Visual Studio 12 Win64" -DCMAKE_PREFIX_PATH="C:\boost_1_55_0_x64;C:\Program Files\Microsoft SDKs\Kinect\v2.0_1409;C:\opencv-2.4.11\build\install" -DKINECT_ENABLED=ON ..
```
