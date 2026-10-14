# N20 RC Car

A four wheel RCcar in the shape of a small car. The ESP32 itself creates a Wi-Fi access point which provides a simple web page for control, meaning the car can be controlled directly from a phone / laptop without any other hardware.

!build image

What is this?

This is a 4 wheel skid-steer robot with four N20 metal gear motors. Motors on each side are combined and fed as one group - left and right. The car can go forward, backward, and turn in place by controlling the speed and direction of each side.

The ESP32 also runs its own Wi-Fi network and displays a basic web page containing directional buttons and a speed slider.

``text

Phone / laptop

|

| Wi-Fi

v

ESP32 webpage and web server

|

| GPIO direction + PWM

v

TB6612FNG motor driver

|

+---- Left motors (2)

|

+---- Right motors (2)

`

Why I created it

What I was trying to build was a small thing – still in the car kind of domain – but to integrate a lot of the technology that I've been playing with into a single exciting parcel that I could enjoy playing with, and that demonstrates how the ESP32 can interface to the real world using a web page served from the board.

Features

- Four-wheel drive using four N20 motors.
- Wi-Fi access-point hosted by the ESP32.
- Browser controls and the URL and search bar on a web page (from the board to go forward, backward, left, right and stop).
- Customizable motor speed using PWM.
- Custom CAD parts and Fritzing circuit.
- Test firmware for motor testing without Wi-Fi.

Repository structure

`text

.

bom.csv

cad

bodyOnly.stl

clamp.step

circuit

rc-car.fzz

firmware

main.cpp

images

build.png

cad.png

circuit.png

running.png

readme.md

video

speedfull.mp4

working1.mp4

`

Hardware

| Component | Qty |

|---|---:|

| ESP32 development board | 1 |
| TB6612FNG dual motor driver | 1 |
| N20 metal gear motors | 4 |
| 43 mm wheels for N20 motors | 4 |
| 18650 Li-ion cells | 2 |
| 2-cell 18650 holder | 1 |
| 3.3V, 2A buck converter | 1 |
Item No. Description Qty. | Custom designed chassis and printed parts | 1 set |
| Wires and connectors | As required |

Build photos

!Assembled car

!Car running

Circuit

!Circuit diagram

The Fritzing project is included at circuit/rc-car.fzz.

# ESP32 to TB6612FNG

| TB6612FNG pin | ESP32 connection |

|---|---|

| PWMA | GPIO 25 |

| AIN1 | GPIO 26 |

| AIN2 | GPIO 27 |

| PWMB | GPIO 14 |

| BIN1 | GPIO 16 |

| BIN2 | GPIO 17 |

| STBY | GPIO 33 |

| VCC | 3.3V logic supply that has been regulated |
| GND | Common ground |
| VM | Motor battery positive |

# Motors

| Driver outputs | Connected motors |

|---|---|

| AO1 and AO2 | Both motors on the left side, in parallel |
| BO1 and BO2 | Both on the right side, both in parallel |

The pairs are in parallel, so the two motors on each side are all getting the same direction signal. If a motor is spinning in the opposite direction, switch its two output wires.

NOTE: a single TB6612FNG channel is used to drive both motors on each side. Make sure that the stall current of one motor on that side does not exceed the maximum current the driver can deliver, and make sure that the pair does not exceed the driver's limit as well.

Power

The two 18650 cells slip into the 2-cell holder. Verify the holder is set to connect them in series or in parallel before wiring it in.

The intended connections are:

- Connect battery pack positive to TB6612FNG VM motor supply.
- Battery pack negative to common ground.
- Supplying ESP32 3V3 pin and TB6612FNG VCC, via regulated 3.3V supply, if supply of current for ESP32 board is suitable for the current peaks of Wi-Fi.
- ESP32, driver, and converter grounds joined together.

The buck converter listed is a 3.3V 2A device not 5V. Powering the development board by the 3V3 pin bypasses the on-board regulator, so check the board docs and also measure the rail under Wi-Fi load. Do not provide 3.3V to the development board's 5V/VIN pins, and 5V to 3V3.

Use correct power charging and protection hardware for pack type. A two cell series pack cannot be charged with one cell charger. Check the voltage and polarity, before plugging in the ESP32 or motor driver.

Firmware

The main firmware is in firmware/main.cpp.

It has to use the Arduino framework and the ESP32 Arduino Core 3.x LEDC PWM API.

# Upload and use

1. Open firmware/main.cpp with your Arduino IDE or import it into an Arduino sketch.
2. Choose the ESP32 board and port.
3. Upload the firmware.
4. Connect your phone or laptop to the ESP32 Wi-Fi network:
- Network: Stanza_Car
- Password: drive123
5. Open http://192.168.4.1 in a browser.
6. Begin at a slow pace and try with lifted wheels.

The access point is included in the ESP32 so it does not need to be connected to your home router or internet.

Videos

# Working

Open video/working1.mp4

# Full-speed test

Open video/speedfull.mp4

CAD

The CAD assets are located in cad/.

!CAD model

- cad/bodyOnly.stl - chassis/body mesh for printing.
- cad/clamp.step - clamp part in STEP format.

Bill of Materials

bom.csv`.

| Item | Qty | Unit Cost | Total |

|---|---:|---:|---:|

| ESP32 Development Board | 1 | 410 | 410 |
| TB6612FNG Motor Driver TB6612FNG Motor Driver | 1 | ₹149 | ₹149 |
| N20 Metal Gear Motors | 4 | ₹310 | ₹1,240 |
| 43 mm Rubber Wheels | 4 | ₹40 | ₹160 |
| 18650 Li-ion Batteries | 2 | ₹45 | ₹90 |
| 2 × 18650 Battery Holder | 1 | ₹19 | ₹19 |
| 3.3V 2A Buck Converter | 1 | ₹112 | ₹112 |

| Total for Priced Items | | | | | |₹2,180 |