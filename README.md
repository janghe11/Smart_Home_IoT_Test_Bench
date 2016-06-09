#Smart Home IoT Test Bench

##Introduction
**Smart Home IoT Test Bench** is a project light version of MQTT. It is not a kind of MQTT, but made to transfer data easy and visualization like IoT devices.

- **Project name :** Capstone Design - Micro Processor 
- **Development period :** 2016Y 03M 02D ~ 2016Y 06M 10D

##Features
![alt tag](https://github.com/janghe11/Smart_Home_IoT_Test_Bench/blob/master/Pictures/Smart_Home_IoT_Test_Bench_Structures.jpg)

##Installation
- **AVR ATmega8535 :** Build CD_ATmega8535/main.c file and burn into your ATmega8535 or use CD_ATmega8535/Debug/Exe/CD_ATmega8535.hex file
- **Raspberry Pi 2 or 3 :** Execute CD_RaspberryPi2/install.sh in /home/pi/ after expand filesystem and time zone set.(sudo permission and internet connection is needed. Ex) sudo sh install.sh)
- **Android :** Build android projects in CD_Android folder and install apk file in your phone.

##Details
###Hardware platform and devices
- AVR → MIDAS Engineering MDA-Multi MICOM (ATmega8535, http://www.midaseng.com/)
- Raspberry Pi 2 (Compatible to RPi3)
- Android (Since 4.1 Jelly Bean is available)
- Wireless router(Tested in ipTime A2004R)
- USB to RS-232C cable

###Development environment and tools
- **Compiler  :** *AVR →* IAR Embedded Workbench Atmel AVR C / *Raspberry Pi →* arm-gcc C Compiler, PHP 5.6.11
- **Operating system :** *Raspberry Pi →* Raspbian Jessie 03-18-2016 / *Android →* Android 4.1 Jellybean or later
- **Development environment :** *AVR development →* Windows XP or 7 32bit / *Raspberry Pi →* Ubuntu 15.10 64bit or later / *Android →* Android Studio 1.5 or later
- **Applications :** Apache2, mariaDB, PHPMyAdmin, etc.

###AVR ATmega8535
####Brief of Smart Home IoT Test Bench structures
![alt tag](https://github.com/janghe11/Smart_Home_IoT_Test_Bench/blob/master/Pictures/Smart%20Home%20IoT%20Test%20Bench%20Structures.jpg)

####Details of Smart Home IoT Test Bench structures
![alt tag](https://github.com/janghe11/Smart_Home_IoT_Test_Bench/blob/master/Pictures/Smart%20Home%20IoT%20Test%20Bench%20Structures(Details).jpg)

####AVR(ATmega8535 main.c) flowchart
![alt tag](https://github.com/janghe11/Smart_Home_IoT_Test_Bench/blob/master/Pictures/Smart%20Home%20IoT%20Test%20Bench%20Flowchart%20-%20ATmega8535%20(main.c).jpg)

####DDR pin map


####Raspberry Pi 2 structures
![alt tag](https://github.com/janghe11/Smart_Home_IoT_Test_Bench/blob/master/Pictures/Smart%20Home%20Iot%20Test%20Bench%20Structures%20-%20Raspberry%20Pi.jpg)

####Raspberry Pi 2(avr_daemon.c) flowchart
![alt tag](https://github.com/janghe11/Smart_Home_IoT_Test_Bench/blob/master/Pictures/Smart%20Home%20IoT%20Test%20Bench%20Flowchart%20-%20Raspberry%20Pi%20(avr_daemon.c).jpg)

####Raspberry Pi 2(control.php) flowchart
![alt tag](https://github.com/janghe11/Smart_Home_IoT_Test_Bench/blob/master/Pictures/Smart%20Home%20IoT%20Test%20Bench%20Flowchart%20-%20Raspberry%20Pi%20(control.php).jpg)
