#include <math.h> /* fabsf() */

#include "main.h"
#include "keybinds.h"
#include "hashing.h"
#include "getprop.h"
#include "bar.h"

#include <string.h>
#include <math.h>

extern WM _wm;
extern UserSettings _cfg;
extern XCBAtom netatom[];
extern XCBAtom wmatom[];
extern XCBAtom gtkatom[];
extern XCBAtom motifatom;


/* Macro definitions */

u16 OLDWIDTH(Client *c)         { return (c->oldw + (c->bw * 2)); }
u16 OLDHEIGHT(Client *c)        { return (c->oldw + (c->bw * 2)); }
u16 WIDTH(Client *c)            { return (c->w + (c->bw * 2)); }
u16 HEIGHT(Client *c)           { return (c->h + (c->bw * 2)); } 
/* Our custom states */
u32 ISALWAYSONTOP(Client *c)    { return c->ewmhflags & WStateFlagAbove; }
u32 ISALWAYSONBOTTOM(Client *c) { return c->ewmhflags & WStateFlagBelow; }
u32 WASFLOATING(Client *c)      { return c->flags & ClientFlagWasFloating; }
u32 ISFLOATING(Client *c)       { return c->flags & ClientFlagFloating; }
u32 ISOVERRIDEREDIRECT(Client *c) { return c->flags & ClientFlagOverrideRedirect; }
u32 KEEPFOCUS(Client *c)        { return c->flags & ClientFlagKeepFocus; }
u32 DISABLEBORDER(Client *c)    { return c->flags & ClientFlagDisableBorder; }

u32 WASDOCKEDVERT(Client *c)    {   const i16 wy = c->desktop->mon->wy;
                                    const u16 wh = c->desktop->mon->wh;
                                    const i16 y = c->oldy;
                                    const u16 h = OLDHEIGHT(c);
                                    return (wy == y) && (wh == h);
                                }
u32 WASDOCKEDHORZ(Client *c)    {   const i16 wx = c->desktop->mon->wx;
                                    const u16 ww = c->desktop->mon->ww;
                                    const i16 x = c->oldx;
                                    const u16 w = OLDWIDTH(c);
                                    return (wx == x) && (ww == w);
                                }

u32 WASDOCKED(Client *c)        { return WASDOCKEDVERT(c) && WASDOCKEDHORZ(c); }

u32 DOCKEDVERT(Client *c)       {   const i16 wy = c->desktop->mon->wy;
                                    const u16 wh = c->desktop->mon->wh;
                                    const i16 y = c->y;
                                    const u16 h = HEIGHT(c);
                                    return (wy == y) && (wh == h);
                                }

u32 DOCKEDHORZ(Client *c)       {   const i16 wx = c->desktop->mon->wx;
                                    const u16 ww = c->desktop->mon->ww;
                                    const i16 x = c->x;
                                    const u16 w = WIDTH(c);
                                    return (wx == x) && (ww == w);
                                }
u32 DOCKED(Client *c)           { return DOCKEDVERT(c) && DOCKEDHORZ(c); }



static const unsigned int
__COULD__BE__FLOATING__GEOM__FITS(Client *c, const float WIDTH_RATIO, const float HEIGHT_RATIO)
{
    const Monitor *m = c->desktop->mon;
    return c->w <= (m->mw * WIDTH_RATIO) || c->h <= (m->mh * HEIGHT_RATIO);
}

static float __SIGMOID__SCALING(int i , float k, float z0)
{   return 1.0f / (1.0f + expf(-k * (i - z0)));
}

/* Unfortunatly this seems to kinda not work with some applications, mainly because some set their location AFTER being mapped.
 * We could maybe have a timer or something that would make all configure requests apply this also.
 * Still dont know why, they do this (firefox), wouldnt it look better to do it before? IDK.
 */
enum FloatType COULDBEFLOATINGGEOM(Client *c)  
                                {
                                    const float k = .8f;
                                    const float z0 = 0x0;

                                    float scale;
                                    i32 i;
                                    for(i = 0; i < FLOATINGLAST; ++i)
                                    {   
                                        /* Roughly as follows (k = 1.5f, z0 = 1.5f)
                                         * DefinitelyFloating,      10 %
                                         * ProbablyFloating,        30 %
                                         * CouldBeFloating,         67 %
                                         * ProbablyNotFloating,     90 %
                                         * DefinitelyNotFloating,   97 %
                                         */
                                        scale = __SIGMOID__SCALING(i, k, z0);
                                        /* Debug("%f", scale); */
                                        if(__COULD__BE__FLOATING__GEOM__FITS(c, scale, scale))
                                        {   return (enum FloatType)i;
                                        }
                                    }
                                    if(ISFIXED(c))
                                    {
                                        const float MIN_W_RATIO = .67f;
                                        if(c->maxw <= c->desktop->mon->mw * MIN_W_RATIO || c->maxh <= c->desktop->mon->mh * MIN_W_RATIO)
                                        {   return ProbablyFloating;
                                        }
                                    }
                                    return ProbablyNotFloating;
                                }
u32 
__COULD__BE__FLOATING__POSITION__FITS(const Client *c, float width_ratio, float height_ratio)
{
    const Monitor *m = c->desktop->mon;

    /* Calculate from center */
    const i32 x = c->x + c->w / 2;
    const i32 y = c->y + c->h / 2;

    /* Calculate center point */
    const i32 cx = m->mx + m->mw / 2;
    const i32 cy = m->my + m->mh / 2;

    /* Calculate Distance */
    const i32 cdx = labs(cx - x);
    const i32 cdy = labs(cy - y);

    const u8 isratiox = (cdx / (float)(cx + !cx)) >= width_ratio;
    const u8 isratioy = (cdy / (float)(cy + !cy)) >= height_ratio;

    /*
    Debug0("----------------------------");
    Debug("(%d, %d)", x, y);
    Debug("(%d, %d)", cx, cy);
    Debug("(%d, %d)", cdx, cdy);
    Debug("(%f, %f)", cdx / (float)cx, cdy / (float)cy);
    Debug("(%u, %u)", isratiox, isratioy);
    Debug("SS: %f", width_ratio);
    */
    return isratiox || isratioy;
}
enum FloatType COULDBEFLOATINGPOSITION(Client *c)
                                {
                                    const float k = 0.8f;
                                    const float z0 = 3.6f;

                                    i32 i;
                                    float scale;
                                    for(i = 0; i < FLOATINGLAST; ++i)
                                    {
                                        scale = __SIGMOID__SCALING(i, k, z0);
                                        if(__COULD__BE__FLOATING__POSITION__FITS(c, scale, scale))
                                        {   return (enum FloatType)i;
                                        }
                                    }
                                    return ProbablyNotFloating;
                                }
enum FloatType COULDBEFLOATINGHINTS(Client *c)
                                {
                                    /* This check is mostly for (some) popup windows 
                                     * Mainly those which dont matter, like steams startup display, but are nice to have's.
                                     */
                                    
                                    /* Splash windows are like those "intro" logos they display on some applications, ex: audacity */
                                    if(ISSPLASH(c))
                                    {   
                                        Debug0("Splash Window.");
                                        return ProbablyFloating;
                                    }
                                    /* Modal dialog boxes are just persistent dialog boxes (aka dont focus anything else as they are important) */
                                    else if(ISMODAL(c))
                                    {   
                                        Debug0("Modal Window.");
                                        return ProbablyFloating;
                                    }
                                    /* Popup menus see above */
                                    else if(ISPOPUPMENU(c))
                                    {   
                                        Debug0("Popup Menu");
                                        return ProbablyFloating;
                                    }
                                    /* Dialog boxes are usually floating */
                                    else if(ISDIALOG(c))
                                    {   
                                        Debug0("Dialog Menu");
                                        return ProbablyFloating;
                                    }
                                    /* Notification boxes like "changed music" things ussualy arent too important, but should still be floating */
                                    else if(ISNOTIFICATION(c))
                                    {   
                                        Debug0("Notification.");
                                        return ProbablyFloating;
                                    }
                                    /* These are like modal boxes but less important */
                                    else if(ISCOMBO(c))
                                    {   
                                        Debug0("Combo Menu,");
                                        return DefinitelyFloating;
                                    }
                                    /* Above windows ussually are small utility boxes, that shouldnt cover other content completly */
                                    else if(ISABOVE(c))
                                    {   
                                        Debug0("AlwaysOnTop Window detected.");
                                        return DefinitelyFloating;
                                    }
                                    /* This checks for other non dialog types that sort of work like dialog(s) if not maximized. */
                                    else if(ISUTILITY(c))
                                    {   
                                        Debug0("Util Window detected, maybe picture-in-picture?");
                                        return ProbablyFloating;
                                    }
                                    else if(ISMAXIMIZEDVERT(c) && ISMAXIMIZEDHORZ(c))
                                    {   
                                        Debug0("Maximized Window detected.");
                                        return DefinitelyNotFloating;
                                    }
                                    else if(ISNORMAL(c))
                                    {   
                                        Debug0("Normal Window detected.");
                                        return ProbablyNotFloating;
                                    }
                                    Debug0("Window has no special attributes.");
                                    /* No special attributes return */
                                    return ProbablyNotFloating;
                                }

static bool 
__FLOAT__TYPE__EXTRA__CHECKS(Client *c)
{
    const char *const classname = c->classname;
    const char *const instance = c->instancename;

    /* If they dont have a classname/instancename then likely they are single instance windows */
    if(!classname || !instance)
    {   return false;
    }

    /* Some windows do set their classname/instancename but to the same string which means one of the following.
     * A.) Its the main window, which we shouldnt make floating (duh).
     * B.) It has subwindows but again see above.
     * C.) It sets this to all windows and doesnt have any subwindows.
     * D.) (rarely) Its broken, but probably will be fixed later if their developer cares enough.
     */
    return strcmp(classname, instance);
}


