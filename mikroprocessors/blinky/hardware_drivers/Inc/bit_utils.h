/**
  * @file    bit_utils.h
  * @brief   Bit manipulation utilities for MDR32F9Q2I
  * @version 1.0
  */

#ifndef BIT_UTILS_H
#define BIT_UTILS_H

#include <stdint.h>

/* Smart bit manipulation macros */
#define BIT_SET(reg, mask)      ((reg) |= (mask))
#define BIT_CLR(reg, mask)      ((reg) &= ~(mask)) 
#define BIT_TGL(reg, mask)      ((reg) ^= (mask))
#define BIT_GET(reg, mask)      (((reg) & (mask)) != 0)

/* Bit field manipulation macros */
#define BITFIELD_SET(reg, mask, value, pos)  ((reg) = ((reg) & ~(mask)) | ((value) << (pos)))
#define BITFIELD_GET(reg, mask, pos)         (((reg) & (mask)) >> (pos))

#endif /* BIT_UTILS_H */