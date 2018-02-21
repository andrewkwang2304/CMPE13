/* 
 * File:   Leds.h
 * Author: andrewkwang
 *
 * Created on February 21, 2018, 1:08 AM
 */

#ifndef LED_H
#define	LED_H

#include <xc.h>


#define LEDS_INIT() do { \
    TRISE = 0x0; \
    LATE = 0x0; \
} while (0)

#define LEDS_GET() LATE

#define LEDS_SET(x) (LATE = (x))
    

#endif	/* LED_H */

