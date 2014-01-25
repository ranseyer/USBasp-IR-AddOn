USBasp-IR-AddOn
===============

IR-Reciver  Extension for USBASP


ir_control, 1.0.0 (20.05.2012) control the "USB IR Remote Receiver"
 (www.mikrocontroller.net/articles/USB_IR_Remote_Receiver).

ir_control [-h] | [-V] | [[[-d] | [-p[1|0]] | [-rCOUNT] | [-s] | [-t]] [-i]]

  -h              print this help and exit
  -V              print version and exit

  -d              delete the PowerOn IR code.
                  The next received code becomes the new PowerOn code
  -p [1|0]        set PowerOn (ON=1/OFF=0)
  -r COUNT        set repeat count (default=5)
  -s              status of: PowerOn (ON=1/OFF=0)
  -i              ignore device name. (just checks the vendor:device ID)
                  needs one of the other options.
