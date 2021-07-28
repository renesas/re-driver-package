/***********************************************************************************************************************
* DISCLAIMER
* This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No
* other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
* applicable laws, including copyright laws.
* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
* THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM
* EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES
* SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS
* SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of
* this software. By using this software, you agree to the additional terms and conditions found by accessing the
* following link:
* http://www.renesas.com/disclaimer
*
* Copyright (C) 2018-2019 Renesas Electronics Corporation. All rights reserved.
***********************************************************************************************************************/
/***********************************************************************************************************************
* File Name    : r_flash_re01_1500kb.h
* Version      : 1.00
* Description  : This is a private header file used internally by the FLASH API module. It should not be modified or
*                included by the user in their application.
***********************************************************************************************************************/
/***********************************************************************************************************************
* History : DD.MM.YYYY Version Description
*         : 04.10.2018 0.51    First Release
*         : 29.11.2018 0.60    +Add enumaration definition for block address
*                              +Update Max Programming Time
*                              +Update Max Erasure Time
*         : 22.07.2019 1.00    Change the product name to RE Family
*                              - Change file name to r_flash_re01_1500kb.h
*                              - Change definition name
*                              - Update comments
***********************************************************************************************************************/

/*******************************************************************************************************************//**
 * @addtogroup grp_device_hal_flash
 * @{
 **********************************************************************************************************************/

#ifndef RE01_1500KB_FLASH_PRIVATE_HEADER_FILE
#define RE01_1500KB_FLASH_PRIVATE_HEADER_FILE

#include "r_flash_cfg.h"
#include <stdint.h>
#include <stdbool.h>

/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/
/* Null argument definitions. */
#define FLASH_NO_FUNC                 ((void (*)(void *))0x10000000)  ///< Reserved space on RE01(1500KB products)
#define FLASH_NO_PTR                  ((void *)0x10000000)            ///< Reserved space on RE01(1500KB products)

#define FLASH_CF_MIN_PGM_SIZE_256_BYTES (256)           ///<  Minimum Code Flash programming size (256 bytes)
#define FLASH_CF_MIN_PGM_SIZE_8_BYTES   (8)             ///<  Minimum Code Flash programming size (8 bytes)
#define FLASH_NUM_BLOCKS_CF             (384)           ///<  Number of Code Flash blocks (384 blocks)
#define FLASH_ROM_SIZE_BYTES            (1572864)       ///<  Flash ROM size (1.5 MB)
#define FLASH_RAM_SIZE_BYTES            (262144)        ///<  Flash RAM size (256 KB)

#define FLASH_CF_BLOCK_SIZE             (4096)          ///<  Code Flash block size (4 KB)

#define FLASH_RAM_END_ADDRESS           (0x2003FFFF)    ///<  End of RAM address

