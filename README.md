# IoTProject
Final Project For CS244
This is the final project for CS244P designed by Yuchun and Ivan.

Project uses two sonar sensors as people tracker, You can find the logic and state machine
diagram in the formal report along with code. 

==================================== Set Up Hardware ====================================
Set up the hardware using the diagram provided in the report, section II, hardware. You 
can find the resulting product showed in the section IV. To use the code, first deploy the
code under Arduino IDE. Connect the to the Wifi by either type the credentials in the 
serial monitor or hard code the credential in credentials.ino. For more information about 
hardware deployment, you can refer to following link: 
https://docs.microsoft.com/en-us/azure/iot-hub/iot-hub-sparkfun-esp8266-thing-dev-get-started

==================================== Set Up Software ====================================
This product is intended for Microsoft users. Creating to web app in the Azure, one is for
handling and plotting the data, the other is for main website. Create a git repo under 
first web app you just created, and push folder plot_data into the repo. In the second web
app, navigate to the site-->wwwroot folder drag and drop index.html, css, javascript, img 
folders found in main_web folder. (Tips you can drag and drop all this folder using FTP).
When you run the hardware, you can see number of people in the library on the website.