static bool
__FLOAT__TYPE__IS__FLOATING(
        Client *c,
        const enum FloatType hints, 
        const enum FloatType geom, 
        const enum FloatType pos
        )
{
    enum FloatType ret = DefinitelyNotFloating;
    switch(hints)
    {
        case DefinitelyFloating:
        {
            switch(geom)
            {
                case DefinitelyFloating:
                case ProbablyFloating:
                case CouldBeFloating:
                    ret = DefinitelyFloating;
                    break;
                case ProbablyNotFloating:
                    switch(pos)
                    {
                        case DefinitelyFloating:
                        case ProbablyFloating:
                        case CouldBeFloating:
                        case ProbablyNotFloating:
                            ret = ProbablyFloating;
                            break;
                            /* unused */
                        case FLOATINGLAST:
                        case DefinitelyNotFloating:
                            break;
                    }
                    break;
                case DefinitelyNotFloating:
                    switch(pos)
                    {
                        case DefinitelyFloating:
                        case ProbablyFloating:
                        case CouldBeFloating:
                            ret = CouldBeFloating;
                            break;
                            /* unused */
                        case FLOATINGLAST:
                        case ProbablyNotFloating:
                        case DefinitelyNotFloating:
                            break;
                    }
                    break;

                    /* unused */
                case FLOATINGLAST:
                    break;
            }
            break;
        }
        case ProbablyFloating:
        {
            switch(geom)
            {
                case DefinitelyFloating:
                case ProbablyFloating:
                    ret = DefinitelyFloating;
                case CouldBeFloating:
                    switch(pos)
                    {
                        case DefinitelyFloating:
                        case ProbablyFloating:
                        case CouldBeFloating:
                            ret = ProbablyFloating;
                            break;
                            /* unused */
                        case FLOATINGLAST:
                        case ProbablyNotFloating:
                        case DefinitelyNotFloating:
                            if(__FLOAT__TYPE__EXTRA__CHECKS(c))
                            {   ret = CouldBeFloating; 
                            }
                            break;
                    }
                    break;
                case ProbablyNotFloating:
                    switch(pos)
                    {
                        case DefinitelyFloating:
                        case ProbablyFloating:
                            ret = ProbablyFloating;
                            break;
                            /* unused */
                        case FLOATINGLAST:
                        case CouldBeFloating:
                        case ProbablyNotFloating:
                        case DefinitelyNotFloating:
                            if(__FLOAT__TYPE__EXTRA__CHECKS(c))
                            {   ret = CouldBeFloating; 
                            }
                            break;
                    }
                    break;
                case DefinitelyNotFloating:
                    switch(pos)
                    {
                        case DefinitelyFloating:
                            ret = ProbablyFloating;
                            /* unused */
                        case FLOATINGLAST:
                        case ProbablyFloating:
                        case CouldBeFloating:
                        case ProbablyNotFloating:
                            if(__FLOAT__TYPE__EXTRA__CHECKS(c))
                            {   ret = CouldBeFloating; 
                            }
                        case DefinitelyNotFloating:
                            break;
                    }
                    break;

                    /* unused */
                case FLOATINGLAST:
                    break;
            }
        }
        case CouldBeFloating:
        {
            switch(geom)
            {
                case DefinitelyFloating:
                    ret = DefinitelyFloating;
                    break;
                case ProbablyFloating:
                    switch(pos)
                    {   
                        case DefinitelyFloating:
                        case ProbablyFloating:
                        case CouldBeFloating:
                            ret = ProbablyFloating;
                            break;
                            /* unused */
                        case FLOATINGLAST:
                        case ProbablyNotFloating:
                        case DefinitelyNotFloating:
                            if(__FLOAT__TYPE__EXTRA__CHECKS(c))
                            {   ret = ProbablyFloating; 
                            }
                            break;
                    }
                    break;
                case CouldBeFloating:
                    switch(pos)
                    {   
                        case DefinitelyFloating:
                        case ProbablyFloating:
                            ret = ProbablyFloating;
                            break;
                            /* unused */
                        case FLOATINGLAST:
                        case CouldBeFloating:
                        case ProbablyNotFloating:
                        case DefinitelyNotFloating:
                            if(__FLOAT__TYPE__EXTRA__CHECKS(c))
                            {   ret = CouldBeFloating; 
                            }
                            break;
                    }
                    break;
                case ProbablyNotFloating:
                    switch(pos)
                    {   
                        case DefinitelyFloating:
                            ret = ProbablyFloating;
                            break;
                            /* unused */
                        case FLOATINGLAST:
                        case ProbablyFloating:
                        case CouldBeFloating:
                        case ProbablyNotFloating:
                            if(__FLOAT__TYPE__EXTRA__CHECKS(c))
                            {   ret = CouldBeFloating; 
                            }
                        case DefinitelyNotFloating:
                            break;
                    }
                    break;
                case DefinitelyNotFloating:
                    switch(pos)
                    {   
                        case DefinitelyFloating:
                            ret = CouldBeFloating;
                            break;
                            /* unused */
                        case FLOATINGLAST:
                        case ProbablyFloating:
                        case CouldBeFloating:
                            if(__FLOAT__TYPE__EXTRA__CHECKS(c))
                            {   ret = CouldBeFloating; 
                            }
                        case ProbablyNotFloating:
                        case DefinitelyNotFloating:
                            break;
                    }
                    break;
                    /* unused */
                case FLOATINGLAST:
                    break;

            }
            break;
        }
        case ProbablyNotFloating:
        {
            switch(geom)
            {
                case DefinitelyFloating:
                    ret = ProbablyFloating;
                case ProbablyFloating:
                    switch(pos)
                    {
                        case DefinitelyFloating:
                        case ProbablyFloating:
                            ret = CouldBeFloating;
                            break;
                            /* unused */
                        case FLOATINGLAST:
                        case CouldBeFloating:
                        case ProbablyNotFloating:
                            if(__FLOAT__TYPE__EXTRA__CHECKS(c))
                            {   ret = CouldBeFloating; 
                            }
                        case DefinitelyNotFloating:
                            break;
                    }
                    break;
                case CouldBeFloating:
                    switch(pos)
                    {
                        case FLOATINGLAST:
                        case DefinitelyFloating:
                        case ProbablyFloating:
                        case CouldBeFloating:
                            if(__FLOAT__TYPE__EXTRA__CHECKS(c))
                            {   ret = CouldBeFloating; 
                            }
                        case ProbablyNotFloating:
                        case DefinitelyNotFloating:
                            break;
                    }
                case ProbablyNotFloating:
                    switch(pos)
                    {
                        case FLOATINGLAST:
                        case DefinitelyFloating:
                        case ProbablyFloating:
                            if(__FLOAT__TYPE__EXTRA__CHECKS(c))
                            {   ret = CouldBeFloating; 
                            }
                        case CouldBeFloating:
                        case ProbablyNotFloating:
                        case DefinitelyNotFloating:
                            break;
                    }
                    break;
                case DefinitelyNotFloating:
                    switch(pos)
                    {   
                        case FLOATINGLAST:
                        case DefinitelyFloating:
                            if(__FLOAT__TYPE__EXTRA__CHECKS(c))
                            {   ret = CouldBeFloating; 
                            }
                        case ProbablyFloating:
                        case DefinitelyNotFloating:
                        case ProbablyNotFloating:
                        case CouldBeFloating:
                            break;
                    }
                    break;
                case FLOATINGLAST:
                    break;
            }
            break;
        }
        case DefinitelyNotFloating:
        {
            switch(geom)
            {
                case DefinitelyFloating:
                    switch(pos)
                    {
                        case DefinitelyFloating:
                        case ProbablyFloating:
                        case CouldBeFloating:
                            ret = CouldBeFloating;
                            break;
                        case DefinitelyNotFloating:
                            if(__FLOAT__TYPE__EXTRA__CHECKS(c))
                            {   ret = CouldBeFloating; 
                            }
                            break;
                        case ProbablyNotFloating:
                        case FLOATINGLAST:
                        default:
                            break;
                    }
                case ProbablyFloating:
                    switch(pos)
                    {
                        case DefinitelyFloating:
                        case ProbablyFloating:
                            ret = CouldBeFloating;
                            break;
                        case CouldBeFloating:
                            if(__FLOAT__TYPE__EXTRA__CHECKS(c))
                            {   ret = CouldBeFloating; 
                            }
                            break;
                        case DefinitelyNotFloating:
                        case ProbablyNotFloating:
                        case FLOATINGLAST:
                        default:
                            break;
                    }
                    break;
                case CouldBeFloating:
                    switch(pos)
                    {
                        case DefinitelyFloating:
                            ret = CouldBeFloating;
                            break;
                            /* unused */
                        case ProbablyFloating:
                            if(__FLOAT__TYPE__EXTRA__CHECKS(c))
                            {   ret = CouldBeFloating; 
                            }
                            break;
                        case CouldBeFloating:
                        case DefinitelyNotFloating:
                        case ProbablyNotFloating:
                        case FLOATINGLAST:
                        default:
                            break;
                    }
                    break;
                case ProbablyNotFloating:
                case DefinitelyNotFloating:
                    switch(pos)
                    {
                        case DefinitelyFloating:
                            if(__FLOAT__TYPE__EXTRA__CHECKS(c))
                            {   ret = CouldBeFloating; 
                            }
                            break;
                        case ProbablyFloating:
                        case CouldBeFloating:
                        case ProbablyNotFloating:
                        case DefinitelyNotFloating:
                        case FLOATINGLAST:
                        default:
                            break;
                    }
                    break;
                    /* unused */
                case FLOATINGLAST:
                    break;
            }
        }

        /* unused */
        case FLOATINGLAST:
        {   break;
        }
    }
    const Monitor *m = c->desktop->mon;
    Debug("Float state: %d", ret);
    /* UNUSED DUE TO unreliablity.
    switch(ret)
    {
        case DefinitelyFloating:
            break;
        case ProbablyFloating:
            break;
        case CouldBeFloating:
            break;
        case ProbablyNotFloating:
            break;
        case DefinitelyNotFloating:
            break;
        case FLOATINGLAST:
        default:
            break;
    }
    */
    /* check if in the corner */
    if(c->x == m->mx && c->y == m->my)
    {   ret = DefinitelyNotFloating;
    }
    Debug("Is float: %s", ret != DefinitelyNotFloating ? "true" : "false");
    return ret != DefinitelyNotFloating;   
}



bool 
SHOULDBEFLOATING(Client *c) 
                                {
                                    /* Note dont check if ISFIXED(c) as games often set that option */
                                    const enum FloatType htype = COULDBEFLOATINGHINTS(c);
                                    const enum FloatType gtype = COULDBEFLOATINGGEOM(c);
                                    const enum FloatType ptype = COULDBEFLOATINGPOSITION(c);

                                    bool ret = __FLOAT__TYPE__IS__FLOATING(c, htype, gtype, ptype);
                                    Debug("(%d, %d, %d)", htype, gtype, ptype);
                                    if(!ret)
                                    {   Debug("[%s] Was Not Floating", c->wmname ? c->wmname : c->netwmname ? c->netwmname : "NULL");
                                    }
                                    return ret;
                                }
/* This covers some apps being able to DragWindow/ResizeWindow, in toggle.c
 * (semi-frequently) a user might "accidentally" click on them (me) and basically we dont want that window to be floating because of that user error.
 * So this is a leeway sort function.
 */
u32 SHOULDMAXIMIZE(Client *c)   {
                                    if(DOCKED(c))
                                    {   return 0;
                                    }
                                    Monitor *m = c->desktop->mon;
                                    const i16 wx = m->wx;
                                    const i16 wy = m->my;
                                    const i16 mx = m->mx;
                                    const i16 my = m->my;

                                    const u16 ww = m->ww;
                                    const u16 wh = m->wh;
                                    const u16 mw = m->mw;
                                    const u16 mh = m->mh;

                                    const i16 x = c->x;
                                    const i16 y = c->y;
                                    const u16 w = c->w;
                                    const u16 h = c->h;
                                    const u16 w1 = WIDTH(c);
                                    const u16 h1 = HEIGHT(c);


                                    /* leeway, pixels */
                                    const u8 LWY = 2;
                                    const u8 iww = (BETWEEN(w, ww + LWY, ww - LWY)) && (BETWEEN(h, wh + LWY, wh - LWY));
                                    const u8 iwb = (BETWEEN(w1, ww + LWY, ww - LWY)) && (BETWEEN(h1, wh + LWY, wh - LWY));
                                    const u8 imw = (BETWEEN(h, mh + LWY, mh - LWY)) && (BETWEEN(w, mw + LWY, mw - LWY));
                                    const u8 imb = (BETWEEN(h1, mh + LWY, mh - LWY)) && (BETWEEN(h1, mw + LWY, mh + LWY));

                                    /* leeway, pixels */
                                    const u8 LWYC = 15;
                                    const u8 iwx = (BETWEEN(x, wx + LWYC, wx - LWYC)) && (BETWEEN(y, wy + LWYC, wy - LWYC));
                                    const u8 imx = (BETWEEN(x, mx + LWYC, mx - LWYC)) && (BETWEEN(y, my + LWYC, my - LWYC));

                                    return
                                        (iwx && (iww || iwb))
                                        ||
                                        (imx && (imw || imb))
                                    ;
                                }

/* used in manage */
u32 DOCKEDINITIAL(Client *c)    {   Monitor *m = c->desktop->mon;
                                    const i16 wx = m->wx;
                                    const i16 wy = m->my;
                                    const i16 mx = m->mx;
                                    const i16 my = m->my;

                                    const u16 ww = m->ww;
                                    const u16 wh = m->wh;
                                    const u16 mw = m->mw;
                                    const u16 mh = m->mh;

                                    const i16 x = c->x;
                                    const i16 y = c->y;
                                    const u16 w = c->w;
                                    const u16 h = c->h;
                                    const u16 w1 = WIDTH(c);
                                    const u16 h1 = HEIGHT(c);

                                    return
                                        ((wx == x) && (wy == y) && (ww == w) && (wh == h))
                                        ||
                                        ((wx == x) && (wy == y) && (ww == w1) && (wh == h1))
                                        ||
                                        ((mx == x) && (my == y) && (mh == h) && (mw == w))
                                        ||
                                        ((mx == x) && (my == y) && (mh == h1) && (mw == w1))
                                        ;
                                }

u32 SHOULDMANAGE(const XCBWindow window) 
                                {
                                    Client *c = wintoclient(window);
                                    XCBWindow already_managed_window = c ? c->win : 0;
                                    const XCBWindow INVALID_WINDOW[] = 
                                    {
                                        XCB_NONE,
                                        _wm.root,
                                        _wm.wmcheckwin,
                                        already_managed_window,
                                    };

                                    int i;
                                    for(i = 0; i < LENGTH(INVALID_WINDOW); ++i)
                                    {
                                        if(window == INVALID_WINDOW[i])
                                        {   return 0;
                                        }
                                    }

                                    return 1;
                                }
Client *REMOVECLIENTREFERENCES(Client *c)
                                {
                                    detachcompletely(c);
                                    delclienthash(c);
                                    return c;
                                }
