AWS Shadow I2S

Prerequisites For Running The Example

Before building the example application, it is necessary to set up and configure your AWS IoT account, properly connect the CC3220 launchpad, as well as configure necessary certificates and device information in the example source files. The top level documentation provides links to Amazon’s instructions, as well as steps for properly configuring your device. Please ensure that you have followed the steps in the top level documentation before building and running the example.

Example Overview

The Shadow Example demonstrates the use of the AWS IoT Thing Shadow.

The example shows how an application can interact with a Thing Shadow by simulating the state of a connected window, which opens or closes automatically, based on the temperature of the room.

You can also interact with the application directly by updating the Thing Shadow in the cloud using the AWS Console web interface. (Note, you can use the AWS Command Line Utility (CLI) to interact with the Shadow, too).

All of the AWS IoT examples use TLS for authentication with the AWS IoT servers. It is therefore necessary to update the application with the current time. This is done in the example by requesting the time from an NTP server at “pool.ntp.org”. If desired, you can update the example to get the time from another NTP server.

Interacting With The Thing Shadow

You can modify the state of the window by changing the desired state of the Thing Shadow to be open or closed.

Once the Thing Shadow is updated, AWS IoT will send a message to the device to inform it of the newly desired state (Note that AWS IoT will only do so if the newly desired state is different than the current state - it sends the “Shadow Delta”).

When the new state is received from the server, the device will then “open” or “close” the simulated connected window (i.e. its “windowOpen” state will change to true or false).

To change the state of the window, log in to your AWS IoT account and go to the AWS IoT console. Once in the console, open the Thing Shadow for your device and edit it. You should see JSON code that looks similar to the following:

    {
      "desired": {
        "windowOpen": false
      },
      "reported": {
        "temperature": 26.5,
        "windowOpen": false
      }
    }
    

You can directly modify the JSON code of the Shadow from the console. Try changing the value of “windowOpen” of the “desired” field to <code>true</code> (or <code>false</code>, depending on its current state). You should then see the new state “Update Accepted” and the window state change in the output of the application.


Alternatively, you can modify the JSON code of the Shadow using the following command in the AWS CLI:


    aws iot-data update-thing-shadow --thing-name myThing --payload "{ \"state\": {\"desired\": { \"windowOpen\": true } } }"

Application Output

The output of the example will appear in your serial terminal session:


    CC3220 has connected to AP and acquired an IP address.
    
    IP Address: 192.168.1.129
    
    Current time: Thu Apr 20 00:15:44 2017
    
    Flashing certificate file ...
     successfully wrote file /cert/ca.der to flash
    
    Flashing certificate file ...
     successfully wrote file /cert/cert.der to flash
    
    Flashing certificate file ...
     successfully wrote file /cert/key.der to flash
    
    
    AWS IoT SDK Version(dev) 2.1.1-
    
    DEBUG:   runAWSClient L#125
    Using rootCA /cert/ca.der
    
    DEBUG:   runAWSClient L#126
    Using clientCRT /cert/cert.der
    
    DEBUG:   runAWSClient L#127
    Using clientKey /cert/key.der
    
    Shadow Init
    
    Shadow Connect
    
    ==========================================================
    
    On Device: window state false
    
    Update Shadow: {"state":{"reported":{"temperature":25.500000,"windowOpen":false}}, "clientToken":"c-sdk-client-id-0"}
    
    ==========================================================
    
    Update Accepted !!
    