/***********************************************************************************************************************
Typedef definitions
***********************************************************************************************************************/
typedef enum
{
    FLASH_CF_BLOCK_0       = 0x00000000,    /*   4KB: 0x00000000 - 0x00000FFF */
    FLASH_CF_BLOCK_1       = 0x00001000,    /*   4KB: 0x00001000 - 0x00001FFF */
    FLASH_CF_BLOCK_2       = 0x00002000,    /*   4KB: 0x00002000 - 0x00002FFF */
    FLASH_CF_BLOCK_3       = 0x00003000,    /*   4KB: 0x00003000 - 0x00003FFF */
    FLASH_CF_BLOCK_4       = 0x00004000,    /*   4KB: 0x00004000 - 0x00004FFF */
    FLASH_CF_BLOCK_5       = 0x00005000,    /*   4KB: 0x00005000 - 0x00005FFF */
    FLASH_CF_BLOCK_6       = 0x00006000,    /*   4KB: 0x00006000 - 0x00006FFF */
    FLASH_CF_BLOCK_7       = 0x00007000,    /*   4KB: 0x00007000 - 0x00007FFF */
    FLASH_CF_BLOCK_8       = 0x00008000,    /*   4KB: 0x00008000 - 0x00008FFF */
    FLASH_CF_BLOCK_9       = 0x00009000,    /*   4KB: 0x00009000 - 0x00009FFF */
    FLASH_CF_BLOCK_10      = 0x0000A000,    /*   4KB: 0x0000A000 - 0x0000AFFF */
    FLASH_CF_BLOCK_11      = 0x0000B000,    /*   4KB: 0x0000B000 - 0x0000BFFF */
    FLASH_CF_BLOCK_12      = 0x0000C000,    /*   4KB: 0x0000C000 - 0x0000CFFF */
    FLASH_CF_BLOCK_13      = 0x0000D000,    /*   4KB: 0x0000D000 - 0x0000DFFF */
    FLASH_CF_BLOCK_14      = 0x0000E000,    /*   4KB: 0x0000E000 - 0x0000EFFF */
    FLASH_CF_BLOCK_15      = 0x0000F000,    /*   4KB: 0x0000F000 - 0x0000FFFF */
    FLASH_CF_BLOCK_16      = 0x00010000,    /*   4KB: 0x00010000 - 0x00010FFF */
    FLASH_CF_BLOCK_17      = 0x00011000,    /*   4KB: 0x00011000 - 0x00011FFF */
    FLASH_CF_BLOCK_18      = 0x00012000,    /*   4KB: 0x00012000 - 0x00012FFF */
    FLASH_CF_BLOCK_19      = 0x00013000,    /*   4KB: 0x00013000 - 0x00013FFF */
    FLASH_CF_BLOCK_20      = 0x00014000,    /*   4KB: 0x00014000 - 0x00014FFF */
    FLASH_CF_BLOCK_21      = 0x00015000,    /*   4KB: 0x00015000 - 0x00015FFF */
    FLASH_CF_BLOCK_22      = 0x00016000,    /*   4KB: 0x00016000 - 0x00016FFF */
    FLASH_CF_BLOCK_23      = 0x00017000,    /*   4KB: 0x00017000 - 0x00017FFF */
    FLASH_CF_BLOCK_24      = 0x00018000,    /*   4KB: 0x00018000 - 0x00018FFF */
    FLASH_CF_BLOCK_25      = 0x00019000,    /*   4KB: 0x00019000 - 0x00019FFF */
    FLASH_CF_BLOCK_26      = 0x0001A000,    /*   4KB: 0x0001A000 - 0x0001AFFF */
    FLASH_CF_BLOCK_27      = 0x0001B000,    /*   4KB: 0x0001B000 - 0x0001BFFF */
    FLASH_CF_BLOCK_28      = 0x0001C000,    /*   4KB: 0x0001C000 - 0x0001CFFF */
    FLASH_CF_BLOCK_29      = 0x0001D000,    /*   4KB: 0x0001D000 - 0x0001DFFF */
    FLASH_CF_BLOCK_30      = 0x0001E000,    /*   4KB: 0x0001E000 - 0x0001EFFF */
    FLASH_CF_BLOCK_31      = 0x0001F000,    /*   4KB: 0x0001F000 - 0x0001FFFF */
    FLASH_CF_BLOCK_32      = 0x00020000,    /*   4KB: 0x00020000 - 0x00020FFF */
    FLASH_CF_BLOCK_33      = 0x00021000,    /*   4KB: 0x00021000 - 0x00021FFF */
    FLASH_CF_BLOCK_34      = 0x00022000,    /*   4KB: 0x00022000 - 0x00022FFF */
    FLASH_CF_BLOCK_35      = 0x00023000,    /*   4KB: 0x00023000 - 0x00023FFF */
    FLASH_CF_BLOCK_36      = 0x00024000,    /*   4KB: 0x00024000 - 0x00024FFF */
    FLASH_CF_BLOCK_37      = 0x00025000,    /*   4KB: 0x00025000 - 0x00025FFF */
    FLASH_CF_BLOCK_38      = 0x00026000,    /*   4KB: 0x00026000 - 0x00026FFF */
    FLASH_CF_BLOCK_39      = 0x00027000,    /*   4KB: 0x00027000 - 0x00027FFF */
    FLASH_CF_BLOCK_40      = 0x00028000,    /*   4KB: 0x00028000 - 0x00028FFF */
    FLASH_CF_BLOCK_41      = 0x00029000,    /*   4KB: 0x00029000 - 0x00029FFF */
    FLASH_CF_BLOCK_42      = 0x0002A000,    /*   4KB: 0x0002A000 - 0x0002AFFF */
    FLASH_CF_BLOCK_43      = 0x0002B000,    /*   4KB: 0x0002B000 - 0x0002BFFF */
    FLASH_CF_BLOCK_44      = 0x0002C000,    /*   4KB: 0x0002C000 - 0x0002CFFF */
    FLASH_CF_BLOCK_45      = 0x0002D000,    /*   4KB: 0x0002D000 - 0x0002DFFF */
    FLASH_CF_BLOCK_46      = 0x0002E000,    /*   4KB: 0x0002E000 - 0x0002EFFF */
    FLASH_CF_BLOCK_47      = 0x0002F000,    /*   4KB: 0x0002F000 - 0x0002FFFF */
    FLASH_CF_BLOCK_48      = 0x00030000,    /*   4KB: 0x00030000 - 0x00030FFF */
    FLASH_CF_BLOCK_49      = 0x00031000,    /*   4KB: 0x00031000 - 0x00031FFF */
    FLASH_CF_BLOCK_50      = 0x00032000,    /*   4KB: 0x00032000 - 0x00032FFF */
    FLASH_CF_BLOCK_51      = 0x00033000,    /*   4KB: 0x00033000 - 0x00033FFF */
    FLASH_CF_BLOCK_52      = 0x00034000,    /*   4KB: 0x00034000 - 0x00034FFF */
    FLASH_CF_BLOCK_53      = 0x00035000,    /*   4KB: 0x00035000 - 0x00035FFF */
    FLASH_CF_BLOCK_54      = 0x00036000,    /*   4KB: 0x00036000 - 0x00036FFF */
    FLASH_CF_BLOCK_55      = 0x00037000,    /*   4KB: 0x00037000 - 0x00037FFF */
    FLASH_CF_BLOCK_56      = 0x00038000,    /*   4KB: 0x00038000 - 0x00038FFF */
    FLASH_CF_BLOCK_57      = 0x00039000,    /*   4KB: 0x00039000 - 0x00039FFF */
    FLASH_CF_BLOCK_58      = 0x0003A000,    /*   4KB: 0x0003A000 - 0x0003AFFF */
    FLASH_CF_BLOCK_59      = 0x0003B000,    /*   4KB: 0x0003B000 - 0x0003BFFF */
    FLASH_CF_BLOCK_60      = 0x0003C000,    /*   4KB: 0x0003C000 - 0x0003CFFF */
    FLASH_CF_BLOCK_61      = 0x0003D000,    /*   4KB: 0x0003D000 - 0x0003DFFF */
    FLASH_CF_BLOCK_62      = 0x0003E000,    /*   4KB: 0x0003E000 - 0x0003EFFF */
    FLASH_CF_BLOCK_63      = 0x0003F000,    /*   4KB: 0x0003F000 - 0x0003FFFF */
    FLASH_CF_BLOCK_64      = 0x00040000,    /*   4KB: 0x00040000 - 0x00040FFF */
    FLASH_CF_BLOCK_65      = 0x00041000,    /*   4KB: 0x00041000 - 0x00041FFF */
    FLASH_CF_BLOCK_66      = 0x00042000,    /*   4KB: 0x00042000 - 0x00042FFF */
    FLASH_CF_BLOCK_67      = 0x00043000,    /*   4KB: 0x00043000 - 0x00043FFF */
    FLASH_CF_BLOCK_68      = 0x00044000,    /*   4KB: 0x00044000 - 0x00044FFF */
    FLASH_CF_BLOCK_69      = 0x00045000,    /*   4KB: 0x00045000 - 0x00045FFF */
    FLASH_CF_BLOCK_70      = 0x00046000,    /*   4KB: 0x00046000 - 0x00046FFF */
    FLASH_CF_BLOCK_71      = 0x00047000,    /*   4KB: 0x00047000 - 0x00047FFF */
    FLASH_CF_BLOCK_72      = 0x00048000,    /*   4KB: 0x00048000 - 0x00048FFF */
    FLASH_CF_BLOCK_73      = 0x00049000,    /*   4KB: 0x00049000 - 0x00049FFF */
    FLASH_CF_BLOCK_74      = 0x0004A000,    /*   4KB: 0x0004A000 - 0x0004AFFF */
    FLASH_CF_BLOCK_75      = 0x0004B000,    /*   4KB: 0x0004B000 - 0x0004BFFF */
    FLASH_CF_BLOCK_76      = 0x0004C000,    /*   4KB: 0x0004C000 - 0x0004CFFF */
    FLASH_CF_BLOCK_77      = 0x0004D000,    /*   4KB: 0x0004D000 - 0x0004DFFF */
    FLASH_CF_BLOCK_78      = 0x0004E000,    /*   4KB: 0x0004E000 - 0x0004EFFF */
    FLASH_CF_BLOCK_79      = 0x0004F000,    /*   4KB: 0x0004F000 - 0x0004FFFF */
    FLASH_CF_BLOCK_80      = 0x00050000,    /*   4KB: 0x00050000 - 0x00050FFF */
    FLASH_CF_BLOCK_81      = 0x00051000,    /*   4KB: 0x00051000 - 0x00051FFF */
    FLASH_CF_BLOCK_82      = 0x00052000,    /*   4KB: 0x00052000 - 0x00052FFF */
    FLASH_CF_BLOCK_83      = 0x00053000,    /*   4KB: 0x00053000 - 0x00053FFF */
    FLASH_CF_BLOCK_84      = 0x00054000,    /*   4KB: 0x00054000 - 0x00054FFF */
    FLASH_CF_BLOCK_85      = 0x00055000,    /*   4KB: 0x00055000 - 0x00055FFF */
    FLASH_CF_BLOCK_86      = 0x00056000,    /*   4KB: 0x00056000 - 0x00056FFF */
    FLASH_CF_BLOCK_87      = 0x00057000,    /*   4KB: 0x00057000 - 0x00057FFF */
    FLASH_CF_BLOCK_88      = 0x00058000,    /*   4KB: 0x00058000 - 0x00058FFF */
    FLASH_CF_BLOCK_89      = 0x00059000,    /*   4KB: 0x00059000 - 0x00059FFF */
    FLASH_CF_BLOCK_90      = 0x0005A000,    /*   4KB: 0x0005A000 - 0x0005AFFF */
    FLASH_CF_BLOCK_91      = 0x0005B000,    /*   4KB: 0x0005B000 - 0x0005BFFF */
    FLASH_CF_BLOCK_92      = 0x0005C000,    /*   4KB: 0x0005C000 - 0x0005CFFF */
    FLASH_CF_BLOCK_93      = 0x0005D000,    /*   4KB: 0x0005D000 - 0x0005DFFF */
    FLASH_CF_BLOCK_94      = 0x0005E000,    /*   4KB: 0x0005E000 - 0x0005EFFF */
    FLASH_CF_BLOCK_95      = 0x0005F000,    /*   4KB: 0x0005F000 - 0x0005FFFF */
    FLASH_CF_BLOCK_96      = 0x00060000,    /*   4KB: 0x00060000 - 0x00060FFF */
    FLASH_CF_BLOCK_97      = 0x00061000,    /*   4KB: 0x00061000 - 0x00061FFF */
    FLASH_CF_BLOCK_98      = 0x00062000,    /*   4KB: 0x00062000 - 0x00062FFF */
    FLASH_CF_BLOCK_99      = 0x00063000,    /*   4KB: 0x00063000 - 0x00063FFF */
    FLASH_CF_BLOCK_100     = 0x00064000,    /*   4KB: 0x00064000 - 0x00064FFF */
    FLASH_CF_BLOCK_101     = 0x00065000,    /*   4KB: 0x00065000 - 0x00065FFF */
    FLASH_CF_BLOCK_102     = 0x00066000,    /*   4KB: 0x00066000 - 0x00066FFF */
    FLASH_CF_BLOCK_103     = 0x00067000,    /*   4KB: 0x00067000 - 0x00067FFF */
    FLASH_CF_BLOCK_104     = 0x00068000,    /*   4KB: 0x00068000 - 0x00068FFF */
    FLASH_CF_BLOCK_105     = 0x00069000,    /*   4KB: 0x00069000 - 0x00069FFF */
    FLASH_CF_BLOCK_106     = 0x0006A000,    /*   4KB: 0x0006A000 - 0x0006AFFF */
    FLASH_CF_BLOCK_107     = 0x0006B000,    /*   4KB: 0x0006B000 - 0x0006BFFF */
    FLASH_CF_BLOCK_108     = 0x0006C000,    /*   4KB: 0x0006C000 - 0x0006CFFF */
    FLASH_CF_BLOCK_109     = 0x0006D000,    /*   4KB: 0x0006D000 - 0x0006DFFF */
    FLASH_CF_BLOCK_110     = 0x0006E000,    /*   4KB: 0x0006E000 - 0x0006EFFF */
    FLASH_CF_BLOCK_111     = 0x0006F000,    /*   4KB: 0x0006F000 - 0x0006FFFF */
    FLASH_CF_BLOCK_112     = 0x00070000,    /*   4KB: 0x00070000 - 0x00070FFF */
    FLASH_CF_BLOCK_113     = 0x00071000,    /*   4KB: 0x00071000 - 0x00071FFF */
    FLASH_CF_BLOCK_114     = 0x00072000,    /*   4KB: 0x00072000 - 0x00072FFF */
    FLASH_CF_BLOCK_115     = 0x00073000,    /*   4KB: 0x00073000 - 0x00073FFF */
    FLASH_CF_BLOCK_116     = 0x00074000,    /*   4KB: 0x00074000 - 0x00074FFF */
    FLASH_CF_BLOCK_117     = 0x00075000,    /*   4KB: 0x00075000 - 0x00075FFF */
    FLASH_CF_BLOCK_118     = 0x00076000,    /*   4KB: 0x00076000 - 0x00076FFF */
    FLASH_CF_BLOCK_119     = 0x00077000,    /*   4KB: 0x00077000 - 0x00077FFF */
    FLASH_CF_BLOCK_120     = 0x00078000,    /*   4KB: 0x00078000 - 0x00078FFF */
    FLASH_CF_BLOCK_121     = 0x00079000,    /*   4KB: 0x00079000 - 0x00079FFF */
    FLASH_CF_BLOCK_122     = 0x0007A000,    /*   4KB: 0x0007A000 - 0x0007AFFF */
    FLASH_CF_BLOCK_123     = 0x0007B000,    /*   4KB: 0x0007B000 - 0x0007BFFF */
    FLASH_CF_BLOCK_124     = 0x0007C000,    /*   4KB: 0x0007C000 - 0x0007CFFF */
    FLASH_CF_BLOCK_125     = 0x0007D000,    /*   4KB: 0x0007D000 - 0x0007DFFF */
    FLASH_CF_BLOCK_126     = 0x0007E000,    /*   4KB: 0x0007E000 - 0x0007EFFF */
    FLASH_CF_BLOCK_127     = 0x0007F000,    /*   4KB: 0x0007F000 - 0x0007FFFF */
    FLASH_CF_BLOCK_128     = 0x00080000,    /*   4KB: 0x00080000 - 0x00080FFF */
    FLASH_CF_BLOCK_129     = 0x00081000,    /*   4KB: 0x00081000 - 0x00081FFF */
    FLASH_CF_BLOCK_130     = 0x00082000,    /*   4KB: 0x00082000 - 0x00082FFF */
    FLASH_CF_BLOCK_131     = 0x00083000,    /*   4KB: 0x00083000 - 0x00083FFF */
    FLASH_CF_BLOCK_132     = 0x00084000,    /*   4KB: 0x00084000 - 0x00084FFF */
    FLASH_CF_BLOCK_133     = 0x00085000,    /*   4KB: 0x00085000 - 0x00085FFF */
    FLASH_CF_BLOCK_134     = 0x00086000,    /*   4KB: 0x00086000 - 0x00086FFF */
    FLASH_CF_BLOCK_135     = 0x00087000,    /*   4KB: 0x00087000 - 0x00087FFF */
    FLASH_CF_BLOCK_136     = 0x00088000,    /*   4KB: 0x00088000 - 0x00088FFF */
    FLASH_CF_BLOCK_137     = 0x00089000,    /*   4KB: 0x00089000 - 0x00089FFF */
    FLASH_CF_BLOCK_138     = 0x0008A000,    /*   4KB: 0x0008A000 - 0x0008AFFF */
    FLASH_CF_BLOCK_139     = 0x0008B000,    /*   4KB: 0x0008B000 - 0x0008BFFF */
    FLASH_CF_BLOCK_140     = 0x0008C000,    /*   4KB: 0x0008C000 - 0x0008CFFF */
    FLASH_CF_BLOCK_141     = 0x0008D000,    /*   4KB: 0x0008D000 - 0x0008DFFF */
    FLASH_CF_BLOCK_142     = 0x0008E000,    /*   4KB: 0x0008E000 - 0x0008EFFF */
    FLASH_CF_BLOCK_143     = 0x0008F000,    /*   4KB: 0x0008F000 - 0x0008FFFF */
    FLASH_CF_BLOCK_144     = 0x00090000,    /*   4KB: 0x00090000 - 0x00090FFF */
    FLASH_CF_BLOCK_145     = 0x00091000,    /*   4KB: 0x00091000 - 0x00091FFF */
    FLASH_CF_BLOCK_146     = 0x00092000,    /*   4KB: 0x00092000 - 0x00092FFF */
    FLASH_CF_BLOCK_147     = 0x00093000,    /*   4KB: 0x00093000 - 0x00093FFF */
    FLASH_CF_BLOCK_148     = 0x00094000,    /*   4KB: 0x00094000 - 0x00094FFF */
    FLASH_CF_BLOCK_149     = 0x00095000,    /*   4KB: 0x00095000 - 0x00095FFF */
    FLASH_CF_BLOCK_150     = 0x00096000,    /*   4KB: 0x00096000 - 0x00096FFF */
    FLASH_CF_BLOCK_151     = 0x00097000,    /*   4KB: 0x00097000 - 0x00097FFF */
    FLASH_CF_BLOCK_152     = 0x00098000,    /*   4KB: 0x00098000 - 0x00098FFF */
    FLASH_CF_BLOCK_153     = 0x00099000,    /*   4KB: 0x00099000 - 0x00099FFF */
    FLASH_CF_BLOCK_154     = 0x0009A000,    /*   4KB: 0x0009A000 - 0x0009AFFF */
    FLASH_CF_BLOCK_155     = 0x0009B000,    /*   4KB: 0x0009B000 - 0x0009BFFF */
    FLASH_CF_BLOCK_156     = 0x0009C000,    /*   4KB: 0x0009C000 - 0x0009CFFF */
    FLASH_CF_BLOCK_157     = 0x0009D000,    /*   4KB: 0x0009D000 - 0x0009DFFF */
    FLASH_CF_BLOCK_158     = 0x0009E000,    /*   4KB: 0x0009E000 - 0x0009EFFF */
    FLASH_CF_BLOCK_159     = 0x0009F000,    /*   4KB: 0x0009F000 - 0x0009FFFF */
    FLASH_CF_BLOCK_160     = 0x000A0000,    /*   4KB: 0x000A0000 - 0x000A0FFF */
    FLASH_CF_BLOCK_161     = 0x000A1000,    /*   4KB: 0x000A1000 - 0x000A1FFF */
    FLASH_CF_BLOCK_162     = 0x000A2000,    /*   4KB: 0x000A2000 - 0x000A2FFF */
    FLASH_CF_BLOCK_163     = 0x000A3000,    /*   4KB: 0x000A3000 - 0x000A3FFF */
    FLASH_CF_BLOCK_164     = 0x000A4000,    /*   4KB: 0x000A4000 - 0x000A4FFF */
    FLASH_CF_BLOCK_165     = 0x000A5000,    /*   4KB: 0x000A5000 - 0x000A5FFF */
    FLASH_CF_BLOCK_166     = 0x000A6000,    /*   4KB: 0x000A6000 - 0x000A6FFF */
    FLASH_CF_BLOCK_167     = 0x000A7000,    /*   4KB: 0x000A7000 - 0x000A7FFF */
    FLASH_CF_BLOCK_168     = 0x000A8000,    /*   4KB: 0x000A8000 - 0x000A8FFF */
    FLASH_CF_BLOCK_169     = 0x000A9000,    /*   4KB: 0x000A9000 - 0x000A9FFF */
    FLASH_CF_BLOCK_170     = 0x000AA000,    /*   4KB: 0x000AA000 - 0x000AAFFF */
    FLASH_CF_BLOCK_171     = 0x000AB000,    /*   4KB: 0x000AB000 - 0x000ABFFF */
    FLASH_CF_BLOCK_172     = 0x000AC000,    /*   4KB: 0x000AC000 - 0x000ACFFF */
    FLASH_CF_BLOCK_173     = 0x000AD000,    /*   4KB: 0x000AD000 - 0x000ADFFF */
    FLASH_CF_BLOCK_174     = 0x000AE000,    /*   4KB: 0x000AE000 - 0x000AEFFF */
    FLASH_CF_BLOCK_175     = 0x000AF000,    /*   4KB: 0x000AF000 - 0x000AFFFF */
    FLASH_CF_BLOCK_176     = 0x000B0000,    /*   4KB: 0x000B0000 - 0x000B0FFF */
    FLASH_CF_BLOCK_177     = 0x000B1000,    /*   4KB: 0x000B1000 - 0x000B1FFF */
    FLASH_CF_BLOCK_178     = 0x000B2000,    /*   4KB: 0x000B2000 - 0x000B2FFF */
    FLASH_CF_BLOCK_179     = 0x000B3000,    /*   4KB: 0x000B3000 - 0x000B3FFF */
    FLASH_CF_BLOCK_180     = 0x000B4000,    /*   4KB: 0x000B4000 - 0x000B4FFF */
    FLASH_CF_BLOCK_181     = 0x000B5000,    /*   4KB: 0x000B5000 - 0x000B5FFF */
    FLASH_CF_BLOCK_182     = 0x000B6000,    /*   4KB: 0x000B6000 - 0x000B6FFF */
    FLASH_CF_BLOCK_183     = 0x000B7000,    /*   4KB: 0x000B7000 - 0x000B7FFF */
    FLASH_CF_BLOCK_184     = 0x000B8000,    /*   4KB: 0x000B8000 - 0x000B8FFF */
    FLASH_CF_BLOCK_185     = 0x000B9000,    /*   4KB: 0x000B9000 - 0x000B9FFF */
    FLASH_CF_BLOCK_186     = 0x000BA000,    /*   4KB: 0x000BA000 - 0x000BAFFF */
    FLASH_CF_BLOCK_187     = 0x000BB000,    /*   4KB: 0x000BB000 - 0x000BBFFF */
    FLASH_CF_BLOCK_188     = 0x000BC000,    /*   4KB: 0x000BC000 - 0x000BCFFF */
    FLASH_CF_BLOCK_189     = 0x000BD000,    /*   4KB: 0x000BD000 - 0x000BDFFF */
    FLASH_CF_BLOCK_190     = 0x000BE000,    /*   4KB: 0x000BE000 - 0x000BEFFF */
    FLASH_CF_BLOCK_191     = 0x000BF000,    /*   4KB: 0x000BF000 - 0x000BFFFF */
    FLASH_CF_BLOCK_192     = 0x000C0000,    /*   4KB: 0x000C0000 - 0x000C0FFF */
    FLASH_CF_BLOCK_193     = 0x000C1000,    /*   4KB: 0x000C1000 - 0x000C1FFF */
    FLASH_CF_BLOCK_194     = 0x000C2000,    /*   4KB: 0x000C2000 - 0x000C2FFF */
    FLASH_CF_BLOCK_195     = 0x000C3000,    /*   4KB: 0x000C3000 - 0x000C3FFF */
    FLASH_CF_BLOCK_196     = 0x000C4000,    /*   4KB: 0x000C4000 - 0x000C4FFF */
    FLASH_CF_BLOCK_197     = 0x000C5000,    /*   4KB: 0x000C5000 - 0x000C5FFF */
    FLASH_CF_BLOCK_198     = 0x000C6000,    /*   4KB: 0x000C6000 - 0x000C6FFF */
    FLASH_CF_BLOCK_199     = 0x000C7000,    /*   4KB: 0x000C7000 - 0x000C7FFF */
    FLASH_CF_BLOCK_200     = 0x000C8000,    /*   4KB: 0x000C8000 - 0x000C8FFF */
    FLASH_CF_BLOCK_201     = 0x000C9000,    /*   4KB: 0x000C9000 - 0x000C9FFF */
    FLASH_CF_BLOCK_202     = 0x000CA000,    /*   4KB: 0x000CA000 - 0x000CAFFF */
    FLASH_CF_BLOCK_203     = 0x000CB000,    /*   4KB: 0x000CB000 - 0x000CBFFF */
    FLASH_CF_BLOCK_204     = 0x000CC000,    /*   4KB: 0x000CC000 - 0x000CCFFF */
    FLASH_CF_BLOCK_205     = 0x000CD000,    /*   4KB: 0x000CD000 - 0x000CDFFF */
    FLASH_CF_BLOCK_206     = 0x000CE000,    /*   4KB: 0x000CE000 - 0x000CEFFF */
    FLASH_CF_BLOCK_207     = 0x000CF000,    /*   4KB: 0x000CF000 - 0x000CFFFF */
    FLASH_CF_BLOCK_208     = 0x000D0000,    /*   4KB: 0x000D0000 - 0x000D0FFF */
    FLASH_CF_BLOCK_209     = 0x000D1000,    /*   4KB: 0x000D1000 - 0x000D1FFF */
    FLASH_CF_BLOCK_210     = 0x000D2000,    /*   4KB: 0x000D2000 - 0x000D2FFF */
    FLASH_CF_BLOCK_211     = 0x000D3000,    /*   4KB: 0x000D3000 - 0x000D3FFF */
    FLASH_CF_BLOCK_212     = 0x000D4000,    /*   4KB: 0x000D4000 - 0x000D4FFF */
    FLASH_CF_BLOCK_213     = 0x000D5000,    /*   4KB: 0x000D5000 - 0x000D5FFF */
    FLASH_CF_BLOCK_214     = 0x000D6000,    /*   4KB: 0x000D6000 - 0x000D6FFF */
    FLASH_CF_BLOCK_215     = 0x000D7000,    /*   4KB: 0x000D7000 - 0x000D7FFF */
    FLASH_CF_BLOCK_216     = 0x000D8000,    /*   4KB: 0x000D8000 - 0x000D8FFF */
    FLASH_CF_BLOCK_217     = 0x000D9000,    /*   4KB: 0x000D9000 - 0x000D9FFF */
    FLASH_CF_BLOCK_218     = 0x000DA000,    /*   4KB: 0x000DA000 - 0x000DAFFF */
    FLASH_CF_BLOCK_219     = 0x000DB000,    /*   4KB: 0x000DB000 - 0x000DBFFF */
    FLASH_CF_BLOCK_220     = 0x000DC000,    /*   4KB: 0x000DC000 - 0x000DCFFF */
    FLASH_CF_BLOCK_221     = 0x000DD000,    /*   4KB: 0x000DD000 - 0x000DDFFF */
    FLASH_CF_BLOCK_222     = 0x000DE000,    /*   4KB: 0x000DE000 - 0x000DEFFF */
    FLASH_CF_BLOCK_223     = 0x000DF000,    /*   4KB: 0x000DF000 - 0x000DFFFF */
    FLASH_CF_BLOCK_224     = 0x000E0000,    /*   4KB: 0x000E0000 - 0x000E0FFF */
    FLASH_CF_BLOCK_225     = 0x000E1000,    /*   4KB: 0x000E1000 - 0x000E1FFF */
    FLASH_CF_BLOCK_226     = 0x000E2000,    /*   4KB: 0x000E2000 - 0x000E2FFF */
    FLASH_CF_BLOCK_227     = 0x000E3000,    /*   4KB: 0x000E3000 - 0x000E3FFF */
    FLASH_CF_BLOCK_228     = 0x000E4000,    /*   4KB: 0x000E4000 - 0x000E4FFF */
    FLASH_CF_BLOCK_229     = 0x000E5000,    /*   4KB: 0x000E5000 - 0x000E5FFF */
    FLASH_CF_BLOCK_230     = 0x000E6000,    /*   4KB: 0x000E6000 - 0x000E6FFF */
    FLASH_CF_BLOCK_231     = 0x000E7000,    /*   4KB: 0x000E7000 - 0x000E7FFF */
    FLASH_CF_BLOCK_232     = 0x000E8000,    /*   4KB: 0x000E8000 - 0x000E8FFF */
    FLASH_CF_BLOCK_233     = 0x000E9000,    /*   4KB: 0x000E9000 - 0x000E9FFF */
    FLASH_CF_BLOCK_234     = 0x000EA000,    /*   4KB: 0x000EA000 - 0x000EAFFF */
    FLASH_CF_BLOCK_235     = 0x000EB000,    /*   4KB: 0x000EB000 - 0x000EBFFF */
    FLASH_CF_BLOCK_236     = 0x000EC000,    /*   4KB: 0x000EC000 - 0x000ECFFF */
    FLASH_CF_BLOCK_237     = 0x000ED000,    /*   4KB: 0x000ED000 - 0x000EDFFF */
    FLASH_CF_BLOCK_238     = 0x000EE000,    /*   4KB: 0x000EE000 - 0x000EEFFF */
    FLASH_CF_BLOCK_239     = 0x000EF000,    /*   4KB: 0x000EF000 - 0x000EFFFF */
    FLASH_CF_BLOCK_240     = 0x000F0000,    /*   4KB: 0x000F0000 - 0x000F0FFF */
    FLASH_CF_BLOCK_241     = 0x000F1000,    /*   4KB: 0x000F1000 - 0x000F1FFF */
    FLASH_CF_BLOCK_242     = 0x000F2000,    /*   4KB: 0x000F2000 - 0x000F2FFF */
    FLASH_CF_BLOCK_243     = 0x000F3000,    /*   4KB: 0x000F3000 - 0x000F3FFF */
    FLASH_CF_BLOCK_244     = 0x000F4000,    /*   4KB: 0x000F4000 - 0x000F4FFF */
    FLASH_CF_BLOCK_245     = 0x000F5000,    /*   4KB: 0x000F5000 - 0x000F5FFF */
    FLASH_CF_BLOCK_246     = 0x000F6000,    /*   4KB: 0x000F6000 - 0x000F6FFF */
    FLASH_CF_BLOCK_247     = 0x000F7000,    /*   4KB: 0x000F7000 - 0x000F7FFF */
    FLASH_CF_BLOCK_248     = 0x000F8000,    /*   4KB: 0x000F8000 - 0x000F8FFF */
    FLASH_CF_BLOCK_249     = 0x000F9000,    /*   4KB: 0x000F9000 - 0x000F9FFF */
    FLASH_CF_BLOCK_250     = 0x000FA000,    /*   4KB: 0x000FA000 - 0x000FAFFF */
    FLASH_CF_BLOCK_251     = 0x000FB000,    /*   4KB: 0x000FB000 - 0x000FBFFF */
    FLASH_CF_BLOCK_252     = 0x000FC000,    /*   4KB: 0x000FC000 - 0x000FCFFF */
    FLASH_CF_BLOCK_253     = 0x000FD000,    /*   4KB: 0x000FD000 - 0x000FDFFF */
    FLASH_CF_BLOCK_254     = 0x000FE000,    /*   4KB: 0x000FE000 - 0x000FEFFF */
    FLASH_CF_BLOCK_255     = 0x000FF000,    /*   4KB: 0x000FF000 - 0x000FFFFF */
    FLASH_CF_BLOCK_256     = 0x00100000,    /*   4KB: 0x00100000 - 0x00100FFF */
    FLASH_CF_BLOCK_257     = 0x00101000,    /*   4KB: 0x00101000 - 0x00101FFF */
    FLASH_CF_BLOCK_258     = 0x00102000,    /*   4KB: 0x00102000 - 0x00102FFF */
    FLASH_CF_BLOCK_259     = 0x00103000,    /*   4KB: 0x00103000 - 0x00103FFF */
    FLASH_CF_BLOCK_260     = 0x00104000,    /*   4KB: 0x00104000 - 0x00104FFF */
    FLASH_CF_BLOCK_261     = 0x00105000,    /*   4KB: 0x00105000 - 0x00105FFF */
    FLASH_CF_BLOCK_262     = 0x00106000,    /*   4KB: 0x00106000 - 0x00106FFF */
    FLASH_CF_BLOCK_263     = 0x00107000,    /*   4KB: 0x00107000 - 0x00107FFF */
    FLASH_CF_BLOCK_264     = 0x00108000,    /*   4KB: 0x00108000 - 0x00108FFF */
    FLASH_CF_BLOCK_265     = 0x00109000,    /*   4KB: 0x00109000 - 0x00109FFF */
    FLASH_CF_BLOCK_266     = 0x0010A000,    /*   4KB: 0x0010A000 - 0x0010AFFF */
    FLASH_CF_BLOCK_267     = 0x0010B000,    /*   4KB: 0x0010B000 - 0x0010BFFF */
    FLASH_CF_BLOCK_268     = 0x0010C000,    /*   4KB: 0x0010C000 - 0x0010CFFF */
    FLASH_CF_BLOCK_269     = 0x0010D000,    /*   4KB: 0x0010D000 - 0x0010DFFF */
    FLASH_CF_BLOCK_270     = 0x0010E000,    /*   4KB: 0x0010E000 - 0x0010EFFF */
    FLASH_CF_BLOCK_271     = 0x0010F000,    /*   4KB: 0x0010F000 - 0x0010FFFF */
    FLASH_CF_BLOCK_272     = 0x00110000,    /*   4KB: 0x00110000 - 0x00110FFF */
    FLASH_CF_BLOCK_273     = 0x00111000,    /*   4KB: 0x00111000 - 0x00111FFF */
    FLASH_CF_BLOCK_274     = 0x00112000,    /*   4KB: 0x00112000 - 0x00112FFF */
    FLASH_CF_BLOCK_275     = 0x00113000,    /*   4KB: 0x00113000 - 0x00113FFF */
    FLASH_CF_BLOCK_276     = 0x00114000,    /*   4KB: 0x00114000 - 0x00114FFF */
    FLASH_CF_BLOCK_277     = 0x00115000,    /*   4KB: 0x00115000 - 0x00115FFF */
    FLASH_CF_BLOCK_278     = 0x00116000,    /*   4KB: 0x00116000 - 0x00116FFF */
    FLASH_CF_BLOCK_279     = 0x00117000,    /*   4KB: 0x00117000 - 0x00117FFF */
    FLASH_CF_BLOCK_280     = 0x00118000,    /*   4KB: 0x00118000 - 0x00118FFF */
    FLASH_CF_BLOCK_281     = 0x00119000,    /*   4KB: 0x00119000 - 0x00119FFF */
    FLASH_CF_BLOCK_282     = 0x0011A000,    /*   4KB: 0x0011A000 - 0x0011AFFF */
    FLASH_CF_BLOCK_283     = 0x0011B000,    /*   4KB: 0x0011B000 - 0x0011BFFF */
    FLASH_CF_BLOCK_284     = 0x0011C000,    /*   4KB: 0x0011C000 - 0x0011CFFF */
    FLASH_CF_BLOCK_285     = 0x0011D000,    /*   4KB: 0x0011D000 - 0x0011DFFF */
    FLASH_CF_BLOCK_286     = 0x0011E000,    /*   4KB: 0x0011E000 - 0x0011EFFF */
    FLASH_CF_BLOCK_287     = 0x0011F000,    /*   4KB: 0x0011F000 - 0x0011FFFF */
    FLASH_CF_BLOCK_288     = 0x00120000,    /*   4KB: 0x00120000 - 0x00120FFF */
    FLASH_CF_BLOCK_289     = 0x00121000,    /*   4KB: 0x00121000 - 0x00121FFF */
    FLASH_CF_BLOCK_290     = 0x00122000,    /*   4KB: 0x00122000 - 0x00122FFF */
    FLASH_CF_BLOCK_291     = 0x00123000,    /*   4KB: 0x00123000 - 0x00123FFF */
    FLASH_CF_BLOCK_292     = 0x00124000,    /*   4KB: 0x00124000 - 0x00124FFF */
    FLASH_CF_BLOCK_293     = 0x00125000,    /*   4KB: 0x00125000 - 0x00125FFF */
    FLASH_CF_BLOCK_294     = 0x00126000,    /*   4KB: 0x00126000 - 0x00126FFF */
    FLASH_CF_BLOCK_295     = 0x00127000,    /*   4KB: 0x00127000 - 0x00127FFF */
    FLASH_CF_BLOCK_296     = 0x00128000,    /*   4KB: 0x00128000 - 0x00128FFF */
    FLASH_CF_BLOCK_297     = 0x00129000,    /*   4KB: 0x00129000 - 0x00129FFF */
    FLASH_CF_BLOCK_298     = 0x0012A000,    /*   4KB: 0x0012A000 - 0x0012AFFF */
    FLASH_CF_BLOCK_299     = 0x0012B000,    /*   4KB: 0x0012B000 - 0x0012BFFF */
    FLASH_CF_BLOCK_300     = 0x0012C000,    /*   4KB: 0x0012C000 - 0x0012CFFF */
    FLASH_CF_BLOCK_301     = 0x0012D000,    /*   4KB: 0x0012D000 - 0x0012DFFF */
    FLASH_CF_BLOCK_302     = 0x0012E000,    /*   4KB: 0x0012E000 - 0x0012EFFF */
    FLASH_CF_BLOCK_303     = 0x0012F000,    /*   4KB: 0x0012F000 - 0x0012FFFF */
    FLASH_CF_BLOCK_304     = 0x00130000,    /*   4KB: 0x00130000 - 0x00130FFF */
    FLASH_CF_BLOCK_305     = 0x00131000,    /*   4KB: 0x00131000 - 0x00131FFF */
    FLASH_CF_BLOCK_306     = 0x00132000,    /*   4KB: 0x00132000 - 0x00132FFF */
    FLASH_CF_BLOCK_307     = 0x00133000,    /*   4KB: 0x00133000 - 0x00133FFF */
    FLASH_CF_BLOCK_308     = 0x00134000,    /*   4KB: 0x00134000 - 0x00134FFF */
    FLASH_CF_BLOCK_309     = 0x00135000,    /*   4KB: 0x00135000 - 0x00135FFF */
    FLASH_CF_BLOCK_310     = 0x00136000,    /*   4KB: 0x00136000 - 0x00136FFF */
    FLASH_CF_BLOCK_311     = 0x00137000,    /*   4KB: 0x00137000 - 0x00137FFF */
    FLASH_CF_BLOCK_312     = 0x00138000,    /*   4KB: 0x00138000 - 0x00138FFF */
    FLASH_CF_BLOCK_313     = 0x00139000,    /*   4KB: 0x00139000 - 0x00139FFF */
    FLASH_CF_BLOCK_314     = 0x0013A000,    /*   4KB: 0x0013A000 - 0x0013AFFF */
    FLASH_CF_BLOCK_315     = 0x0013B000,    /*   4KB: 0x0013B000 - 0x0013BFFF */
    FLASH_CF_BLOCK_316     = 0x0013C000,    /*   4KB: 0x0013C000 - 0x0013CFFF */
    FLASH_CF_BLOCK_317     = 0x0013D000,    /*   4KB: 0x0013D000 - 0x0013DFFF */
    FLASH_CF_BLOCK_318     = 0x0013E000,    /*   4KB: 0x0013E000 - 0x0013EFFF */
    FLASH_CF_BLOCK_319     = 0x0013F000,    /*   4KB: 0x0013F000 - 0x0013FFFF */
    FLASH_CF_BLOCK_320     = 0x00140000,    /*   4KB: 0x00140000 - 0x00140FFF */
    FLASH_CF_BLOCK_321     = 0x00141000,    /*   4KB: 0x00141000 - 0x00141FFF */
    FLASH_CF_BLOCK_322     = 0x00142000,    /*   4KB: 0x00142000 - 0x00142FFF */
    FLASH_CF_BLOCK_323     = 0x00143000,    /*   4KB: 0x00143000 - 0x00143FFF */
    FLASH_CF_BLOCK_324     = 0x00144000,    /*   4KB: 0x00144000 - 0x00144FFF */
    FLASH_CF_BLOCK_325     = 0x00145000,    /*   4KB: 0x00145000 - 0x00145FFF */
    FLASH_CF_BLOCK_326     = 0x00146000,    /*   4KB: 0x00146000 - 0x00146FFF */
    FLASH_CF_BLOCK_327     = 0x00147000,    /*   4KB: 0x00147000 - 0x00147FFF */
    FLASH_CF_BLOCK_328     = 0x00148000,    /*   4KB: 0x00148000 - 0x00148FFF */
    FLASH_CF_BLOCK_329     = 0x00149000,    /*   4KB: 0x00149000 - 0x00149FFF */
    FLASH_CF_BLOCK_330     = 0x0014A000,    /*   4KB: 0x0014A000 - 0x0014AFFF */
    FLASH_CF_BLOCK_331     = 0x0014B000,    /*   4KB: 0x0014B000 - 0x0014BFFF */
    FLASH_CF_BLOCK_332     = 0x0014C000,    /*   4KB: 0x0014C000 - 0x0014CFFF */
    FLASH_CF_BLOCK_333     = 0x0014D000,    /*   4KB: 0x0014D000 - 0x0014DFFF */
    FLASH_CF_BLOCK_334     = 0x0014E000,    /*   4KB: 0x0014E000 - 0x0014EFFF */
    FLASH_CF_BLOCK_335     = 0x0014F000,    /*   4KB: 0x0014F000 - 0x0014FFFF */
    FLASH_CF_BLOCK_336     = 0x00150000,    /*   4KB: 0x00150000 - 0x00150FFF */
    FLASH_CF_BLOCK_337     = 0x00151000,    /*   4KB: 0x00151000 - 0x00151FFF */
    FLASH_CF_BLOCK_338     = 0x00152000,    /*   4KB: 0x00152000 - 0x00152FFF */
    FLASH_CF_BLOCK_339     = 0x00153000,    /*   4KB: 0x00153000 - 0x00153FFF */
    FLASH_CF_BLOCK_340     = 0x00154000,    /*   4KB: 0x00154000 - 0x00154FFF */
    FLASH_CF_BLOCK_341     = 0x00155000,    /*   4KB: 0x00155000 - 0x00155FFF */
    FLASH_CF_BLOCK_342     = 0x00156000,    /*   4KB: 0x00156000 - 0x00156FFF */
    FLASH_CF_BLOCK_343     = 0x00157000,    /*   4KB: 0x00157000 - 0x00157FFF */
    FLASH_CF_BLOCK_344     = 0x00158000,    /*   4KB: 0x00158000 - 0x00158FFF */
    FLASH_CF_BLOCK_345     = 0x00159000,    /*   4KB: 0x00159000 - 0x00159FFF */
    FLASH_CF_BLOCK_346     = 0x0015A000,    /*   4KB: 0x0015A000 - 0x0015AFFF */
    FLASH_CF_BLOCK_347     = 0x0015B000,    /*   4KB: 0x0015B000 - 0x0015BFFF */
    FLASH_CF_BLOCK_348     = 0x0015C000,    /*   4KB: 0x0015C000 - 0x0015CFFF */
    FLASH_CF_BLOCK_349     = 0x0015D000,    /*   4KB: 0x0015D000 - 0x0015DFFF */
    FLASH_CF_BLOCK_350     = 0x0015E000,    /*   4KB: 0x0015E000 - 0x0015EFFF */
    FLASH_CF_BLOCK_351     = 0x0015F000,    /*   4KB: 0x0015F000 - 0x0015FFFF */
    FLASH_CF_BLOCK_352     = 0x00160000,    /*   4KB: 0x00160000 - 0x00160FFF */
    FLASH_CF_BLOCK_353     = 0x00161000,    /*   4KB: 0x00161000 - 0x00161FFF */
    FLASH_CF_BLOCK_354     = 0x00162000,    /*   4KB: 0x00162000 - 0x00162FFF */
    FLASH_CF_BLOCK_355     = 0x00163000,    /*   4KB: 0x00163000 - 0x00163FFF */
    FLASH_CF_BLOCK_356     = 0x00164000,    /*   4KB: 0x00164000 - 0x00164FFF */
    FLASH_CF_BLOCK_357     = 0x00165000,    /*   4KB: 0x00165000 - 0x00165FFF */
    FLASH_CF_BLOCK_358     = 0x00166000,    /*   4KB: 0x00166000 - 0x00166FFF */
    FLASH_CF_BLOCK_359     = 0x00167000,    /*   4KB: 0x00167000 - 0x00167FFF */
    FLASH_CF_BLOCK_360     = 0x00168000,    /*   4KB: 0x00168000 - 0x00168FFF */
    FLASH_CF_BLOCK_361     = 0x00169000,    /*   4KB: 0x00169000 - 0x00169FFF */
    FLASH_CF_BLOCK_362     = 0x0016A000,    /*   4KB: 0x0016A000 - 0x0016AFFF */
    FLASH_CF_BLOCK_363     = 0x0016B000,    /*   4KB: 0x0016B000 - 0x0016BFFF */
    FLASH_CF_BLOCK_364     = 0x0016C000,    /*   4KB: 0x0016C000 - 0x0016CFFF */
    FLASH_CF_BLOCK_365     = 0x0016D000,    /*   4KB: 0x0016D000 - 0x0016DFFF */
    FLASH_CF_BLOCK_366     = 0x0016E000,    /*   4KB: 0x0016E000 - 0x0016EFFF */
    FLASH_CF_BLOCK_367     = 0x0016F000,    /*   4KB: 0x0016F000 - 0x0016FFFF */
    FLASH_CF_BLOCK_368     = 0x00170000,    /*   4KB: 0x00170000 - 0x00170FFF */
    FLASH_CF_BLOCK_369     = 0x00171000,    /*   4KB: 0x00171000 - 0x00171FFF */
    FLASH_CF_BLOCK_370     = 0x00172000,    /*   4KB: 0x00172000 - 0x00172FFF */
    FLASH_CF_BLOCK_371     = 0x00173000,    /*   4KB: 0x00173000 - 0x00173FFF */
    FLASH_CF_BLOCK_372     = 0x00174000,    /*   4KB: 0x00174000 - 0x00174FFF */
    FLASH_CF_BLOCK_373     = 0x00175000,    /*   4KB: 0x00175000 - 0x00175FFF */
    FLASH_CF_BLOCK_374     = 0x00176000,    /*   4KB: 0x00176000 - 0x00176FFF */
    FLASH_CF_BLOCK_375     = 0x00177000,    /*   4KB: 0x00177000 - 0x00177FFF */
    FLASH_CF_BLOCK_376     = 0x00178000,    /*   4KB: 0x00178000 - 0x00178FFF */
    FLASH_CF_BLOCK_377     = 0x00179000,    /*   4KB: 0x00179000 - 0x00179FFF */
    FLASH_CF_BLOCK_378     = 0x0017A000,    /*   4KB: 0x0017A000 - 0x0017AFFF */
    FLASH_CF_BLOCK_379     = 0x0017B000,    /*   4KB: 0x0017B000 - 0x0017BFFF */
    FLASH_CF_BLOCK_380     = 0x0017C000,    /*   4KB: 0x0017C000 - 0x0017CFFF */
    FLASH_CF_BLOCK_381     = 0x0017D000,    /*   4KB: 0x0017D000 - 0x0017DFFF */
    FLASH_CF_BLOCK_382     = 0x0017E000,    /*   4KB: 0x0017E000 - 0x0017EFFF */
    FLASH_CF_BLOCK_383     = 0x0017F000,    /*   4KB: 0x0017F000 - 0x0017FFFF */

    FLASH_CF_BLOCK_END     = 0x0017FFFF,    /*   End of Code Flash Area       */
    FLASH_CF_BLOCK_INVALID = 0x00180000     /*   Block 383 + 4KB              */
} flash_block_address_t;

