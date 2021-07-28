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
* File Name    : r_flash_re01_256kb.h 
* Version      : 1.00
* Description  : This is a private header file used internally by the FLASH API module. It should not be modified or
*                included by the user in their application.
***********************************************************************************************************************/
/***********************************************************************************************************************
* History : DD.MM.YYYY Version Description
*         : 16.12.2019 0.4     First Release (Ported from 1500KB)
*                              - Change the include file names
*                              - Update comments
*         : 16.1.2020  0.8     Update comments
*         : 25.5.2020  1.0     Update comments
***********************************************************************************************************************/
/*******************************************************************************************************************//**
 * @addtogroup grp_device_hal_flash
 * @{
 **********************************************************************************************************************/

#ifndef RE01_256KB_FLASH_PRIVATE_HEADER_FILE
#define RE01_256KB_FLASH_PRIVATE_HEADER_FILE

#include "r_flash_cfg.h"
#include <stdint.h>
#include <stdbool.h>

/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/
/* Null argument definitions. */
#define FLASH_NO_FUNC                 ((void (*)(void *))0x10000000)  ///< Reserved space on RE01(256KB products)
#define FLASH_NO_PTR                  ((void *)0x10000000)            ///< Reserved space on RE01(256KB products)

#define FLASH_CF_MIN_PGM_SIZE_256_BYTES (256)           ///<  Minimum Code Flash programming size (256 bytes) 
#define FLASH_CF_MIN_PGM_SIZE_8_BYTES   (8)             ///<  Minimum Code Flash programming size (8 bytes)
#define FLASH_NUM_BLOCKS_CF             (64)           ///<  Number of Code Flash blocks (64 blocks)
#define FLASH_ROM_SIZE_BYTES            (262144)       ///<  Flash ROM size (256 KB)
#define FLASH_RAM_SIZE_BYTES            (131072)        ///<  Flash RAM size (128 KB)

#define FLASH_CF_BLOCK_SIZE             (4096)          ///<  Code Flash block size (4 KB)  

#define FLASH_RAM_END_ADDRESS           (0x2001FFFF)    ///<  End of RAM address

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

    FLASH_CF_BLOCK_END     = 0x0003FFFF,    /*   End of Code Flash Area       */
    FLASH_CF_BLOCK_INVALID = 0x00040000     /*   Block 63 + 4KB              */
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

#endif /* RE01_256KB__FLASH_PRIVATE_HEADER_FILE */