u32 ISFIXED(Client *c)          { return (c->minw != 0) && (c->minh != 0) && (c->minw == c->maxw) && (c->minh == c->maxh); }
u32 ISURGENT(Client *c)         { return c->ewmhflags & WStateFlagDemandAttention; }
/* flag */
u32 NEVERFOCUS(Client *c)       { return c->ewmhflags & WStateFlagNeverFocus; }
/* client state 
 * taken from i3, 
 * polybar kinda sucks at us grabbing their buttons, so we choose to never focus "dock" (generally statusbars) type windows.
 * What does polybar do?:
 * It used to break the whole WM, but that was mitagated from another i3 feature of replay pou32ing only the main button(s).
 * Now its only unusable if grabbuttons(), twice, basically the "focus" part of grabbuttons(), which is undisireable.
 */
u32 NEVERHOLDFOCUS(Client *c)   { return NEVERFOCUS(c) || ISDOCK(c);}
u32 ISMAXHORZ(Client *c)        { return WIDTH(c) == c->desktop->mon->ww; }
u32 ISMAXVERT(Client *c)        { return HEIGHT(c) == c->desktop->mon->wh; }
u32 ISVISIBLE(Client *c)        { return (c->desktop->mon->desksel == c->desktop || ISSTICKY(c)) && !(ISHIDDEN(c) || ISMAPICONIC(c)); }
/* TODO: XServer race conditions makes this unsuitable for usage */
__DEPRECATED__ u32 ISMAPPED(Client *c) { return c->flags & ClientFlagMapped; }
u32 SHOWDECOR(Client *c)        { return c->flags & ClientFlagShowDecor; }
u32 ISSELECTED(Client *c)       { return c->desktop->sel == c; }
        
/* EWMH Window types */
u32 ISDESKTOP(Client *c)        { return c->ewmhflags & WTypeFlagDesktop; }
u32 ISDOCK(Client *c)           { return c->ewmhflags & WTypeFlagDock; }
u32 ISTOOLBAR(Client *c)        { return c->ewmhflags & WTypeFlagToolbar; }
u32 ISMENU(Client *c)           { return c->ewmhflags & WTypeFlagMenu; }
u32 ISUTILITY(Client *c)        { return c->ewmhflags & WTypeFlagUtility; }
u32 ISSPLASH(Client *c)         { return c->ewmhflags & WTypeFlagSplash; }
u32 ISDIALOG(Client *c)         { return c->ewmhflags & WTypeFlagDialog; }
u32 ISDROPDOWNMENU(Client *c)   { return c->ewmhflags & WTypeFlagDropdownMenu; }
u32 ISPOPUPMENU(Client *c)      { return c->ewmhflags & WTypeFlagPopupMenu; }
u32 ISTOOLTIP(Client *c)        { return c->ewmhflags & WTypeFlagTooltip; }
u32 ISNOTIFICATION(Client *c)   { return c->ewmhflags & WTypeFlagNotification; }
u32 ISCOMBO(Client *c)          { return c->ewmhflags & WTypeFlagCombo; }
u32 ISDND(Client *c)            { return c->ewmhflags & WTypeFlagDnd; }
u32 ISNORMAL(Client *c)         { return c->ewmhflags & WTypeFlagNormal; }
u32 ISMAPICONIC(Client *c)      { return c->ewmhflags & WStateFlagMapIconic; }
u32 ISMAPNORMAL(Client *c)      { return !ISMAPICONIC(c); }
/* EWMH Window states */
u32 ISMODAL(Client *c)          { return c->ewmhflags & WStateFlagModal; }
u32 ISSTICKY(Client *c)         { return c->ewmhflags & WStateFlagSticky; }
/* DONT USE */
u32 ISMAXIMIZEDVERT(Client *c)  { return c->ewmhflags & WStateFlagMaximizedVert; }
/* DONT USE */
u32 ISMAXIMIZEDHORZ(Client *c)  { return c->ewmhflags & WStateFlagMaximizedHorz; }
u32 ISSHADED(Client *c)         { return c->ewmhflags & WStateFlagShaded; }
u32 SKIPTASKBAR(Client *c)      { return c->ewmhflags & WStateFlagSkipTaskbar; }
u32 SKIPPAGER(Client *c)        { return c->ewmhflags & WStateFlagSkipPager; }
u32 ISHIDDEN(Client *c)         { return c->ewmhflags & WStateFlagHidden; }
u32 ISFULLSCREEN(Client *c)     { return c->ewmhflags & WStateFlagFullscreen; }
u32 ISABOVE(Client *c)          { return c->ewmhflags & WStateFlagAbove; }
u32 ISBELOW(Client *c)          { return c->ewmhflags & WStateFlagBelow; }
u32 DEMANDSATTENTION(Client *c) { return c->ewmhflags & WStateFlagDemandAttention; }
u32 ISFOCUSED(Client *c)        { return c->ewmhflags & WStateFlagFocused; }
u32 WSTATENONE(Client *c)       { return c->ewmhflags == 0; }
/* WM Protocol */
u32 HASWMTAKEFOCUS(Client *c)   { return c->ewmhflags & WStateFlagWMTakeFocus; }
u32 HASWMSAVEYOURSELF(Client *c){ return c->ewmhflags & WStateFlagWMSaveYourself; }
u32 HASWMDELETEWINDOW(Client *c){ return c->ewmhflags & WStateFlagWMDeleteWindow; }

void
applygravity(const enum XCBBitGravity gravity, int32_t *x, int32_t *y, const uint32_t w, const uint32_t h, const uint32_t bw)
{
    /* im a dumbass */
    switch(gravity)
    {
        case XCBNorthGravity:
            *x += w >> 1;
            *y -= bw;
            break;
        /* ICCCM standard states that we should assume XCBNorthWestGravity if gravity not set */
        default:
            /* FALLTHROUGH */
        case XCBForgetGravity:
            /* FALLTHROUGH */
        case XCBNorthWestGravity:
            *x -= bw;
            *y -= bw;
            break;
        case XCBNorthEastGravity:
            *x += w + bw;
            *y -= bw;
            break;
        case XCBWestGravity:
            *x -= bw;
            *y += h >> 1;
            break;
        case XCBEastGravity:
            *x += w + bw;
            *y += h >> 1;
            break;
        case XCBSouthWestGravity:
            *x -= bw;
            *y += h + bw;
            break;
        case XCBSouthEastGravity:
            *x += w + bw;
            *y += h + bw;
            break;
        case XCBSouthGravity:
            *x += w >> 1;
            *y += h + bw;
            break;
        case XCBCenterGravity:
            *x += w >> 1;
            *y += h >> 1;
            break;
        case XCBStaticGravity:
            /* Im not sure how to implement this, but it seems that this only applies to framed windows, (reparented).
             * And would simply just not move them relative to the frame? unless requests to move or something.
             * But since frames arent implemented this doesnt really do anything
             */
            Debug0("Static gravity present?");
            break;
    }
}

uint8_t
applysizehints(Client *c, i32 *x, i32 *y, i32 *width, i32 *height, uint8_t interact)
{
    u8 baseismin;
    const Monitor *m = c->desktop->mon;

    if (interact)
    {
        if (*x > _wm.sw) 
        {   *x = _wm.sw - WIDTH(c);
        }
        if (*y > _wm.sh) 
        {   *y = _wm.sh - HEIGHT(c);
        }
        if (*x + *width + (WIDTH(c) - c->w) < 0)
        {   *x = 0;
        }
        if (*y + *height + (HEIGHT(c) - c->h) < 0)
        {   *y = 0;
        }
    }
    else
    {
        if (*x >= m->wx + m->ww)
        {   *x = m->wx + m->ww - WIDTH(c);
        }
        if (*y >= m->wy + m->wh) 
        {   *y = m->wy + m->wh - HEIGHT(c);
        }
        if (*x + *width + (WIDTH(c) - c->w) <= m->wx) 
        {   *x = m->wx;
        }
        if (*y + *height + (HEIGHT(c) - c->h) <= m->wy) 
        {   *y = m->wy;
        }
    }

    /* see last two sentences in ICCCM 4.1.2.3 */
    baseismin = c->basew == c->minw && c->baseh == c->minh;
    /* temporarily remove base dimensions */
    if (!baseismin)
    {
        *width  -= c->basew;
        *height -= c->baseh;
    }
    /* adjust for aspect limits */
    if (c->mina > 0 && c->maxa > 0)
    {
        if (c->maxa < (float)*width / *height) 
        {   *width = *height * c->maxa + 0.5;
        }
        else if (c->mina < (float)*height / *width) 
        {   *height = *width * c->mina + 0.5;
        }
    }
    /* increment calculation requires this */
    if (baseismin)
    {
        *width  -= c->basew;
        *height -= c->baseh;
    }
    /* adjust for increment value */
    if (c->incw)
    {   *width -= *width % c->incw;
    }
    if (c->inch) 
    {   *height -= *height % c->inch;
    }
    /* restore base dimensions */
    *width = MAX(*width + c->basew, c->minw);
    *height = MAX(*height + c->baseh, c->minh);
    if (c->maxw) 
    {   *width = MIN(*width, c->maxw);
    }
    if (c->maxh) 
    {   *height = MIN(*height, c->maxh);
    }
    return *x != c->x || *y != c->y || *width != c->w || *height != c->h;
}

void
cleanupclient(Client *c)
{
    free(c->wmname);
    free(c->netwmname);
    free(c->classname);
    free(c->instancename);
    free(c->decor);
    free(c->icon);
    free(c);
    c = NULL;
}

void
clientinitcolormap(Client *c, XCBGetWindowAttributes *wa)
{
    if(wa)
    {
        c->colormap = 0;
        updatecolormap(c, wa->colormap);
    }
}

void
clientinitdecor(Client *c)
{
    Decoration *decor = c->decor;

    decor->h = 15;
    decor->w = 10;

    const u8 depth = XCB_COPY_FROM_PARENT;
    const XCBVisual visual = XCBGetScreen(_wm.dpy)->root_visual;
    const u8  class = XCB_WINDOW_CLASS_INPUT_OUTPUT;
    const u32 mask = XCB_CW_BACK_PIXEL | XCB_CW_BORDER_PIXEL;

    XCBCreateWindowValueList va =
    {
        .background_pixel = ~0,
        .border_pixel = 0,
        .override_redirect = 1,
    };

    decor->win = XCBCreateWindow(_wm.dpy, _wm.root, 0, 0, c->w, c->h, 0, depth, class, visual, mask, &va);
}

void
clientinitfloat(Client *c)
{
    if(!SHOULDBEFLOATING(c))
    {   return;
    }
    if(!ISFLOATING(c))
    {   setfloating(c, 1);
    }
}


void 
clientinitgeom(Client *c, XCBWindowGeometry *wg)
{
    /* Give initial values. */
    c->x = c->oldx = 0;
    c->y = c->oldy = 0;
    c->w = c->oldw = _wm.selmon->ww;
    c->h = c->oldh = _wm.selmon->wh;
    c->bw = 0; /* TODO */

    /* If we got attributes apply them. */
    if(wg)
    {   
        /* init geometry */
        c->x = c->oldx = wg->x;
        c->y = c->oldy = wg->y;
        c->w = c->oldw = wg->width;
        c->h = c->oldh = wg->height;
        c->oldbw = wg->border_width;
        /* if no specified border width default to our own. */
        if(wg->border_width)
        {   c->bw = wg->border_width;
        }
    }
}

void 
clientinitwtype(Client *c, XCBWindowProperty *windowtypereply)
{
    if(windowtypereply)
    {
        XCBAtom *data = XCBGetPropertyValue(windowtypereply);
        uint32_t ATOM_LENGTH = 0;
        XCBGetPropertyValueLength(windowtypereply, sizeof(XCBAtom), &ATOM_LENGTH);
        updatewindowtypes(c, data, ATOM_LENGTH);
    }
}

void 
clientinitwstate(Client *c, XCBWindowProperty *windowstatereply)
{
    if(windowstatereply)
    {
        XCBAtom *data = XCBGetPropertyValue(windowstatereply);
        uint32_t ATOM_LENGTH = 0;
        XCBGetPropertyValueLength(windowstatereply, sizeof(XCBAtom), &ATOM_LENGTH);
        updatewindowstates(c, data, ATOM_LENGTH);
    }
}

void 
clientinittrans(Client *c, XCBWindow trans)
{
    Client *t;
    if(trans && (t = wintoclient(trans)))
    {   c->desktop = t->desktop;
    }
    else
    {   c->desktop = _wm.selmon->desksel;
    }
}

