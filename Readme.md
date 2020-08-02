# EasiOS: Augmented Reality Projectile Path Prediction for Airsoft/BB Guns

EasiOS predicts the path of a BB fired from an airsoft gun, including the point of impact, and overlays it onto a video stream from the gun's perspective so you can see exactly what the BB will hit. 

Video: https://www.youtube.com/watch?v=5hyBadQNh6I&feature=youtu.be

![](https://thumbs.gfycat.com/FearlessRegalHamadryas-size_restricted.gif)

![](https://thumbs.gfycat.com/AgedQuestionableCornsnake-size_restricted.gif)



A gun-mounted [ZED 2 depth camera](https://www.stereolabs.com/zed-2/), combined with the ZED SDK, effectively build a 3D map of the area in front of the gun up to 40 meters away (20 meters at recommended settings). Then, the on-board computer (ideally an [NVIDIA Jetson Nano](https://developer.nvidia.com/embedded/jetson-nano-developer-kit)) runs a real-time physics simulation, using parameters specified about the airsoft gun and environment, in addition to data collected from the ZED 2's sensors. 

By mounting the camera, Jetson Nano, and a small screen to the gun, you can use this to accurately place shots, taking into account gravity, spin, drag, and other forces that are not easy for a human to calcutate on the spot. 


## Requirements

### Hardware

Required to build and run: 

- Stereolabs ZED 2 depth camera ([link]((https://www.stereolabs.com/zed-2/)))
- Computer compatible with ZED SDK 3.0

Required to run on airsoft rifle: 

- [NVIDIA Jetson Nano](https://developer.nvidia.com/embedded/jetson-nano-developer-kit)
- LiPO battery, at least 20C - I use [this one](https://www.amazon.com/gp/product/B07VQTZ6G7)
- Two DC/DC converters - I use [these](https://www.amazon.com/gp/product/B07F3S5ZDX/)
- Adapters/splitters for splitting battery output and connecting to the DC/DCs, then plugging into the Nano's GPIO pins
- Some kind of hardware to attach the Nano, camera, screen, battery, and DC/DC to the airsoft rifle, such as picatinny rail adapters
- Small monitor - HDMI is fine, but this can also output via SPI to [this specific LCD module](https://www.waveshare.com/wiki/2inch_LCD_Module) via SPI if you want something smaller and less power-intensive - see 'LCD Screen via SPI' section

### Software

- ZEDK SDK 3 -tested with 3.2.1 [(Download)](https://www.stereolabs.com/developers/release/)
- Operating system compatible with the ZED SDK - Windows, Ubuntu, L4T, etc. (Tested on Windows 10 and L4T w. Jetpack 4.3) 
- OpenCV 4.0 or greater (tested with 4.0.0 on Windows, 4.4.0 on L4T)



## Installing Dependencies

If using the Jetson Nano, this has only been tested using Jetpack 4.3. Get it [here](https://developer.nvidia.com/jetpack-43-archive).


### ZED SDK

Download and install SDK version 3.0 or greater [here](https://www.stereolabs.com/developers/release/). Be sure to choose the appropriate download for your operating system. This has been tested for v3.2.0 only, however, supporting earlier or later versions
should take little to no modification. 

### OpenCV

This has been tested on OpenCV 4.0.0 on Windows, and 4.4.0 on L4T via Jetpack 4.3. 

- Windows Installation Tutorial: https://docs.opencv.org/master/d3/d52/tutorial_windows_install.html
- Linux Installation Tutorial: https://docs.opencv.org/master/d7/d9f/tutorial_linux_install.html

When building OpenCV, make sure that the GTK library is included. This may or may not be automatic, depending on your 
system and installation. Refer to [this StackOverflow issue](https://docs.opencv.org/master/d7/d9f/tutorial_linux_install.html) for troubleshooting. 


## Building/Running EasiOS

After cloning this repository, use CMake to build it. 

### Windows 

On Windows, this is easiest with the [Cmake GUI](https://cmake.org/download/). 

After opening CMake:

- Under **Where is the source code:**, put in the root folder where you cloned easi_aug. Ex: "C:/Projects/easi_aug"
- Under **Where to build the binaries:**, put in the same path with '/build/ appended. Ex: "C:/Projects/easi_aug/build"
- Click **Configure**
- Under **Specify the generator for this project**, select "Visual Studio 15 2017 Win64". If that option is missing, you may need to install Visual Studio 2017. 
- Click **Finish** on that Window, followed by **Generate**

Before you can run the app, you need to put a config file in the build folder. You can find one that will work in the root project folder. Simply copy-paste that in. 

To run, press the Play button or press F5. Alternatively, build the application and double click on the executable. 


### Linux

Use the command line to navigate to the folder where you cloned easi_aug. This should have folders for src, include, images, etc. Then type the following:  

    mkdir build
    cd build
    cmake .. 
    make -j7

*Note: If you want to output to an LCD monitor via SPI, then for the third line, type `cmake -DSPI_LCD=ON ..` instead. See the 'LCD Screen via SPI' section.*

You must have a config file in the build folder to run the app. There's one in the root project folder. To move it, type: 

    cp ../config.txt config.txt

To run: 

    ./EasiOS

If using 

*Note: If outputting to an LCD monitor via SPI, you'll need to use an elevated terminal, so type `sudo ./EasiOS` instead.*


## Configuring the Simulation

While the above steps should get the app running and you should see a path overlayed on the real world, you'll need to supply some information to the simulation to make it accurate. 

You can input all the necessary info via the config file you copied over, but it's *much* easier to use the in-app GUI. 

To expand the GUI, click on the white 'three lines' icon in the top right to see this: 

![](https://i.imgur.com/l7vOOm5.png)

You're now in the Calibration menu. The five menus, accessible by the buttons on the sidebar, are as follows: 

- **Calibration:** Used to indicate the position and rotation of the ZED camera relative to the barrel. Measured from the center of the ZED's left sensor. 
- **Projectile:** Indicate the forward speed, spin speed, and mass of the BB.  
- **Display:** Allows you to toggle various overlays, and rotate the ZED image and UI
- **Environment:** Set the outside temperature and relative humidity. Also indicates the pressure as measured from the ZED's barometer, and a calculation of the air density adn viscosity as calculated from the supplied values. 
- **Statistics:** Not actually a menu, but gives physics data on the simulation, such as the projectile speed, and the coefficientsa dn force of drag and life at both the barrel and the point of impact. 

*Note: The Calibration and Projectile sections are essential to get an accurate simulation. Setting the Environment values impact accuracy slightly, but should not be enough to ruin the simulation if you leave them at default values.*

### Calibration

In order to sync the simuated BB with the actual world it's running in, both for detecting collisions and overlaying it on the display, you need to tell EasiOS exactly how the camera is offset from the barrel. 

Start by taking an actual measurement of the positional offset, and entering that in (in meters). Do the same for rotation, but it's okay if you put in an estimation. 

Next, we'll switch to 'laser' mode, which draws the BB path *without* physics simulation. This will make it much easier to line up the path with the barrel, especially as we haven't configured the rest of the physics simulation yet. 

- Go to the Display menu (the third menu with the screen icon). 
- Uncheck **G-XHR** and **G-PTH**
- (Optional) Uncheck **DIST** and **TIME** 
- Check **L-XHR** and **L-PTH**
- Return to the Calibration menu

You should now see this: 

![](https://i.imgur.com/31FHem0.png)

Next, you'll need to establish some kind of ground truth - a visible marker in the real world that you *know* that the barrel is pointing toward directly. 

The best solution is to put a laser pointer in the barrel, and use the dot it creates. This is especially useful because you can easily point it at surfaces at different distances, which helps validate the calibration. To guarantee accuracy, use a [laser bore sight](https://www.amazon.com/laser-bore-sighter/s?k=laser+bore+sighter) made for rifles. 

If you can't use a laser pointer, then you could line up the barrel with a long, straight surface, like the edge of a table, and use the end of that edge as your zero point. 

With the zero point established, line up the green line so that it's coming out of your barrel and ends at that point. 

Note that calibration is a trickier process than it looks. Be ready for some trial and error, especially the first time. It is helpful if you are somewhere where you can fire your BB gun at something within close range, especially for making sure the line is coming straight out of the center of your barrel. An airsoft tracer attachment and BBs can also assist with this. 

Once you're done, restore the Display settings you changed to see the red physics simulation line again. 

### Projectile 

![](https://i.imgur.com/4OGjp1K.png)

The Speed and Mass settings are much simpler: 

- **Speed: ** Use a chronometer, available to use for free at most airsoft fields, to measure the speed of your BB. You must input it in meters per second. To convert a speed in feet per second (FPS), multiply that value by 0.3048. 
- **Mass: ** This is simply the mass of the BBs in grams, almost certainly listed on the bottle they came in. 

Hop-Up is more difficult. There is currently no standard tool for measuring this. However, note that a speed of 120,000 RPM is considered to be around the upwards rate of spin for an airsoft rifle, so you can estimate accordingly. Then you can tweak it so that it lines up with your projectile when you fire it. 

*Note: As of writing, an important part of the physics simulation related to hop-up is not finalized - the decay of a BB's spin over time. This means that you will lose accuracy the faster that your BB is spinning, since we're not properly accounting for this change. Consider underestimating your hop-up value, or turning down the hop-up on your gun altogether. See the 'TODO' section.*

### Display

We were here during the Calibration section to switch off the gravity simulation temporarily. Here's what all the options do: 

- **L-XHR:** Toggle the physics-less 'laser' crosshair dot on the impact site
- **L-PTH:** Toggle the physics-less 'laser' path
- **G-XHR:** Toggle the crosshair dot on the BB's impact site, factoring in physics ('G' stands for 'gravity')
- **G-PTH:** Toggle displaying the path of the BB, factoring in physics
- **DIST:** Distance, in meters, to the impact site, according to the physics simulation 
- **TIME:** Time, in seconds, that it will take the BB to travel from the barrel to the impact site
- **UI Rotation:** Rotates the UI, in case your screen is rotated (This may be broken - see 'TODO' section)
- **Camera Rotation:** Rotates the image from the ZED, in case your camera is rotated relative to your screen

### Environment

![](https://i.imgur.com/wPw2Am5.png)

This is the easiest configuration step of all. Simply Google the temperature (in Celcius) and relative humidity (percentage) for your area at the time you're using your gun. 

Ideally, you should update this every time your use your gun, and update it throughout the day. However, note that the effect of the temperature and humidity on the BB is not too big; it changes the drag slightly. So if you forget to update this setting since the last time you used your gun, the simulation will likely be pretty close. 

*Note: While the ZED 2 has an internal temperature sensor, we do not use it to calculate the temperature because it's strongly affected by the heat output of the camera itself. However, we do use the ZED 2's barometer for a constant air pressure reading.*

### Statistics

![](https://i.imgur.com/JtJFFN7.png)

This page isn't needed for an accurate simulation, and in fact does not have any settings. But for fellow nerds, you can use it to get an understanding of the physics simulation in real-time. 

Abbreviations: 

- **@Brl: ** The state of the BB right as it left the barrel
- **@Imp: ** The state of the BB right before hitting the impact point
- **Speed: ** Not an abbreviation but my OCD won't let me not list it here. 
- **cD: ** Drag coefficient as calculated at the specified point. Derived from the speed, spin, BB diameter, air density, and air viscosity
- **fD: ** Force, in Newtons, applied by drag at the specified point
- **cL: ** Lift coefficient as calculated at the specified point. Derived from the spin rate, the linear speed, and BB diameter
- **fL: ** Force, in Newtons, applied by the Magnus effect (from spin/hop-up) at the specified point

