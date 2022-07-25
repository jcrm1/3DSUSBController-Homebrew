# 3DSUSBController-Homebrew
3DS Homebrew to turn your 3DS into a USB game controller (3DS homebrew code)  

Requires ```jcrm1/3DSUSBController-Server``` and ```jcrm1/3DSUSBController-USB-Device```  

Running:
1. Connect USB device  
2. Start server (```java -jar *.jar```)  
3. Start homebrew  

You can exit by pressing Start and Select simultaneously (button down on same frame). I recommend spamming to get it right.
You can use the touchscreen as the IR pointer in Dolphin. I use this as the input expression for Hide: ```Full Axis 2+` > 0.99999 | `Full Axis 3+` > 0.99999```  

Building (requires devkitPro 3ds-dev):  
```make```

Disclosure:
I did not know C going into this project; I still don't know C. It's entirely possible that there are glaring errors or bugs in this. However, I have not yet had issues with it.