void
configure(Client *c)
{
    XCBGenericEvent ev;
    memset(&ev, 0, sizeof(XCBGenericEvent));
    XCBConfigureNotifyEvent *ce = (XCBConfigureNotifyEvent *)&ev;
    ce->response_type = XCB_CONFIGURE_NOTIFY;
    ce->event = _wm.root;
    ce->window = c->win;
    ce->x = c->x;
    ce->y = c->y;
    ce->width = c->w;
    ce->height = c->h;
    ce->border_width = c->bw;
    ce->override_redirect = False;
    XCBSendEvent(_wm.dpy, c->win, False, XCB_EVENT_MASK_STRUCTURE_NOTIFY, (const char *)&ev);
}

Client *
createclient(void)
{
    /* This uses calloc as we are currently testing stuff, but we will juse malloc and zero it out later in production*/
    Client *c = calloc(1, sizeof(Client ));
    Decoration *decor = X11DecorCreate();
    if(!c || !decor)
    {   
        Debug0("Could not allocate memory for client (OutOfMemory).");
        Debug("Client:      %p", (void *)c);
        Debug("Decoration:  %p", (void *)decor);
        free(c);
        free(decor);
        return NULL;
    }
    c->decor = decor;
    c->x = c->y = 0;
    c->w = c->h = 0;
    c->oldx = c->oldy = 0;
    c->oldw = c->oldh = 0;
    c->ewmhflags = 0;
    c->ewmhflags = 0;
    c->bw = c->oldbw = 0;
    c->bcol = 0;
    c->win = 0;
    c->mina = c->maxa = 0;
    c->basew = c->baseh = 0;
    c->incw = c->inch = 0;
    c->maxw = c->maxh = 0;
    c->pid = 0;
    c->desktop = NULL;
    c->wmname = NULL;
    c->netwmname = NULL;
    c->classname = NULL;
    c->instancename = NULL;
    return c;
}

void
focus(Client *c)
{
    Monitor *selmon = _wm.selmon;
    Desktop *desk  = selmon->desksel;
    c = focusrealize(c);
    if(desk->sel && desk->sel != c)
    {   unfocus(desk->sel, 0);
    }
    if(c)
    {
        if(ISURGENT(c))
        {   seturgent(c, 0);
        }

        grabbuttons(c, 1);
        XCBSetWindowBorder(_wm.dpy, c->win, c->bcol);
        setfocus(c);
    }
    else
    {   
        XCBSetInputFocus(_wm.dpy, _wm.root, XCB_INPUT_FOCUS_POINTER_ROOT, XCB_CURRENT_TIME);
        XCBDeleteProperty(_wm.dpy, _wm.root, netatom[NetActiveWindow]);
    }
    desk->sel = c;
    Debug("Focused: [%d]", c ? c->win : 0);
}

Client *
focusrealize(Client *c)
{
    Monitor *selmon = _wm.selmon;
    Desktop *desk  = selmon->desksel;
    if(!c || !ISVISIBLE(c) || NEVERHOLDFOCUS(c))
    {   for(c = startfocus(desk); c && !ISVISIBLE(c) && !KEEPFOCUS(c); c = nextfocus(c));
    }
    if(c)
    {
        if(c->desktop->mon != _wm.selmon)
        {   _wm.selmon = c->desktop->mon;
        }
        if(c->desktop != _wm.selmon->desksel)
        {   setdesktopsel(_wm.selmon, c->desktop);
        }
        detachfocus(c);
        attachfocus(c);
    }
    Debug("Focus Realized: [%d]", c ? c->win : 0);
    return c;
}

void
grabbuttons(Client *c, uint8_t focused)
{
    /* make sure no other client steals our grab */
    xcb_grab_server(_wm.dpy);
    u16 i, j;
    /* numlock is int */
    int modifiers[4] = { 0, XCB_MOD_MASK_LOCK, _wm.numlockmask, _wm.numlockmask|XCB_MOD_MASK_LOCK};
    /* somewhat taken from i3 */
    /* Always grab these to allow for replay pointer when focusing by mouse click */
    u8 gbuttons[3] = { LMB, MMB, RMB };

    /* ungrab any previously grabbed buttons that are ours */
    for(i = 0; i < LENGTH(modifiers); ++i)
    {
        if(!NEVERHOLDFOCUS(c))
        {
            for(j = 0; j < LENGTH(gbuttons); ++j)
            {   XCBUngrabButton(_wm.dpy, gbuttons[j], modifiers[i], c->win);
            }
        }
        for(j = 0; j < LENGTH(buttons); ++j)
        {   XCBUngrabButton(_wm.dpy, buttons[j].button, modifiers[i], c->win);
        }
    }
    if (!focused)
    {
        /* grab focus buttons */
        if(!NEVERHOLDFOCUS(c))
        {
            for (i = 0; i < LENGTH(gbuttons); ++i)
            {
                for (j = 0; j < LENGTH(modifiers); ++j)
                {   XCBGrabButton(_wm.dpy, gbuttons[i], modifiers[j], c->win, False, BUTTONMASK, XCB_GRAB_MODE_SYNC, XCB_GRAB_MODE_SYNC, XCB_NONE, XCB_NONE);
                }
            }
        }
    }
    for (i = 0; i < LENGTH(buttons); ++i)
    {
        for (j = 0; j < LENGTH(modifiers); ++j)
        {
            XCBGrabButton(_wm.dpy, buttons[i].button, 
                    buttons[i].mask | modifiers[j], 
                    c->win, False, BUTTONMASK, 
                    XCB_GRAB_MODE_ASYNC, XCB_GRAB_MODE_SYNC, 
                    XCB_NONE, XCB_NONE);
        }
    }
    xcb_ungrab_server(_wm.dpy);
}

void
grabkeys(void)
{
    u32 i, j, k;
    u32 modifiers[4] = { 0, XCB_MOD_MASK_LOCK, _wm.numlockmask, _wm.numlockmask|XCB_MOD_MASK_LOCK };
    XCBKeyCode *keycodes[LENGTH(keys)];
    XCBUngrabKey(_wm.dpy, XCB_GRAB_ANY, XCB_MOD_MASK_ANY, _wm.root);
    
    /* This grabs all the keys */
    for(i = 0; i < LENGTH(keys); ++i)
    {   keycodes[i] = XCBKeySymbolsGetKeyCode(_wm.syms, keys[i].keysym);
    }
    for(i = 0; i < LENGTH(keys); ++i)
    {
        for(j = 0; keycodes[i][j] != XCB_NO_SYMBOL; ++j)
        {
            if(keys[i].keysym == XCBKeySymbolsGetKeySym(_wm.syms, keycodes[i][j], 0))
            {   
                for(k = 0; k < LENGTH(modifiers); ++k)
                {
                    XCBGrabKey(_wm.dpy, 
                            keycodes[i][j], keys[i].mod | modifiers[k], 
                            _wm.root, True, 
                            XCB_GRAB_MODE_ASYNC, XCB_GRAB_MODE_ASYNC);
                }
            }
        }
    }

    for(i = 0; i < LENGTH(keys); ++i)
    {   free(keycodes[i]);
    }
}

void 
killclient(Client *c, enum KillType type)
{
    if(HASWMSAVEYOURSELF(c))
    {   sendprotocolevent(c, wmatom[WMSaveYourself]);
    }
    if(HASWMDELETEWINDOW(c) && type == Graceful)
    {   sendprotocolevent(c, wmatom[WMDeleteWindow]);
    }
    else
    {
        XCBWindow win = c->win;
        switch(type)
        {
            case Graceful:
                XCBKillClient(_wm.dpy, win);
                break;
            case Safedestroy:
                /* TODO */
                XCBKillClient(_wm.dpy, win);
                break;
            case Destroy:
                XCBDestroyWindow(_wm.dpy, win);
                break;
            default:
                XCBKillClient(_wm.dpy, win);
                break;
        }
        XCBGenericEvent ev;
        memset(&ev, 0, sizeof(XCBGenericEvent));
        XCBUnmapNotifyEvent *unev = (XCBUnmapNotifyEvent *)&ev;
        /* let event handler handle this */
        unev->from_configure = 0;
        unev->response_type = XCB_UNMAP_NOTIFY;
        unev->event = _wm.root;
        unev->window = win;
        XCBSendEvent(_wm.dpy, _wm.root, 0, XCB_EVENT_MASK_SUBSTRUCTURE_NOTIFY, (const char *)&ev);
    }
}

Client *
lastclient(Desktop *desk)
{
    return desk->clast;
}

Client *
lastfocus(Desktop *desk)
{
    return desk->flast;
}

Client *
laststack(Desktop *desk)
{
    return desk->slast;
}

Client *
lastrstack(Desktop *desk)
{
    return desk->rlast;
}

void 
managecleanup(void *replies[ManageClientLAST])
{
    /* Remove *_reply from structures 
     * 'Wipe'
     */
    if(replies[ManageClientClass])
    {   XCBWipeGetWMClass(replies[ManageClientClass]);
    }
    if(replies[ManageClientWMProtocol])
    {   XCBWipeGetWMProtocols(replies[ManageClientWMProtocol]);
    }

    i32 i;
    for(i = 0; i < ManageClientLAST; ++i)
    {   free(replies[i]);
    }
}

void
managerequest(XCBWindow win, XCBCookie requests[ManageClientLAST])
{
    requests[ManageClientAttributes] = XCBGetWindowAttributesCookie(_wm.dpy, win);
    requests[ManageClientGeometry] = XCBGetWindowGeometryCookie(_wm.dpy, win);
    requests[ManageClientTransient] = PropGetTransientCookie(_wm.dpy, win);
    requests[ManageClientWType] = PropGetWindowTypeCookie(_wm.dpy, win);
    requests[ManageClientWState] = PropGetWindowStateCookie(_wm.dpy, win);
    requests[ManageClientSizeHint] = PropGetSizeHintsCookie(_wm.dpy, win);
    requests[ManageClientWMHints] = PropGetWMHintsCookie(_wm.dpy, win);
    requests[ManageClientClass] = PropGetWMClassCookie(_wm.dpy, win);
    requests[ManageClientWMProtocol] = PropGetWMProtocolCookie(_wm.dpy, win);
    requests[ManageClientStrut] = PropGetStrutCookie(_wm.dpy, win);
    requests[ManageClientStrutP] = PropGetStrutpCookie(_wm.dpy, win);
    requests[ManageClientNetWMName] = PropGetNetWMNameCookie(_wm.dpy, win);
    requests[ManageClientWMName] = PropGetWMNameCookie(_wm.dpy, win);
    requests[ManageClientPid] = PropGetPidCookie(_wm.dpy, win);
    requests[ManageClientIcon] = PropGetIconCookie(_wm.dpy, win);
    requests[ManageClientMotif] = PropGetMotifHintsCookie(_wm.dpy, win);
}

