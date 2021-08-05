# About this repository 
This repository provides Software Development Kit (CMSIS Driver Package) for RE family.
Software Development Kit (CMSIS Driver Package) provided by this repository is the same as that provided by renesas web.
There is also an SDK using SC that can be developed using the GUI configurator in the software development kit for RE family.
If you are using SDK using SC, please refer to the link below.   
https://www.renesas.com/software-tool/re-software-development-kit

# Software Development Kit (CMSIS Driver Package)
Software Development Kit (CMSIS Driver Package) for RE family is a driver package that can be used to increase development efficiency. The package includes startup code and drivers for the peripheral functions so users can start system development immediately. In this package, ARM® CMSIS compliant API driver software and Renesas driver software are provided.


# Taerget Devices
RE01 1500KB group,  RE01 256KB group


# Development environment
**・Boards & Kits**  
When using RE01 1500kB group  
https://www.renesas.com/ek-re01-1500kb  
When using RE01 256kB group  
https://www.renesas.com/ek-re01-256kb  
<br>
**・Development Tools**  
The Software Development Kit (CMSIS Driver Package) for RE family supports two environments, GCC and IAR.  
| Environment | IED | compiler^*1 | debugger^*2 |
| ------------- | ------------- | ------------- | ------------- |
| GCC | [e2 studio](https://www.renesas.com/jp/en/software-tool/e-studio)  | GNU C/C++ Compiler | [SEGGER J-Link/J-Link OB](https://www.segger.com/products/debug-probes/j-link/)  ([J-Link software and documentation pack](https://www.segger.com/downloads/jlink/JLink_Windows.exe)),IAR I-Jet |
| IAR | [EWARM](https://www.iar.com/ewarm)  | IAR C/C++ Compiler for Arm | [SEGGER J-Link/J-Link OB](https://www.segger.com/products/debug-probes/j-link/)  ([J-Link software and documentation pack](https://www.segger.com/downloads/jlink/JLink_Windows.exe)), Renesas E2/E2 lite |
*1: It is installed when IED is installed.
*2: When developing with the Evaluation Kit introduced in Boards & Kits, do not need to purchase it separately. J-Link/OB is installed on the Evaluation Kit. 

# How to use the SDK
Please refer to the Getting Started Guide for how to use the SDK. 
The Getting Started is a document that contains the following essential information for development. We recommend that you refer to it when starting development.  
-Procedure/method of using driver (initial clock, pin setting, interrupt setting, program allocation method into RAM, etc.)  
-How to set up the debugger in the development environment  
-trouble shooting  
https://www.renesas.com/document/apn/re01-1500kb-256kb-group-getting-started-guide-development-using-cmsis-package-application-note


# Related page  
RE family Product page: https://www.renesas.com/re  
Software development tools: https://www.renesas.com/re_tools  
Evaluation Kits  
 -EK-RE01 1500KB: https://www.renesas.com/ek-re01-1500kb  
 -EK-RE01 256KB: https://www.renesas.com/ek-re01-256kb  
FAQ  
 -Japanese: https://ja-support.renesas.com/knowledgeBase/category/31135  
 -English: https://en-support.renesas.com/knowledgeBase/category/31133  
