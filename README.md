<!-- LTeX: language=en-US -->

# ✨ Magic Light ✨

This project for the course IC212 at ENSTA aims to implement a simple light synchronization system. Multiple motes are connected to a wireless network, and they can synchronize the color of their LEDs according to multiple events such as the presence of a person in the room, the music being played, etc.

The source code for the AIOT Play mote is located in the `src_aiotplay` directory. The code is inspired by the [AIOT Play projects](https://github.com/aiotsystems/aiot_play_fw.git) of INRIA Paris.

The project can be built in Segger Embedded Studio, by opening the `aiot_play_fw.emProject` file and selecting Magic Light as the active project. Then, you can build and run the project. The main source file is `magic_lights.c`.

The file `node-red_config.json` contains the configuration for the Node-Red flow, to communicate with the mote and use the graphical interface made with Node-Red Dashboard.