void
managereplies(XCBCookie requests[ManageClientLAST], void *replies[ManageClientLAST])
{
    XCBWindowGeometry *wg;
    XCBGetWindowAttributes *waattributes;
    XCBWindowProperty *wtypeunused;
    XCBWindowProperty *stateunused;
    XCBSizeHints hints;
    u8 hintstatus = 0;
    XCBWMHints *wmh;
    XCBWMClass cls = { ._reply = NULL };
    u8 clsstatus = 0;
    XCBWMProtocols wmprotocols = { ._reply = NULL, .atoms_len = 0 };
    u8 wmprotocolsstatus = 0;
    XCBWindowProperty *strutpreply;
    XCBWindowProperty *strutreply;
    XCBWindowProperty *netwmnamereply;
    XCBWindowProperty *wmnamereply;
    XCBWindowProperty *iconreply;
    pid_t pid = 0;
    XCBWindowProperty *motifreply;
    XCBWindow trans;
    u8 transstatus;

    /* wait for replies */
    waattributes = XCBGetWindowAttributesReply(_wm.dpy, requests[ManageClientAttributes]);
    wg = XCBGetWindowGeometryReply(_wm.dpy, requests[ManageClientGeometry]);
    transstatus = XCBGetTransientForHintReply(_wm.dpy, requests[ManageClientTransient], &trans); 
    wtypeunused = XCBGetWindowPropertyReply(_wm.dpy, requests[ManageClientWType]);
    stateunused = XCBGetWindowPropertyReply(_wm.dpy, requests[ManageClientWState]);
    hintstatus = XCBGetWMNormalHintsReply(_wm.dpy, requests[ManageClientSizeHint], &hints);
    wmh = XCBGetWMHintsReply(_wm.dpy, requests[ManageClientWMHints]);
    clsstatus = XCBGetWMClassReply(_wm.dpy, requests[ManageClientClass], &cls);
    wmprotocolsstatus = XCBGetWMProtocolsReply(_wm.dpy, requests[ManageClientWMProtocol], &wmprotocols);
    strutreply = XCBGetWindowPropertyReply(_wm.dpy, requests[ManageClientStrut]);
    strutpreply = XCBGetWindowPropertyReply(_wm.dpy, requests[ManageClientStrutP]);
    netwmnamereply = XCBGetWindowPropertyReply(_wm.dpy, requests[ManageClientNetWMName]);
    wmnamereply = XCBGetWindowPropertyReply(_wm.dpy, requests[ManageClientWMName]);
    iconreply = XCBGetWindowPropertyReply(_wm.dpy, requests[ManageClientIcon]);
    pid = XCBGetPidReply(_wm.dpy, requests[ManageClientPid]);
    motifreply = XCBGetWindowPropertyReply(_wm.dpy, requests[ManageClientMotif]);


    XCBWindow *transreply = NULL;
    XCBSizeHints *sizehintsreply = NULL;
    XCBWMClass *classreply = NULL;
    XCBWMProtocols *wmprotocolsreply = NULL;
    int32_t *pidreply = NULL;

    if(transstatus)
    {   
        transreply = malloc(sizeof(XCBWindow ));
        if(transreply)
        {   *transreply = trans;
        }
    }
    if(hintstatus)
    {
        sizehintsreply = malloc(sizeof(XCBSizeHints ));
        if(sizehintsreply)
        {   *sizehintsreply = hints;
        }
    }
    if(clsstatus)
    {   
        classreply = malloc(sizeof(XCBWMClass ));
        if(classreply)
        {   *classreply = cls;
        }
        else
        {   XCBWipeGetWMClass(&cls);
        }
    }
    if(wmprotocolsstatus)
    {   
        wmprotocolsreply = malloc(sizeof(XCBWMProtocols ));
        if(wmprotocolsreply)
        {   *wmprotocolsreply = wmprotocols;
        }
        else
        {   XCBWipeGetWMProtocols(&wmprotocols);
        }
    }

    const int PID_FAILURE = -1;
    if(pid != PID_FAILURE)
    {   
        pidreply = malloc(sizeof(pid_t ));
        if(pidreply)
        {   *pidreply = pid;
        }
    }

    replies[ManageClientAttributes] = waattributes;
    replies[ManageClientGeometry] = wg;
    replies[ManageClientTransient] = transreply;
    replies[ManageClientWType] = wtypeunused;
    replies[ManageClientWState] = stateunused;
    replies[ManageClientSizeHint] = sizehintsreply;
    replies[ManageClientWMHints] = wmh;
    replies[ManageClientClass] = classreply;
    replies[ManageClientWMProtocol] = wmprotocolsreply;
    replies[ManageClientStrut] = strutreply;
    replies[ManageClientStrutP] = strutpreply;
    replies[ManageClientNetWMName] = netwmnamereply;
    replies[ManageClientWMName] = wmnamereply;
    replies[ManageClientPid] = pidreply;
    replies[ManageClientIcon] = iconreply;
    replies[ManageClientMotif] = motifreply;
}


Client *
manage(XCBWindow win, void *replies[ManageClientLAST])
{
    Monitor *m = NULL;
    Client *c = NULL;
    /* checks */
    if(!SHOULDMANAGE(win))
    {   
        Debug("Cannot manage(): [%u]", win);
        goto FAILURE;
    }
    
    const u16 bw = 0;
    const u32 bcol = 0;
    const u8 showdecor = 1;

    const u32 inputmask = XCB_EVENT_MASK_ENTER_WINDOW|XCB_EVENT_MASK_FOCUS_CHANGE|XCB_EVENT_MASK_PROPERTY_CHANGE|XCB_EVENT_MASK_STRUCTURE_NOTIFY;

    /* we do it here before, because we are waiting for replies and for more memory. */
    c = createclient();

    XCBWindowGeometry *wg = replies[ManageClientGeometry];
    XCBGetWindowAttributes *waattributes = replies[ManageClientAttributes];
    XCBWindowProperty *wtypeunused = replies[ManageClientWType];
    XCBWindowProperty *stateunused = replies[ManageClientWState];
    XCBSizeHints *hints = replies[ManageClientSizeHint];
    XCBWMHints *wmh = replies[ManageClientWMHints];
    XCBWMClass *cls = replies[ManageClientClass]; 
    XCBWMProtocols *wmprotocols = replies[ManageClientWMProtocol];
    XCBWindowProperty *strutpreply = replies[ManageClientStrutP];
    XCBWindowProperty *strutreply = replies[ManageClientStrut];
    XCBWindowProperty *netwmnamereply = replies[ManageClientNetWMName];
    XCBWindowProperty *wmnamereply = replies[ManageClientWMName];
    XCBWindowProperty *iconreply = replies[ManageClientIcon];
    pid_t *pid = replies[ManageClientPid];
    XCBWindowProperty *motifreply = replies[ManageClientMotif];
    XCBWindow *trans = replies[ManageClientTransient];

    u32 *strutp = strutp = strutpreply ? XCBGetWindowPropertyValue(strutpreply) : NULL;
    u32 *strut = strut = strutreply ? XCBGetWindowPropertyValue(strutpreply) : NULL;
    

    if(!c)
    {   goto FAILURE;
    }
    c->win = win;

    if(waattributes && waattributes->override_redirect)
    {   Debug("Override Redirect: [%d]", win);
        /* theoredically we could manage these but they are a hastle to deal with */
        goto FAILURE;
    }

    /* this sets up the desktop which is quite important for some operations */
    clientinitcolormap(c, waattributes);
    clientinittrans(c, trans ? *trans : 0); 
    clientinitgeom(c, wg);
    clientinitwtype(c, wtypeunused);
    clientinitwstate(c, stateunused);
    updatewindowprotocol(c, wmprotocols);
    setfloating(c, !!trans);
    /* Custom stuff */
    setclientpid(c, pid ? *pid : 0);
    setborderwidth(c, bw);
    setbordercolor32(c, bcol);
    setshowdecor(c, showdecor);
    updatetitle(c, getnamefromreply(netwmnamereply), getnamefromreply(wmnamereply));
    updateborder(c);
    updatesizehints(c, hints);
    updateclass(c, cls);
    updatewmhints(c, wmh);
    updatemotifhints(c, motifreply);
    updateicon(c, iconreply);
    /* check if should be floating after, all size hints and other things are set. */
    clientinitfloat(c);
    clientinitdecor(c);
    XCBSelectInput(_wm.dpy, win, inputmask);
    grabbuttons(c, 0);

    m = c->desktop->mon;

    addclienthash(c);
    attach(c);
    attachstack(c);
    attachfocus(c);

    updateclientlist(win, ClientListAdd);
    setwtypemapnormal(c, 1);
    setclientstate(c, XCB_WINDOW_NORMAL_STATE);
    /* add to hash */
    /* propagates border_width, if size doesn't change */
    configure(c);
    /* if its a new bar we dont want to return it as the monitor now manages it */
    if(!checknewbar(m, c, strut || strutp))
    {   c = NULL;
    }
    goto CLEANUP;
FAILURE:
    free(c);
    c = NULL;
CLEANUP:
    return c;
}

void
maximize(Client *c)
{
    maximizehorz(c);
    maximizevert(c);
    Debug("Maximized: %u", c->win);
}

void
maximizehorz(Client *c)
{
    const Monitor *m = c->desktop->mon;
    i32 x = m->wx;
    i32 y = c->y;
    i32 w = m->ww - (WIDTH(c) - c->w);
    i32 h = c->h;
    resize(c, x, y, w, h, 0);
}

void
maximizevert(Client *c)
{
    const Monitor *m = c->desktop->mon;
    i32 x = c->x;
    i32 y = m->my;
    i32 w = c->w;
    i32 h = m->wh - (HEIGHT(c) - c->h);
    resize(c, x, y, w, h, 0);
}

Client *
nextclient(Client *c)
{
    return c ? c->next : c;
}

Client *
nextstack(Client *c)
{
    return c ? c->snext : c;
}

Client *
nextrstack(Client *c)
{
    return c ? c->rnext : c;
}

Client *
nextfocus(Client *c)
{
    return c ? c->fnext : c;
}

Client *
nexttiled(Client *c)
{   
    for(; c && (ISFLOATING(c) || !ISVISIBLE(c)); c = nextstack(c));
    return c;
}

Client *
nextvisible(Client *c)
{
    for(; c && !ISVISIBLE(c); c = c->next);
    return c;
}

Client *
prevclient(Client *c)
{
    return c ? c->prev : c;
}

Client *
prevfocus(Client *c)
{
    return c ? c->fprev : c;
}

Client *
prevstack(Client *c)
{
    return c ? c->sprev : c;
}

Client *
prevrstack(Client *c)
{   
    return c ? c->rprev : c;
}

Client *
prevvisible(Client *c)
{
    while(c && !ISVISIBLE(c))
    {   c = prevclient(c);
    }
    return c;
}

void
resize(Client *c, i32 x, i32 y, i32 width, i32 height, uint8_t interact)
{
    if(applysizehints(c, &x, &y, &width, &height, interact))
    {   resizeclient(c, x, y, width, height);
    }
}

void 
resizeclient(Client *c, int16_t x, int16_t y, uint16_t width, uint16_t height)
{
    u32 mask = 0;

    if(c->x != x)
    {   
        c->oldx = c->x;
        c->x = x;
        mask |= XCB_CONFIG_WINDOW_X;
    }
    if(c->y != y)
    {
        c->oldy = c->y;
        c->y = y;
        mask |= XCB_CONFIG_WINDOW_Y;
    }
    if(c->w != width)
    {
        c->oldw = c->w;
        c->w = width;
        mask |= XCB_CONFIG_WINDOW_WIDTH;
    }
    if(c->h != height)
    {   
        c->oldh = c->h;
        c->h = height;
        mask |= XCB_CONFIG_WINDOW_HEIGHT;
    }

    XCBWindowChanges changes =
    {   
        .x = x,
        .y = y,
        .width = width,
        .height = height,
    };

    /* Process resize requests only to visible clients as to.
     * 1.) Save resources, no need to handle non visible windows.
     * 2.) Incase that the window does get visible make it not appear to be movable (different desktop).
     * 3.) Prevent the window from moving itself back into view, when it should be hidden.
     * 4.) Incase a window does want focus, we switch to that desktop respectively and let showhide() do the work.
     */
    if(ISVISIBLE(c))
    {
        if(mask)
        {   XCBConfigureWindow(_wm.dpy, c->win, mask, &changes);
        }
    }
    else
    {   Debug("[%u] Not visible", c->win);
    }
    /* only send config if changed */
    if(mask)
    {   configure(c);
    }
}

void
resizemove(Client *c, int16_t x, int16_t y, uint8_t interact)
{
    i32 _x = x;
    i32 _y = y;
    i32 width = c->w;
    i32 height = c->h;
    if(applysizehints(c, &_x, &_y, &width, &height, interact))
    {   
        (void)width;
        (void)height;
        resizeclient(c, _x, _y, c->w, c->h);
    }
}

void
sendprotocolevent(Client *c, XCBAtom proto)
{
    XCBGenericEvent ev;
    memset(&ev, 0, sizeof(XCBGenericEvent));
    XCBClientMessageEvent *cev = (XCBClientMessageEvent *)&ev;
    cev->type = wmatom[WMProtocols];
    cev->response_type = XCB_CLIENT_MESSAGE;
    cev->window = c->win;
    cev->format = 32;
    cev->data.data32[0] = proto;
    cev->data.data32[1] = XCB_CURRENT_TIME;
    XCBSendEvent(_wm.dpy, c->win, False, XCB_EVENT_MASK_STRUCTURE_NOTIFY, (const char *)&ev);
}

void
setalwaysontop(Client *c, u8 state)
{
    SETFLAG(c->ewmhflags, WStateFlagAbove, !!state);
}

void
setalwaysonbottom(Client *c, uint8_t state)
{
    SETFLAG(c->ewmhflags, WStateFlagBelow, !!state);
}

void
setborderalpha(Client *c, uint8_t alpha)
{
    c->bcol &= ~(UINT8_MAX << 24);
    c->bcol |= alpha << 24;
}

