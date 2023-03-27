# 3DSUSBController-Homebrew
3DS Homebrew to turn your 3DS into a USB game controller (3DS homebrew code)  

Requires [3DSUSBController-Server](https://github.com/jcrm1/3DSUSBController-Server) and [3DSUSBController-USB-Device](https://github.com/jcrm1/3DSUSBController-USB-Device)  

Running:
1. Follow the steps in each repo (3DSUSBController-Server, 3DSUSBController-Homebrew, 3DSUSBController-USB-Device) to build/install all required software.
2. Connect Teensy to computer  
3. Start server (`java -jar *.jar <Address> <Port>`)  
4. Start homebrew  
5. Enter address and port of server on 3DS (Example: `192.168.1.12:8080`)

You can exit by pressing Start and Select simultaneously (button down on same frame). I recommend spamming to get it right.
You can use the touchscreen as the IR pointer in Dolphin. I use this as the input expression for Hide: ```Full Axis 2+` > 0.99999 | `Full Axis 3+` > 0.99999```  

Building (requires devkitPro 3ds-dev):  
`make`

Disclosure:
I did not know C going into this project. I went back and cleaned it up a bit, but it's entirely possible that there are glaring errors or bugs in this. However, I have not yet had issues with it.