typedef struct
{
    uint32_t num_blocks;            ///< number of blocks at this size
    uint32_t block_size;            ///< Size of each block
} rom_block_sizes_t;


typedef struct
{
    uint32_t start_addr;            ///< starting address for this block section
    uint32_t end_addr;              ///< ending (up to and including this) address
    uint16_t block_number;          ///< the rom block number for this address queried
    uint32_t thisblock_stAddr;      ///< the starting address for the above block #
    uint32_t block_size;            ///< Size of this block
} rom_block_info_t;

typedef union
{
    unsigned long LONG;
    struct
    {
        unsigned long FAWS :12;
        unsigned long :3;
        unsigned long FSPR :1;
        unsigned long FAWE :12;
        unsigned long :3;
        unsigned long BTFLG :1;
    } BIT;
} fawreg_t;

/** Max Programming Time for 256 bytes (ROM) */
#define WAIT_MAX_ROM_WRITE (50000) // 50ms

/** Max Erasure Time for Code Flash */
#define WAIT_MAX_ERASE_CF (100000) // 100ms

/*******************************************************************************************************************//**
 * @} (end addtogroup grp_device_hal_flash)
 **********************************************************************************************************************/

#endif/* RE01_1500KB__FLASH_PRIVATE_HEADER_FILE */
