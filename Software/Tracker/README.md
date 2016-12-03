Configuring vpx library to build it with VS2013
./configure --disable-examples --disable-docs --enable-vp8 --enable-realtime-only --enable-runtime-cpu-detect --target=x86_64-win64-vs12

Project depends on boost. In case of Windows use prebuilt binaries. While installing the boost confirm that it has lib folder 
and not lib... In the last case rename it to be just lib folder.

Set up first time
-----------------
1. Create a new folder (bin, build or smt.)
2. cd into the folder 
3. Generate VS solution and project files one of these ways
	a)Building without Kinect:
		```
		cmake -G "Visual Studio 12 Win64" -DCMAKE_PREFIX_PATH="C:\boost_1_55_0_x64" ..
		```
	b) Building with Kinect (Library paths on Zotac are the same as here):
		```
		cmake -G "Visual Studio 12 Win64" -DCMAKE_PREFIX_PATH="C:\boost_1_55_0_x64;C:\Program Files\Microsoft SDKs\Kinect\v2.0_1409" -DKINECT_ENABLED=ON ..
		```
4. Open follower.sln file with Visual Studio 2013

Running the core
--------
follower_core 
-h x.x.x.x (ip-address) 
-c cpu.net
-s COMx 
-i default.lua

Detection algorithm
-------
The detection algorithm works in two phases: possible candidate selection and candidate filtering.
Selection is roughly based on *Fast Human Detection for Indoor Mobile Robots Using Depth Images* [1]:
1. The initial depth image (512x424) is downsampled into a 64x53 (1/8) grid, where each depth value is received by selecting n(=16) points through stratified sampling from the original 8x8 grid cell and taking their median. The original algorithm used random sampling, however stratified sampling is faster and allows for repeatability.
2. The downscaled depth image is segmented using *Efficient Graph-Based Image Segmentation* [2]
3. A normal map/image is created from the downscaled depth image and segmented using the same algorithm.
4. Normal and depth points which lie in the same connected component are selected and their corresponding bounding boxes are calculated.
5. Now the regions are merged together based on several heuristics (their distance from each other, the amount of points they have). The original algorithm tried to filter out non-planar regions, but this removed a lot of regions with useful information (e.g. heads) so this step is omitted.
6. Merged regions that fit all the criteria (not too small or large) are used in the final step. Depth values from the original image are copied into a 64x128 image based on the filtered regions. The result is a list of 64x128 depth images which are the final candidates. The previous version created a HOG feature vector from the images, but is now disabled.

The candidate depth images are fed into a convolutional neural network which filters out possible humans. The training set for the classifier consisted of roughly 60k depth images.

[1]: http://www.cs.cmu.edu/~mmv/papers/13icra-CoBotPeopleDetection.pdf
[2]: http://cs.brown.edu/~pff/segment/
Coding
-------
1. Next time after set up it is necessary only to open follower.sln file for coding
2. Follow Google coding guidelines https://google.github.io/styleguide/cppguide.html
	a) In /src folder there is a cpplint.py file to check your code against guidelines.
	Install python 2.7 and run it like that: cpplint.py --linelength=120 comm/serialcomm.cpp
	b) Though guideline says to keep a function size under 40 lines, it is very recommended to keep 
	the size below 20 lines.
	c) Exception. Function names are either all lower-case, with underscores between words (f.e. bool is_empty() const).
	or lower camel case (f.e. bool isEmpty() const).
	d) Try to give reasonable names so others understand the purpose of the variable/function. Use only
	commonly used abbreviations.
	
