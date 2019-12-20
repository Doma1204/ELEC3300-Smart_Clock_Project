# RGB Panel
A panel module is a 8x8 WS2812 RGB panel, with a STMF0 chip attached. You can freely connect any number of module and build whatever shape you like. The panel are connected to the mainboard via I2C. It has a panel detection system in order to detect the number of panel connected, their position, shape, and orientation.

## Panel Detection System
There are three phases in panel detection system: Detection Phase, Search Phase and Communication Initiate Phase. The system will use three pins on each side, direction pin, which is different GPIO pin on four sides, SCL pin and SDA pin. The searching progress can be done with breath first search or depth first search, but this project implements the depth first search procedure.

### Detection Phase
In this phase, the four direction pins on four sides will be first initialized as input pin with rising edge trigger, and SCL pin will be initialized as input pin with falling edge trigger. Then, the panel is ready for waiting directional signal from one of the four direction pins. The directional signal is first came from the mainboard. Top, Left, Bottom and Right directional signal have different rising edge time interval to distinguish different direction. The panel will record which direction pin detects a signal and which directional signal it detect to determine it orientation. The SCL pin will change to output pin and pull low for 1ms to notify other panels and mainboard so that other module is acknowledged in order to determine the I2C address of RGB panel. Finally, the direction pin that detect the signal will be reinitialized as output pin and set high to indicate it is under search phase.

### Search Phase
In the search phase, the other three direction pin will be reinitialized to output pin and output corresponding directional signal. If there is a panel found, it should receive a falling edge signal in the SCL pin, and the panel will pause the search phase until the next panel finish the search phase. When the search phase is finished, the direction pin that was previously set high will be set to low to indicate the search phase is completed.

### Communication Initiate Phase
After the detection and search phase, the panel and mainboard will initiate the I2C. Then, mainboard will request the panel direction and orientation information via I2C. Finally, the mainboard to process the information and construct the shape of panel and the number of panel in order to display appropriate information on the display.

After the Panel Detection, the mainboard will know how and what to send the pixel information to dedicated RGB panel.