void
setbordercolor(Client *c, uint8_t red, uint8_t green, uint8_t blue)
{
    const u32 col = blue + (green << 8) + (red << 16);
    setbordercolor32(c, col);
}

void
setbordercolor32(Client *c, uint32_t col)
{   
    const u32 mask = (UINT32_MAX ^ (UINT8_MAX << 24));
    c->bcol &= mask;
    c->bcol |= col & mask;
}

void
setborderwidth(Client *c, uint16_t border_width)
{
    if(!DISABLEBORDER(c))
    {
        c->oldbw = c->bw;
        c->bw = border_width;
    }
}

void
setclientdesktop(Client *c, Desktop *desk)
{
    detachcompletely(c);
    c->desktop = desk;
    attach(c);
    attachstack(c);
    attachfocus(c);
}

void
setclientstate(Client *c, u8 state)
{
    /* Due to windows only having 1 map state we can set this without needing to replace other data */
    const i32 data[2] = { state, XCB_NONE };
    XCBChangeProperty(_wm.dpy, c->win, wmatom[WMState], wmatom[WMState], 32, XCB_PROP_MODE_REPLACE, (unsigned char *)data, 2);
}

void
setclientwtype(Client *c, XCBAtom atom, u8 state)
{
    PropArg arg;
    arg.ui[0] = atom;
    if(state)
    {   PropListenArg(_wm.handler, _wm.dpy, c->win, PropSetWtype, arg);
    }
    else
    {   PropListenArg(_wm.handler, _wm.dpy, c->win, PropUnsetWtype, arg);
    }
}

void
setclientnetstate(Client *c, XCBAtom atom, u8 state)
{
    PropArg arg;
    arg.ui[0] = atom;
    if(state)
    {   PropListenArg(_wm.handler, _wm.dpy, c->win, PropSetWState, arg);
    }
    else
    {   PropListenArg(_wm.handler, _wm.dpy, c->win, PropUnsetWState, arg);
    }
}

void
setdisableborder(Client *c, uint8_t state)
{
    SETFLAG(c->flags, ClientFlagDisableBorder, !!state);
}

void
setclientpid(Client *c, pid_t pid)
{
    c->pid = pid;
}

void
setmapstate(Client *c, enum WMMapState state)
{
    switch(state)
    {
        default:
        case WMMapStateMapped:
            SETFLAG(c->flags, ClientFlagMapped, 1);
            break;
        case WMMapStateUnmapped:
            SETFLAG(c->flags, ClientFlagMapped, 0);
            break;
    }
}

void
setwtypedesktop(Client *c, uint8_t state)
{
    SETFLAG(c->ewmhflags, WTypeFlagDesktop, !!state);
}

void
setwtypedialog(Client *c, uint8_t state)
{
    SETFLAG(c->ewmhflags, WTypeFlagDialog, !!state);
}

void
setwtypedock(Client *c, uint8_t state)
{
    SETFLAG(c->ewmhflags, WTypeFlagDock, !!state);
}

void
setwtypetoolbar(Client *c, uint8_t state)
{
    SETFLAG(c->ewmhflags, WTypeFlagToolbar, !!state);
}

void
setwtypemenu(Client *c, uint8_t state)
{
    SETFLAG(c->ewmhflags, WTypeFlagMenu, !!state);
}

void
setwtypeneverfocus(Client *c, uint8_t state)
{
    SETFLAG(c->ewmhflags, WStateFlagNeverFocus, !!state);
}

void
setwtypeutility(Client *c, uint8_t state)
{
    SETFLAG(c->ewmhflags, WTypeFlagUtility, !!state);
}

void
setwtypesplash(Client *c, uint8_t state)
{
    SETFLAG(c->ewmhflags, WTypeFlagSplash, !!state);
}

void
setwtypedropdownmenu(Client *c, uint8_t state)
{
    SETFLAG(c->ewmhflags, WTypeFlagDropdownMenu, !!state);
}

void
setwtypepopupmenu(Client *c, uint8_t state)
{
    SETFLAG(c->ewmhflags, WTypeFlagPopupMenu, !!state);
}

void
setwtypetooltip(Client *c, uint8_t state)
{
    SETFLAG(c->ewmhflags, WTypeFlagTooltip, !!state);
}

void
setwtypenotification(Client *c, uint8_t state)
{
    SETFLAG(c->ewmhflags, WTypeFlagNotification, !!state);
}

void
setwtypecombo(Client *c, uint8_t state)
{
    SETFLAG(c->ewmhflags, WTypeFlagCombo, !!state);
}

void
setwtypednd(Client *c, uint8_t state)
{
    SETFLAG(c->ewmhflags, WTypeFlagDnd, !!state);
}

void
setwtypenormal(Client *c, uint8_t state)
{
    SETFLAG(c->ewmhflags, WTypeFlagNormal, !!state);
}

void
setwtypemapiconic(Client *c, uint8_t state)
{
    SETFLAG(c->ewmhflags, WStateFlagMapIconic, !!state);
}

void
setwtypemapnormal(Client *c, uint8_t state)
{
    setwtypemapiconic(c, !state);
}

void 
setwmtakefocus(Client *c, uint8_t state)
{
    SETFLAG(c->ewmhflags, WStateFlagWMTakeFocus, !!state);
}

void 
setwmsaveyourself(Client *c, uint8_t state)
{
    SETFLAG(c->ewmhflags, WStateFlagWMSaveYourself, !!state);
}

void 
setwmdeletewindow(Client *c, uint8_t state)
{
    SETFLAG(c->ewmhflags, WStateFlagWMDeleteWindow, !!state);
}

void
setskippager(Client *c, uint8_t state)
{
    SETFLAG(c->ewmhflags, WStateFlagSkipPager, !!state);
}

void
setskiptaskbar(Client *c, uint8_t state)
{
    SETFLAG(c->ewmhflags, WStateFlagSkipTaskbar, !!state);
}

void
setshowdecor(Client *c, uint8_t state)
{
    /* TODO, Implement this.
     * As of now this is not implemented but for the sake of being NetWM Compliant, 
     * We just set the decorations to be 0, (AKA No decorations)
     */
    state = 0;
    enum __FrameExtents
    {
        __FrameExtentsLW,   /* Left "decoration" Width */
        __FrameExtentsRW,   /* Right "decoration" Width */
        __FrameExtentsTW,   /* Top "decoration" Width, AKA the Height */
        __FrameExtentsBW,   /* Bottom "decoration" Width, AKA the Height */
    };
    u32 data[4] = { 0, 0, 0, 0 };
    if(state)
    {   
        if(c->decor->win)
        {   
            Decoration *decor = c->decor;
            XCBMapWindow(_wm.dpy, decor->win);   
            /* as of now not supported but eventually */
            data[0] = 0;
            data[1] = 0;
            data[2] = decor->h;
            data[3] = 0;
        }
    }
    else
    {
        if(c->decor->win)
        {   XCBUnmapWindow(_wm.dpy, c->decor->win);
        }
    }
    SETFLAG(c->flags, ClientFlagShowDecor, !!state);
    XCBChangeProperty(_wm.dpy, c->win, netatom[NetWMFrameExtents], XCB_ATOM_CARDINAL, 32, XCB_PROP_MODE_REPLACE, (unsigned char *)data, 4);
}

void
setfullscreen(Client *c, u8 state)
{
    if(state && !ISFULLSCREEN(c))
    {
        setclientnetstate(c, netatom[NetWMStateFullscreen], 1);
        setborderwidth(c, c->bw);
        setborderwidth(c, 0);
    }
    else if(!state && ISFULLSCREEN(c))
    {
        setclientnetstate(c, netatom[NetWMStateFullscreen], 0);
        setborderwidth(c, c->oldbw);
    }
    updateborderwidth(c);
    SETFLAG(c->ewmhflags, WStateFlagFullscreen, !!state);
}

void
setfloating(Client *c, uint8_t state)
{
    SETFLAG(c->flags, ClientFlagWasFloating, !!(c->flags & ClientFlagFloating));
    SETFLAG(c->flags, ClientFlagFloating, !!state);
}

void
setfocus(Client *c)
{
    if(!NEVERHOLDFOCUS(c))
    {
        XCBSetInputFocus(_wm.dpy, c->win, XCB_INPUT_FOCUS_POINTER_ROOT, XCB_CURRENT_TIME);
        XCBChangeProperty(_wm.dpy, _wm.root, netatom[NetActiveWindow], XCB_ATOM_WINDOW, 32, XCB_PROP_MODE_REPLACE, (unsigned char *)&(c->win), 1);
        setclientnetstate(c, netatom[NetWMStateFocused], 1);
        SETFLAG(c->ewmhflags, WStateFlagFocused, 1);
    }
    if(HASWMTAKEFOCUS(c))
    {   sendprotocolevent(c, wmatom[WMTakeFocus]);
    }
}

void 
sethidden(Client *c, uint8_t state)
{
    SETFLAG(c->ewmhflags, WStateFlagHidden, !!state);
}

void 
setkeepfocus(Client *c, uint8_t state)
{
    SETFLAG(c->flags, ClientFlagKeepFocus, !!state);
}

void
setmaximizedvert(Client *c, uint8_t state)
{
    SETFLAG(c->ewmhflags, WStateFlagMaximizedVert, !!state);
}

void
setmaximizedhorz(Client *c, uint8_t state)
{
    SETFLAG(c->ewmhflags, WStateFlagMaximizedHorz, !!state);
}

void
setshaded(Client *c, uint8_t state)
{
    SETFLAG(c->ewmhflags, WStateFlagShaded, !!state);
}

void
setmodal(Client *c, uint8_t state)
{
    SETFLAG(c->ewmhflags, WStateFlagModal, !!state);
}

void
setoverrideredirect(Client *c, uint8_t state)
{
    SETFLAG(c->flags, ClientFlagOverrideRedirect, !!state);
}

void
setsticky(Client *c, u8 state)
{
    SETFLAG(c->ewmhflags, WStateFlagSticky, !!state);
}

void
seturgent(Client *c, uint8_t state) 
{
    XCBCookie wmhcookie = XCBGetWMHintsCookie(_wm.dpy, c->win);
    XCBWMHints *wmh = XCBGetWMHintsReply(_wm.dpy, wmhcookie);
    SETFLAG(c->ewmhflags, WStateFlagDemandAttention, !!state);
    if(state)
    {   /* set window border */   
    }
    else
    {   /* set window border */   
    }

    if(wmh)
    {
        wmh->flags = state ? (wmh->flags | XCB_WM_HINT_URGENCY) : (wmh->flags & ~XCB_WM_HINT_URGENCY);
        XCBSetWMHintsCookie(_wm.dpy, c->win, wmh);
        free(wmh);
    }
    /* drawbar */
}

void
showhide(Client *c)
{
    const Monitor *m = c->desktop->mon;
    i16 x;
    if(ISVISIBLE(c))
    {   x = c->x;
    }
    else
    {   x = -c->w - m->mx;
    }
    XCBMoveResizeWindow(_wm.dpy, c->win, x, c->y, c->w, c->h);
}

Client *
startclient(Desktop *desk)
{
    return desk->clients;
}

Client *
startfocus(Desktop *desk)
{
    return desk->focus;
}

Client *
startstack(Desktop *desk)
{
    return desk->stack;
}

Client *
startrstack(Desktop *desk)
{
    return desk->rstack;
}

void
unfocus(Client *c, uint8_t setfocus)
{
    grabbuttons(c, 0);
    XCBSetWindowBorder(_wm.dpy, c->win, c->bcol);
    setclientnetstate(c, netatom[NetWMStateFocused], 0);
    if(setfocus)
    {   
        XCBSetInputFocus(_wm.dpy, _wm.root, XCB_INPUT_FOCUS_POINTER_ROOT, XCB_CURRENT_TIME);
        XCBDeleteProperty(_wm.dpy, _wm.root, netatom[NetActiveWindow]);
    }
    SETFLAG(c->ewmhflags, WStateFlagFocused, 0);
}

void
updateicon(Client *c, XCBWindowProperty *iconprop)
{
    free(c->icon);
    c->icon = geticonprop(iconprop);
}

void
updateborder(Client *c)
{
    updateborderwidth(c);
    updatebordercol(c);
}

void
updatebordercol(Client *c)
{
    XCBSetWindowBorder(_wm.dpy, c->win, c->bcol);
}

void
updateborderwidth(Client *c)
{
    XCBSetWindowBorderWidth(_wm.dpy, c->win, c->bw);
}

