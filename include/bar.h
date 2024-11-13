#ifndef __BAR__H__
#define __BAR__H__

#include <stdint.h>

#include "util.h"


enum 
BarSides
{
    BarSideLeft, 
    BarSideRight, 
    BarSideTop, 
    BarSideBottom,
};



struct Client;
struct Monitor;


/* checks if a client could be a bar */
uint32_t NonNull COULDBEBAR(struct Client *c, uint8_t strut);
/* Gets the bar side in which it is relative to the screen */
enum BarSides NonNullAll GETBARSIDE(struct Monitor *m, struct Client *bar, uint8_t get_prev_side);

/* Checks given the provided information if a window is eligible to be a new bar.
 * if it is then it becomes the new bar.
 * RETURN: EXIT_SUCCESS on Success.
 * RETURN: EXIT_FAILURE on no new bar (Failure).
*/
uint8_t NonNullAll checknewbar(struct Monitor *m, struct Client *c, const uint8_t has_strut_or_strut_partial);
/* Sets up special data. */
void NonNullAll setupbar(struct Monitor *m, struct Client *bar);
/* updates the bar geometry from the given monitor */
void NonNull updatebargeom(struct Monitor *m);
/* updates the Status Bar Position from given monitor */
void NonNull updatebarpos(struct Monitor *m);

#endif
