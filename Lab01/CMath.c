/*
 * File:   CMath.c
 * Author: akwang
 *
 * Created on January 17, 2018, 9:22 PM
 */


#include "xc.h"
#include "CMath.h"

float myabs(float x);

//int main(void) {
//    return 0;
//}

float myabs(float x) {
    if (x < 0) {
        x = -x;
    }
    return x;
}

float enorm(float px, float py, float qx, float qy) {
    float g, e, t, r, s;
    float dx = myabs(qx - px);
    float dy = myabs(qy - py);
    if (dx > dy) {
        g = dx;
        e = dy;
    } else {
        g = dy;
        e = dx;
    }
    int i = 0;
    while(i != 2) {
        t = e / g;
        r = t*t;
        s = r / (4.0 + r);
        g = g + 2 * s * g;
        e = e*s;
        i++;
    }
    return g;
}

float arctangent2(float y, float x) {
    float absY = myabs(y);
    float r, phi;
    if (x > 0) {
        r = (x - absY) / (x + absY);
        phi = 0.1963 * r * r * r - .9817 * r + 3.14159 * 0.75;
    } else {
        r = (x + absY) / (x - absY);
        phi = 0.1963 * r * r * r - .9817 * r + 3.14159 * 0.75;
    }
    if (y < 0) {
        phi = -phi;
    }
    return phi;
}