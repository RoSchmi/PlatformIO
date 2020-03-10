/**
* @file         Reform_uint16_2_float32.h
* @version      0
* @date         2020-01_27
* @author       RoSchmi
* 
*
* @description
*       Function converts two Uint16 Integers into their float32 equivalent 
*
*/

#pragma once

#ifndef _REFORM_UINT16_2_FLOAT32_H_
#define _REFORM_UINT16_2_FLOAT32_H_
#endif

#include <inttypes.h>

typedef struct
{
    bool validity;         /*defines if the result is valid */
    float value;           /*the result*/   
}
int16_2_float_function_result;


    class Reform_uint16_2_float32
    { 
    public:
        
        //float reform_uint16_2_float32(uint16_t up1, uint16_t u2); 
        int16_2_float_function_result reform_uint16_2_float32(uint16_t u1, uint16_t u2);
    };
