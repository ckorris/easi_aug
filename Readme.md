# EasiOS: Augmented Reality Projectile Path Prediction for Airsoft/BB Guns

EasiOS predicts the path of a BB fired from an airsoft gun, including the point of impact, and overlays it onto a video stream from the gun's perspective so you can see exactly what the BB will hit. 

Video: https://www.youtube.com/watch?v=5hyBadQNh6I&feature=youtu.be

![](https://thumbs.gfycat.com/FearlessRegalHamadryas-size_restricted.gif)

![](https://thumbs.gfycat.com/AgedQuestionableCornsnake-size_restricted.gif)

A gun-mounted [ZED 2 depth camera](https://www.stereolabs.com/zed-2/), combined with the ZED SDK, effectively build a 3D map of the area in front of the gun up to 40 meters away (20 meters at recommended settings). Then, the on-board computer (ideally an [NVIDIA Jetson Nano](https://developer.nvidia.com/embedded/jetson-nano-developer-kit)) runs a real-time physics simulation, using parameters specified about the airsoft gun and environment, in addition to data collected from the ZED 2's sensors. 

By mounting the camera, Jetson Nano, and a small screen to the gun, you can use this to accurately place shots, taking into account gravity, spin, drag, and other forces that are not easy for a human to calculate on the spot. 


## Requirements

![](https://thumbs.gfycat.com/BestDimwittedAtlanticridleyturtle-size_restricted.gif)

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
- Small monitor - HDMI is fine, but this can also output to [this specific LCD module](https://www.waveshare.com/wiki/2inch_LCD_Module) via SPI if you want something smaller and less power-intensive - see 'LCD Screen via SPI' section

### Software

- ZEDK SDK 3 - tested with 3.2.1 [(Download)](https://www.stereolabs.com/developers/release/)
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

![](https://thumbs.gfycat.com/ImpracticalAccurateBaldeagle-size_restricted.gif)

After cloning this repository, use CMake to build it. 

### Windows 

On Windows, this is easiest with the [Cmake GUI](https://cmake.org/download/). 

After opening CMake:

- Under **Where is the source code:** put in the root folder where you cloned easi_aug. Ex: "C:/Projects/easi_aug"
- Under **Where to build the binaries:** put in the same path with '/build/ appended. Ex: "C:/Projects/easi_aug/build"
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


## LCD Screen via SPI (Optional)

![](https://thumbs.gfycat.com/DentalSecretHart-size_restricted.gif)

While HDMI displays will work on the software side, they're usually too large to practically mount to an airsoft gun. 

Instead, if using the Jetson Nano, you can hook up a mini LCD screen via SPI instead. That makes it lighter, draw less power, and easier to shield from other airsoft BBs. 

However, at time of writing, SPI support on the Jetson Nano has several issues when used in C/C++ via the native calls to the spidev library you're supposed to use for this sort of thing. So I instead developed a custom SPI implementation that's fast enough to run a screen at a respectable frame rate. 

That said, as my goal was to make an augmented reality system, not a full SPI library, my implementation is extremely specific for outputting images to a specific screen that I bought. Also, realistically, I have doubts anyone else will actually clone this repo *and* use it on an airsoft gun. As such, it will *only* work on a Jetson Nano, and only with a very specific LCD monitor: [WaveShare's 2 Inch SPI screen](https://www.waveshare.com/wiki/2inch_LCD_Module). 

While WaveShare's support is abysmal, their physical hardware has consistently been good, so I recommend just buying this specific screen unless you're very comfortable working with SPI and adapting my code to your needs. (If you *are* the latter, feel free to email me at ckorris@gmail.com if you need help. This was by far the most challenging part of the entire project and I'd like to help others land at the bottom of the rabbithole a little softer than I did)

With all that said, here's how you get that specific screen to work: 

1. When building EasiOS as described in the 'Building/Running EasiOS' section, replace the line `cmake ..` with `cmake -DSPI_LCD=ON ..`. You can do this if you've already built the application; simply run that command again, followed by `make -j7` again. 

2. Connect the terminals on the screen to the Jetson Nano's GPIO pins accordingly: 

- **VCC** to a **3v3** pin (Pin 1 or Pin 17)
- **GND** to any **GND** pin 
- **BL** (Backlight) to **Pin 12**
- **RST** (Reset) to **Pin 13**
- **DIN** (Digital In) to **Pin 19**
- **DC** (Data/Command) to **Pin 22**
- **CLK** (Clock) to **Pin 23**
- **CS** (Chip Select) to **Pin 24**

3. Run the app from an elevated terminal, e.g. `sudo ./EasiOS`

*Note: As of Jetpack 4.3, there is an option to enable and disable SPI, as outlined and described in detail in [this wonderful JetsonHacks tutorial](https://www.jetsonhacks.com/2020/05/04/spi-on-jetson-using-jetson-io/). However, as my implementation bypasses all this and writes directly to the memory that controls the pins, this is not required and should make no difference.*

## Battery Power (Optional)

*Note: Big thanks to [Myzhar](https://github.com/Myzhar) for helping me with this. I had no idea where to start and he walked me through the whole process. The man is a legend.* 

![](https://thumbs.gfycat.com/UnselfishChubbyArachnid-size_restricted.gif)

To use the Jetson Nano on an airsoft gun, you'll need to power it via battery. We can supply 5 volts, 3 amps via the two 5V GPIO pins on the Nano (pins 2 and 4) for a total of 6 amps. 

### Requirements: 

- LiPo battery - preferably 7.4 volts, at least 20C, with a large mAh rating. I use [this one](https://www.amazon.com/gp/product/B07VQTZ6G7) which runs the system for roughly 1.25 hours
- Two DC/DC converters whose range encompasses 7.4 volts in (or the voltage of your battery) and 4 volts out. I used [these](https://www.amazon.com/gp/product/B07F3S5ZDX/).
- Wire, at least some 16 AUG for the pre-split voltage, and regular hobbyist 22 AUG is fine for the rest
- Soldering kit (iron, solder, etc.)
- Female adapter for the battery (Deans, Tamiya, etc.) to straight wires - you can disassemble most any kind of appropriate adapter
- Voltmeter
- GPIO jumper wire/bridge
- (Optional) T-splitter wires


It's hard to find a 5V LiPo battery unless you make one. Usually they come in 7.4 or greater. So we have to step that voltage down to 5 volts first using DC/DC converters. 

Now, since it's hard to find DC/DC converters that can take the full amperage of the battery, we'll want to split the current *before* converting it, using two separate DC/DC converters. You can make a splitter with wire and a soldering iron, or buy one. 

Below I describe a barebones setup that will power the Nano successfully. You can take liberties to play with the lengths or connections. For example, I run a section of wire through the inside of my airsoft gun, but connect the leads from the converters, and to the Nano's GPIOs, via detachable Tamiya plugs for convenience. 

![](https://i.imgur.com/Y8A4mcz.jpg)

Also, it goes without saying that working with electricity is dangerous, and this assumes you will take the necessary safety precautions, including insulating the wires as you go. If you don't know what I'm talking about, don't try anything in this section. 

- Solder the voltage and ground wires from the female battery adapter/plug to two T-splitters. You should use at least 16 AUG wire for this. You now have two voltage lines and two ground.
- Solder the two voltage lines to the postive input terminals on the DC/DC converters, and do the same with the ground lines and the negative input terminals.

Before connecting to the Nano, we need to make sure we're stepping down the voltage to exactly 5 volts. Do *not* skip this step unless you love the smell of burning silicon and hate the smell of money. 

- Plug in the battery. If the DC/DC converters have LEDs, they should light up. 
- Set your voltmeter to test for DC voltage.
- Touch the positive voltmeter terminal to the positive output terminal of one of the DC/DCs, and the negative to the negative output of the same one. 
- Use a screwdriver to turn the little screw on top of that converter. As you wind it, the voltage reading should change. Continue turning until you get to 5 volts or slightly below it. Repeat this process with the other converter.

To be safe, I recommend supergluing the base of the screw once it's at 5 volts. That screw can move over time and could end up frying your Nano later. 

-Unplug your battery.

- Take four wires with female ends (e.g. they can be attached to a GPIO pin) and solder the other ends to the output terminals of the converters. Ideally, use red ones for the positive outputs and black for the negatives. 

- Plug the two positive leads into the two 5V pins on the Nano, and the two negatives into any of the GND pins. 

The last thing before plugging in the battery is to bridge the two J48 pins on the Nano. This tells the Nano to bypass USB power. These are two pins sitting by themselves between the power jack and the camera connector. 

Now double check that there are no potential shorts, and plug in the battery. The Jetson should start up. 



## TODO


While I've accomplished most of what I want, there's a few things left: 

**1. Test:** I haven't actually tested *any* of the physics on this at a range greater than six meters, thanks to the pandemic. However, I have a place I can go to sometime this month. Other than the spin decay issue, the math *should* all be solid, but I'm not about to make promises before testing. 

**2. Remove OpenCV:** The OpenCV dependency is, frankly, weird. I chose it when I got started because, being new to C++, I wanted a library that had easy-to-use matrix functions, a built-in UI, and samples for integrating it with the ZED SDK. However, I don't use any of the computer vision features, which makes the project rather bloated on disk (not to mention a very long build time for OpenCV on my poor Jetson Nano). So eventually I want to replace it with a library (or libraries) that are more lightweight and suited for the application. 

**3. Spin Decay:** The physics simulation does not currently use an accurate calculation for spin decay - that is, how quickly a spinning BB stops spinning. This is because it's a very complex situation, and formulas you can find are nebulous at best. In addition, existing knowledge on the topic is largely about situations other than a smooth, unfixed sphere. As such, it will take a lot more work to put together an accurate model for this, especially as I am not a physicist or anything close.

That said, I'm about halfway through building an experiment to test this, by measuring the spin across a distance. It involves using glow in the dark BBs that are painted half black, and light sensors placed throughout the tube. The idea is that the spin will cause the BBs to strobe, so sampling the emitted light from a known location can allow us to see the spin rate. 

I've built the chamber and collected data, but currently my sample rate is too slow, and I need a way to ensure that the BBs are oriented with their 'horizons' perpendicular to the direction they spin.

## Acknowledgements

Thanks goes out to: 

- Stereolabs, my former employers, for making the incredible ZED 2 and SDK. They were also kind enough to send me a free ZED 2 after I had left the company, since I had worked on its launch beforehand. They make seriously incredible products and you all should give them money. 

- Walter, a.k.a. [Myzhar](https://github.com/Myzhar), also at Stereolabs, who helped me a ton with getting the Nano running off of battery power. 

- The [Airsoft Trajectory Project](http://mackila.com/airsoft/atp/), authored by some guy in 2007, from which I used several physics calculations, and what pointed me in the right direction for many others. 

- Dr. Gary Drykacz, author of [The Physics of Paintball](https://web.archive.org/web/20040617080904/http://home.comcast.net/~dyrgcmn/pball/pballIntro.html), which the author of the Airsoft Trajectory Project cited many times, and whose Java paintball app source code helped me understand how to apply academic-style physics formulas to an incremental computer simulation. 

- [Valentis](https://github.com/valentis), whose [Jetson Nano GPIO Example](https://github.com/valentis/jetson-nano-gpio-example) got me started understanding how to control GPIO output via direct memory access, which ended up being the cornerstone of my custom SPI output scripts.

- Alex, Allen, and Ben of EASI Company, the best airsoft squad ever, for making me love the sport. 