void
unmanage(Client *c, uint8_t destroyed)
{
    const XCBWindow win = c->win;

    /* prevent dangling pointer here woops */
    if(!destroyed)
    {   
        /* TODO causes alot of errors for some reason even if its not "destroyed" */
    }
    delclienthash(c);
    detachcompletely(c);
    /* Destroy colormap */
    updatecolormap(c, 0);
    updateclientlist(win, ClientListRemove);
    cleanupclient(c);
    Debug("Unmanaged: [%u]", win);
}

void 
unmaximize(Client *c)
{
    unmaximizevert(c);
    unmaximizehorz(c);
    Debug("Umaximized: [%u]", c->win);
}

void 
unmaximizehorz(Client *c)
{
    i32 x = c->oldx;
    i32 y = c->y;
    i32 w = c->oldw;
    i32 h = c->h;

    if(DOCKEDHORZ(c))
    {   
        /* if never maximized */
        if(WASDOCKEDHORZ(c))
        {   resize(c, x / 2, y, w / 2, h, 0);
        }
        else
        {   resize(c, x, y, w, h, 0);
        }
    }
    else
    {   Debug("Client already unmaxed horz: [%u]", c->win);
    }
}

void 
unmaximizevert(Client *c)
{
    i32 x = c->x;
    i32 y = c->oldy;
    i32 w = c->w;
    i32 h = c->oldh;
    if(DOCKEDVERT(c))
    {
        if(WASDOCKEDVERT(c))
        {   resize(c, x, y / 2, w, h / 2, 0);
        }
        else
        {   resize(c, x, y, w, h, 0);
        }
    }
    else
    {   Debug("Client already unmaxed vert: [%u]", c->win);
    }
}

void
updateclass(Client *c, XCBWMClass *_class)
{
    const u32 MAX_LEN = 1024;
    if(_class)
    {
        if(_class->class_name)
        {
            const u32 CLASS_NAME_LEN = strnlen(_class->class_name, MAX_LEN) + 1;
            const size_t CLASS_NAME_SIZE = sizeof(char) * CLASS_NAME_LEN;
            char *clsname = malloc(CLASS_NAME_SIZE);
            if(clsname)
            {    
                memcpy(clsname, _class->class_name, CLASS_NAME_SIZE - sizeof(char));
                clsname[CLASS_NAME_LEN - 1] = '\0';
                free(c->classname);
                c->classname = clsname;
            }
        }
        if(_class->instance_name)
        {   
            const u32 INSTANCE_NAME_LEN = strnlen(_class->instance_name, MAX_LEN) + 1;
            const size_t INSTANCE_NAME_SIZE = sizeof(char) * INSTANCE_NAME_LEN;
            char *iname = malloc(INSTANCE_NAME_SIZE);
            if(iname)
            {
                memcpy(iname, _class->instance_name, INSTANCE_NAME_SIZE - sizeof(char));
                iname[INSTANCE_NAME_LEN - 1] = '\0';
                free(c->instancename);
                c->instancename = iname;
            }
        }
    }
}

void
updatecolormap(Client *c, XCBColormap colormap)
{
    if(c->colormap)
    {   XCBUninstallColormap(_wm.dpy, c->colormap);
    }
    if(colormap)
    {   XCBInstallColormap(_wm.dpy, colormap);
    }
    c->colormap = colormap;
}

static void 
__update_motif_decor(Client *c, uint32_t hints)
{
    /* bit definitions for MwmHints.decorations */
    const u32 DECOR_ALL = 1 << 0;
    const u32 DECOR_BORDER = 1 << 1;
    const u32 DECOR_RESIZEH = 1 << 2;
    const u32 DECOR_TITLE = 1 << 3;
    const u32 DECOR_MENU = 1 << 4;
    const u32 DECOR_MINIMIZE = 1 << 5;
    const u32 DECOR_MAXIMIZE = 1 << 6;
    if(hints & DECOR_ALL)
    {   hints |= (uint32_t)~0;
    }

    if(hints & DECOR_BORDER)
    {   
        setborderwidth(c, c->oldbw);
        updateborderwidth(c);
    }
    else
    {   
        setdisableborder(c, 0);
        setborderwidth(c, 0);
        updateborderwidth(c);
        setdisableborder(c, 1);
    }
    if(hints & DECOR_RESIZEH)
    {   
        /* NOP */
        ASSUME(0);
    }
    if(hints & DECOR_TITLE)
    {   setshowdecor(c, 1);
    }
    else
    {   setshowdecor(c, 0);
    }
    if(hints & DECOR_MENU)
    {  
        /* NOP */
        ASSUME(0);
    }
    if(hints & DECOR_MINIMIZE || hints & DECOR_MAXIMIZE)
    {   
        /* NOP */
        ASSUME(0);
    }
}

static void __update_motif_func(Client *c, int32_t hints)
{
    /* bit definitions for MwmHints.functions */
    const u32 FUNCS_ALL = 1 << 0;
    const u32 FUNCS_RESIZE = 1 << 1;
    const u32 FUNCS_MOVE = 1 << 2;
    const u32 FUNCS_MINIMIZE = 1 << 3;
    const u32 FUNCS_MAXIMIZE = 1 << 4;
    const u32 FUNCS_CLOSE = 1 << 5;

    if(hints & FUNCS_ALL)
    {   hints |= (int32_t)~0;
    }

    if(hints & FUNCS_RESIZE)
    {   /* NOP */
    }
    else
    {   /* IDK set fixed or something */       
    }
    if(hints & FUNCS_MOVE)
    {   /* IGNORE */   
    }
    if(hints & FUNCS_MINIMIZE)
    {   /* IGNORE */
    }
    if(hints & FUNCS_MAXIMIZE)
    {   /* IGNORE */
    }
    if(hints & FUNCS_CLOSE)
    {   /* IGNORE */
    }
}

static void __update_motif_input(Client *c, int32_t hints)
{
    /* values for MwmHints.input_mode */
    enum ___input 
    {
        INPUT_MODELESS = 0,
        INPUT_PRIMARY_MODAL = 1,
        INPUT_SYSTEM_MODAL = 2,
        INPUT_FULL_MODAL = 3,
    };

    switch(hints)
    {   
        case INPUT_PRIMARY_MODAL:
        case INPUT_SYSTEM_MODAL:
        case INPUT_FULL_MODAL:
            /* FALLTHROUGH */
            /* TODO: Add a hash to client "class" name attribute and just make it so 1 primary window is allowed 
             * AKA just HASH the primary class name to be urgent/active window 
             */
            seturgent(c, 1);
            break;

        case INPUT_MODELESS:
            /* FALLTHROUGH */
        default: 
            break;
    }
}

static void __update_motif_status(Client *c, int32_t hints)
{
    /* bit definitions for MwmHints.status */
    const u32 STATUS_TEAROFF_WIDOW = 1 << 0;
    if(hints & STATUS_TEAROFF_WIDOW)
    {   setmodal(c, 1);
    }
}


void
updatemotifhints(Client *c, XCBWindowProperty *motifprop)
{
    /* bit definitions for MwmHints.flags */
    const u32 HINTS_FUNCTIONS = 1 << 0;
    const u32 HINTS_DECORATION = 1 << 1;
    const u32 HINTS_INPUT_MODE = 1 << 2;
    const u32 HINTS_STATUS = 1 << 3;

    if(motifprop)
    {
        MotifWmHints *hints = XCBGetPropertyValue(motifprop);
        uint32_t len = 0;
        XCBGetPropertyValueLength(motifprop, sizeof(MotifWmHints), &len);
        if(hints && len == 1)
        {   
            if(hints->flags & HINTS_DECORATION)
            {   __update_motif_decor(c, hints->decorations);
            }
            if(hints->flags & HINTS_FUNCTIONS)
            {   __update_motif_func(c, hints->functions);
            }
            if(hints->flags & HINTS_INPUT_MODE)
            {   __update_motif_input(c, hints->input_mode);
            }
            if(hints->flags & HINTS_STATUS)
            {   __update_motif_status(c, hints->status);
            }
        }
    }
}

void
updatesizehints(Client *c, XCBSizeHints *size)
{
    if(!size)
    {   return;
    }
    i32 basew = c->basew;
    i32 baseh = c->baseh;
    i32 minw = c->minw;
    i32 minh = c->minh;
    i32 maxw = c->maxw;
    i32 maxh = c->maxh;
    i32 incw = c->incw;
    i32 inch = c->inch;       
    float mina = c->mina + 0.0f;   /* make sure sign is positive */
    float maxa = c->maxa + 0.0f;   /* make sure sign is positive */
    i32 gravity = c->gravity;

    if(size->flags & XCB_SIZE_HINT_P_MIN_SIZE)
    {
        minw = size->min_width;
        minh = size->min_height;
    }
    else if(size->flags & XCB_SIZE_HINT_P_BASE_SIZE)
    {   
        minw = size->base_width;
        minh = size->base_height;
    }

    if(size->flags & XCB_SIZE_HINT_P_BASE_SIZE)
    {
        basew = size->base_width;
        baseh = size->base_height;
    }
    else if(size->flags & XCB_SIZE_HINT_P_MIN_SIZE)
    {   
        basew = size->min_width;
        baseh = size->min_height;
    }

    if(size->flags & XCB_SIZE_HINT_P_RESIZE_INC)
    {
        incw = size->width_inc;
        inch = size->height_inc;
    }
    if(size->flags & XCB_SIZE_HINT_P_MAX_SIZE)
    {
        maxw = size->max_width;
        maxh = size->max_height;
    }

    if(size->flags & XCB_SIZE_HINT_P_ASPECT)
    {
        mina = (float)size->min_aspect_den / (size->min_aspect_num + !size->min_aspect_den);
        maxa = (float)size->max_aspect_num / (size->max_aspect_den + !size->max_aspect_num);
        mina = fabsf(mina);
        maxa = fabsf(maxa);
    }

    if(size->flags & XCB_SIZE_HINT_P_WIN_GRAVITY)
    {   gravity = size->win_gravity;
    }
    /* clamp */
    minw = MIN(minw, UINT16_MAX);
    minh = MIN(minh, UINT16_MAX);
    maxw = MIN(maxw, UINT16_MAX);
    maxh = MIN(maxh, UINT16_MAX);
    basew = MIN(basew, UINT16_MAX);
    baseh = MIN(baseh, UINT16_MAX);
    (void)mina;
    (void)maxa;
    inch = MIN(inch, UINT16_MAX);
    incw = MIN(incw, UINT16_MAX);
    gravity = MIN(gravity, XCBStaticGravity);

    /* cleanse impossible sizes */
    minw = MAX(minw, 0);
    minh = MAX(minh, 0);
    maxw = MAX(maxw, 0);
    maxh = MAX(maxh, 0);
    basew = MAX(basew, 0);
    baseh = MAX(baseh, 0);
    mina = MAX(mina, 0);
    maxa = MAX(maxa, 0);
    inch = MAX(inch, 0);
    incw = MAX(incw, 0);
    gravity = MAX(gravity, 0);

    c->minw = minw;
    c->minh = minh;
    c->maxw = maxw;
    c->maxh = maxh;
    c->basew = basew;
    c->baseh = baseh;
    c->mina = mina;
    c->maxa = maxa;
    c->inch = inch;
    c->incw = incw;
    c->gravity = gravity;

    i32 x = c->x;
    i32 y = c->y;
    i32 w = c->w;
    i32 h = c->h;
    u8 geom = 0;
    /* check for resize flags */
    if(size->flags & XCB_SIZE_HINT_P_SIZE)
    {
        if(w != c->w || h != c->h)
        {
            w = size->width;
            h = size->height;
            geom = 1;
        }
    }

    if(size->flags & XCB_SIZE_HINT_P_POSITION)
    {
        if(size->x || size->y)
        {
            x = size->x;
            y = size->y;
            geom = 1;
        }
    }

    if(size->flags & XCB_SIZE_HINT_US_SIZE)
    {
        w = size->width;
        h = size->height;
        geom = 1;
    }

    if(size->flags & XCB_SIZE_HINT_US_POSITION)
    {
        x = size->x;
        y = size->y;
        geom = 1;
    }

    if(geom)
    {   resize(c, x, y, w, h, 1);
    }
}

void
updatetitle(Client *c, char *netwmname, char *wmname)
{
    if(c->wmname != wmname)
    {   free(c->wmname);
        c->wmname = NULL;
    }
    if(c->netwmname != netwmname)
    {   free(c->netwmname);
        c->netwmname = NULL;
    }
    c->wmname = wmname;
    c->netwmname = netwmname;
}

