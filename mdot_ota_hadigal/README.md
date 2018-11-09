# This repo maintains code for ota program
- The code establishes connection between mdot and gateway
- A lora pkt is sent to gateway after successful connection establishment
- **Note**: This repo does not contain the mbed-os repository from git. Please clone the mbed-os repo from git before compiling. Make sure you have mbed CLI installed on your machine.
- Please refer [main README](https://github.com/hadigal/lora_development#3-configuring-the-eclipse-for-mdot-development) for build process instructions.
- The below screen-shot shows the output for the code:
  * left screen is gateway terminal and right screen is mdot access via COM port:
  ![output:](https://github.com/hadigal/lora_development/blob/master/mdot_ota_hadigal/output.png)
