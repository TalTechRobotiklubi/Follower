Set up first time
-----------------
1. Create a new folder (bin, build or smt.)
2. cd into the folder 
3. Generate VS solution and project files one of these ways
	a)Building without Kinect:
		```
		cmake -G "Visual Studio 12 Win64" -DCMAKE_PREFIX_PATH="C:\boost_1_55_0_x64;C:\Opencv\Opencv2.4.11\opencv\build" ..
		```
	b) Building with Kinect (Library paths on Zotac are the same as here):
		```
		cmake -G "Visual Studio 12 Win64" -DCMAKE_PREFIX_PATH="C:\boost_1_55_0_x64;C:\Program Files\Microsoft SDKs\Kinect\v2.0_1409;C:\opencv-2.4.11\build\install" -DKINECT_ENABLED=ON ..
		```
4. Open follower.sln file with Visual Studio 2013

Coding
-------
1. Next time after set up it is necessary only to open follower.sln file for coding
2. Follow Google coding guidelines https://google.github.io/styleguide/cppguide.html
	a) In /src folder there is a cpplint.py file to check your code against guidelines.
	Install python 2.7 and run it like that: cpplint.py --linelength=120 comm/serialcomm.cpp
	b) Exception. Function names are either all lower-case, with underscores between words (f.e. bool is_empty() const).
	or lower camel case (f.e. bool isEmpty() const).
	c) Try to give reasonable names so others understand the purpose of the variable/function. Use only
	commonly used abbreviations.
	