void
updatewindowprotocol(Client *c, XCBWMProtocols *protocols)
{
    if(protocols && protocols->atoms_len)
    {
        uint32_t i;
        XCBAtom atom;
        for(i = 0; i < protocols->atoms_len; ++i)
        {
            atom = protocols->atoms[i];
            if(atom == wmatom[WMTakeFocus])
            {   setwmtakefocus(c, 1);
            }
            else if(atom == wmatom[WMDeleteWindow])
            {   setwmdeletewindow(c, 1);
            }
            else if(atom == wmatom[WMSaveYourself])
            {   setwmsaveyourself(c, 1);
            }
        }
    }
}

void
updatewindowstate(Client *c, XCBAtom state, uint8_t add_remove_toggle)
{
    if(!state)
    {   return;
    }
    const u8 toggle = add_remove_toggle == 2;
    /* This is similiar to those Windows 10 dialog boxes that play the err sound and cant click anything else */
    if (state == netatom[NetWMStateModal])
    {
        if(toggle)
        {   
            setmodal(c, !ISMODAL(c));
            setwtypedialog(c, !ISDIALOG(c));
        }
        else
        {
            setmodal(c, add_remove_toggle);
            setwtypedialog(c, add_remove_toggle);
        }
    }                                                           /* This is just syntax sugar, really its just a alias to NetWMStateAbove */
    else if (state == netatom[NetWMStateAbove] || state == netatom[NetWMStateAlwaysOnTop])
    {
        if(toggle)
        {   setalwaysontop(c, !ISALWAYSONTOP(c));
        }
        else
        {   setalwaysontop(c, add_remove_toggle);
        }
    }
    else if (state == netatom[NetWMStateDemandAttention])
    {
        if(toggle)
        {   seturgent(c, !ISURGENT(c));
        }
        else
        {   seturgent(c, add_remove_toggle);
        }
    }
    else if (state == netatom[NetWMStateFullscreen])
    {
        if(toggle)
        {   setfullscreen(c, !ISFULLSCREEN(c));
        }
        else
        {   setfullscreen(c, add_remove_toggle);
        }
    }
    else if (state == netatom[NetWMStateMaximizedHorz])
    {
        if(toggle)
        {   
            if(DOCKEDHORZ(c))
            {   
                unmaximizehorz(c);
                setmaximizedhorz(c, 0);
            }
            else
            {   
                maximizehorz(c);
                setmaximizedhorz(c, 1);
            }
        }
        else
        {   
            if(add_remove_toggle)
            {   maximizehorz(c);
            }
            else
            {   unmaximizehorz(c);
            }
            setmaximizedhorz(c, add_remove_toggle);
        }
    }
    else if (state == netatom[NetWMStateMaximizedVert])
    {
        if(toggle)
        {
            if(DOCKEDVERT(c))
            {   
                unmaximizevert(c);
                setmaximizedvert(c, 0);
            }
            else
            {   
                maximizevert(c);
                setmaximizedvert(c, 1);
            }
        }
        else
        {
            if(add_remove_toggle)
            {   maximizevert(c);
            }
            else
            {   unmaximizevert(c);
            }
            setmaximizedvert(c, add_remove_toggle);
        }
    }
    else if (state == netatom[NetWMStateSticky])
    {
        if(toggle)
        {   setsticky(c, !ISSTICKY(c));
        }
        else
        {   setsticky(c, add_remove_toggle);
        }
    }
    else if (state == netatom[NetWMStateBelow])
    {   
        /* this is a wierd state to even configure so idk */
        if(toggle)
        {       setalwaysonbottom(c, !ISALWAYSONBOTTOM(c));
        }
        else
        {   setalwaysonbottom(c, add_remove_toggle);
        }
    }
    else if (state == netatom[NetWMStateSkipTaskbar])
    {   
        if(toggle)
        {   setskiptaskbar(c, !SKIPTASKBAR(c));
        }
        else
        {   setskiptaskbar(c, add_remove_toggle);
        }
    }
    else if (state == netatom[NetWMStateSkipPager])
    {
        if(toggle)
        {   setskippager(c, !SKIPPAGER(c));
        }
        else
        {   setskippager(c, add_remove_toggle);
        }
    }
    else if (state == netatom[NetWMStateHidden])
    {   /* window manager sets this, ignore */
    }
    else if (state == netatom[NetWMStateFocused])
    {   /* window manager sets this, ignore */
    }
    else if (state == netatom[NetWMStateShaded])
    {
        if(toggle)
        {   setshaded(c, !ISSHADED(c));
        }
        else
        {   setshaded(c, add_remove_toggle);
        }
    }
    else
    {   char *name = GetAtomNameQuick(_wm.dpy, state);
        if(name)
        {   Debug("Atom type: %s", name);
        }
        else
        {   Debug0("Could not find type.");
        }
        free(name);
    }
}

void
updatewindowstates(Client *c, XCBAtom states[], uint32_t atomslength)
{
    if(!states)
    {   return;
    }

    /* bullshit client is trying to mess with us */
    u16 MAX_LIMIT = 1000;
    atomslength = MIN(atomslength, MAX_LIMIT);


    u32 i;
    for(i = 0; i < atomslength; ++i)
    {
        /* Even though the wm-spec says that we should remove things that arent in the list 
         * The client will ussually tell us in clientmessage if its important. 
         * It also says however that if its in the list assume its a prop so...
         */
        updatewindowstate(c, states[i], 1);
    }
}

void
updatewindowtype(Client *c, XCBAtom wtype, uint8_t add_remove_toggle)
{
    if(!wtype)
    {   return;
    }

    const u8 toggle = add_remove_toggle == 2;

    if (wtype == netatom[NetWMWindowTypeDesktop])
    {
        if(toggle)
        {   setwtypedesktop(c, !ISDESKTOP(c));
        }
        else
        {   setwtypedesktop(c, add_remove_toggle);
        }
        /* TODO */
    }
    else if (wtype == netatom[NetWMWindowTypeDock])
    {
        if(toggle)
        {   setwtypedock(c, !ISDOCK(c));
        }
        else
        {   setwtypedock(c, add_remove_toggle);
        }
    }
    else if (wtype == netatom[NetWMWindowTypeToolbar])
    {   
        if(toggle)
        {   setwtypetoolbar(c, !ISTOOLBAR(c));
        }
        else
        {   setwtypetoolbar(c, add_remove_toggle);
        }
    }
    else if (wtype == netatom[NetWMWindowTypeMenu])
    {
        if(toggle)
        {   setwtypemenu(c, !ISMENU(c));
        }
        else
        {   setwtypemenu(c, add_remove_toggle);
        }
    }
    else if (wtype == netatom[NetWMWindowTypeUtility])
    {
        if(toggle)
        {   setwtypeutility(c, !ISUTILITY(c));
        }
        else
        {   setwtypeutility(c, add_remove_toggle);
        }
    }
    else if (wtype == netatom[NetWMWindowTypeSplash])
    {
        if(toggle)
        {   setwtypesplash(c, !ISSPLASH(c));
        }
        else
        {   setwtypesplash(c, add_remove_toggle);
        }
    }
    else if (wtype == netatom[NetWMWindowTypeDialog])
    {   
        if(toggle)
        {   setwtypedialog(c, !ISDIALOG(c));
        }
        else
        {   setwtypedialog(c, add_remove_toggle);
        }
    }
    else if (wtype == netatom[NetWMWindowTypeDropdownMenu])
    {   
        if(toggle)
        {   setwtypedropdownmenu(c, !ISDROPDOWNMENU(c));
        }
        else
        {   setwtypedropdownmenu(c, add_remove_toggle);
        }
    }
    else if (wtype == netatom[NetWMWindowTypePopupMenu])
    {
        if(toggle)
        {   setwtypepopupmenu(c, !ISPOPUPMENU(c));
        }
        else
        {   setwtypepopupmenu(c, add_remove_toggle);
        }
    }
    else if (wtype == netatom[NetWMWindowTypeTooltip])
    {
        if(toggle)
        {   setwtypetooltip(c, !ISTOOLTIP(c));
        }
        else
        {   setwtypetooltip(c, add_remove_toggle);
        }
    }
    else if (wtype == netatom[NetWMWindowTypeNotification])
    { 
        if(toggle)
        {   setwtypenotification(c, !ISNOTIFICATION(c));
        }
        else
        {   setwtypenotification(c, add_remove_toggle);
        }
    }
    else if (wtype == netatom[NetWMWindowTypeCombo])
    {
        if(toggle)
        {   setwtypecombo(c, !ISCOMBO(c));
        }
        else
        {   setwtypecombo(c, add_remove_toggle);
        }
    }
    else if (wtype == netatom[NetWMWindowTypeDnd])
    {
        if(toggle)
        {   setwtypednd(c, !ISDND(c));
        }
        else
        {   setwtypednd(c, add_remove_toggle);
        }
    }
    else if (wtype == netatom[NetWMWindowTypeNormal])
    {
        if(toggle)
        {   setwtypenormal(c, !ISNORMAL(c));
        }
        else
        {   setwtypenormal(c, add_remove_toggle);   
        }
    }
    else
    {   
        char *name = GetAtomNameQuick(_wm.dpy, wtype);
        if(name)
        {   Debug("Atom type: %s", name);
        }
        else
        {   Debug0("Could not find type.");
        }
        free(name);
    }
}

void
updatewindowtypes(Client *c, XCBAtom wtypes[], uint32_t atomslength)
{
    if(!wtypes)
    {   return;
    }
    /* bullshit client is trying to mess with us */
    u8 MAX_LIMIT = 255;
    atomslength = MIN(atomslength, MAX_LIMIT);

    i32 i;
    for(i = 0; i < atomslength; ++i)
    {   
        /* wm-spec says that we should assume anythings in the list are props so we just pass into "add" */
        updatewindowtype(c, wtypes[i], 1);
    }
}

void
updatewmhints(Client *c, XCBWMHints *wmh)
{
    if(wmh)
    {
        if(c == c->desktop->sel && wmh->flags & XCB_WM_HINT_URGENCY)
        {
            wmh->flags &= ~XCB_WM_HINT_URGENCY;
            XCBSetWMHintsCookie(_wm.dpy, c->win, wmh);
            /* dont put seturgent() here cause that would just undo what we did and be recursive */
        }
        else
        {   
            /* dont put seturgent() here cause that would just undo what we did and be recursive */
            SETFLAG(c->ewmhflags, WStateFlagDemandAttention, !!(wmh->flags & XCB_WM_HINT_URGENCY));
        }
        if(wmh->flags & XCB_WM_HINT_INPUT)
        {   setwtypeneverfocus(c, !wmh->input);
        }
        else
        {   setwtypeneverfocus(c, 0);
        }
        if(wmh->flags & XCB_WM_HINT_STATE)
        {
            switch(wmh->initial_state)
            {   
                case XCB_WINDOW_ICONIC_STATE:
                    if(!ISMAPICONIC(c))
                    {   setwtypemapiconic(c, 1);
                    }
                    break;
                case XCB_WINDOW_WITHDRAWN_STATE:
                    Debug0("Window is 'withdrawn' this is not handled by the window manager.");
                    break;
                case XCB_WINDOW_NORMAL_STATE:
                    if(!ISMAPNORMAL(c))
                    {   setwtypemapnormal(c, 1);
                    }
                    break;
                default:
                    break;
            }
        }
        if(wmh->flags & XCB_WM_HINT_ICON_PIXMAP)
        {   /* update icon or something */
        }
        if(wmh->flags & XCB_WM_HINT_ICON_MASK)
        {   /* use flagged bits to asign icon shape */
        }
    }
}

Client *
wintoclient(XCBWindow win)
{
    Client *c = getclienthash(win);

    if(c)
    {   return c;
    }

    /* malloc can fail some times so linear prob for client (unlikely), or it just doesnt exist. */
    Monitor *m;
    Desktop *desk;
    for(m = _wm.mons; m; m = nextmonitor(m))
    {
        for(desk = m->desktops; desk; desk = nextdesktop(desk))
        {
            for(c = startclient(desk); c; c = nextclient(c))
            {
                if(c->win == win)
                {   
                    Debug0("Found non hashed client.");
                    Debug0("Hashing...");
                    /* try and re-add it to the hasmap */
                    addclienthash(c);
                    return c;
                }
            }
        }
    }
    return NULL;
}
