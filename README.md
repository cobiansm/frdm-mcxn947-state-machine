# Smart-Lock

## Overview
This projects implements an IoT-based security solution developed for the NXP FRDM RW612 platform. This system manages a physical lock using a servo motor, controlled by both local RFID/Keyboard input and remote network commands, all synchronized through a Real-Time Operating System. The Smart Lock uses a USB RFID reader to capture user IDs, validates them against a remote PHP/SQL database, and controls a servo motor to lock or unlock the door. Additionally, it features a TCP server for administrative remote control.

## Tasks
- **USB Keyboard Task**: Acts as a USB Host to convert the physical data from the RFID/NFC card into a format that the system can understand. and forwards the assembled Tag IDs to the Database Queue.
- **Database Task**: Validates users by performing remote HTTP requests to a PHP/SQL server and triggers the lock mechanism upon successful authentication.
- **TCP Admin Server**: Operates a network socket on port 1030 to receive remote “Open” commands for administrative bypass of the lock.
- **Servo Task**: Controls the physical lock hardware using CTimer PWM signals to move the servomotor between locked and unlocked positions.

## Functional diagram:

```text
user          internet - usb/sd card -+
 O           /                 ^      |
-+- -> NFC ->                save      > - servo -> puerta  
 |           \                 |      |      ^
/ \           no internet - database -+      |
                                             |
                                             |
admin                                        |
 O                  tcp/ip                   |
-+- ---------------> app >-------------------+
 |
/ \
```

---

## Context diagram:

```text
+-----+                                   +-------+ 
| NFC | - send data to authenticate ----> |       | 
+-----+                                   |       |
                                          | smart | - search for tag ID -------> +--------+
+----------+ < search for tag ID -------- | lock  | <- return if tag ID exists - | USB/SD |
| Database |                              |       | - save valid tag IDs ------> +--------+
+----------+ - return if tag ID exists -> |       |
                                          |       |                              +-------+
+-----------+                             |       | - move left/right ---------> | Servo |
| Admin/App | - send unlock command ----> |       |                              +-------+
+-----------+                             +-------+
```

---

## Ideal software diagram:

```text
                                          +-------------------+ 
+-----+          send tag ID              |                   |
| NFC | --------------------------------> |  software system  | 
+-----+    HID USB (keyboard emulated)    |                   |
                                          | usb ctrl          |    write/search/save tag ID  +--------+
+----------+       register/auth ID       | nfc ctr           | <--------------------------> | USB/SD |
| Database | <--------------------------> | servo ctrl        |           USB/SDIO           +--------+
+----------+         PHP request          | tcp/ip admin ctrl |
                                          | database ctrl     |        move left/right       +-------+
+-----------+    send unlock command      | access admin ctrl | ---------------------------> | Servo |
| Admin/App | --------------------------> | data logging ctrl |             PWM              +-------+
+-----------+       TCP/IP socket         |                   |
                                          +-------------------+
```
