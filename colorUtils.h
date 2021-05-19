/* colorUtils.h
 */

#ifndef MODE_H
#define MODE_H

typedef enum {
  AVERAGE,
  RED,        /* extract  */
  GREEN,
  BLUE
} Mode;

#endif /* MODE_H */

/**
 * Returns the minimum value among the three given values
 */
int min( int a, int b, int c );

/**
 * Returns the maximum value among the three given values
 */
int max( int a, int b, int c );


