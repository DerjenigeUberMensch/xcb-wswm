#ifndef EVENTS_H_
#define EVENTS_H_

#include "XCB-TRL/xcb_trl.h"
#include "util.h"

/* Handles Key Press Events (Self explanitory) */
void NonNull keypress(XCBGenericEvent *event);
/* Handles Key Release Events (Self explanitory) */
void NonNull keyrelease(XCBGenericEvent *event);
/* Handles Button Release Events (Self explanitory) */
void NonNull buttonpress(XCBGenericEvent *event);
/* Handles Button Release Events (Self explanitory) */
void NonNull buttonrelease(XCBGenericEvent *event);
/* Handles Motion Events (AKA mouse movement.) */
void NonNull motionnotify(XCBGenericEvent *event);
/* Handles Enter Events (AKA "entering" a client/mouse going inside/ontop a window) */
void NonNull enternotify(XCBGenericEvent *event);
/* Handles Leave Events (AKA "leaving" a client/mouse going outside of a window) */
void NonNull leavenotify(XCBGenericEvent *event);
/* Handles Focus in Events (AKA when a client gets focused) */
void NonNull focusin(XCBGenericEvent *event);
/* Handles Focus out Events (AKA when a client is unfocused) */
void NonNull focusout(XCBGenericEvent *event);
/* Handles Keymap events (AKA when a client requests for a certain key "combination" (map) for them)*/
void NonNull keymapnotify(XCBGenericEvent *event);
/* Handles Expose events (Not sure.) */
void NonNull expose(XCBGenericEvent *event);
/* Handles Graphics Expose Events (Not sure.) */
void NonNull graphicsexpose(XCBGenericEvent *event);
/* Handles No Expose Events (Not sure.) */
void NonNull noexpose(XCBGenericEvent *event);
/* Handles Circulate Events
 * (This is for the most part just more explicit configure request, With Sibling/Stackmode)
 */
void NonNull circulaterequest(XCBGenericEvent *event);
/* Handles Configure Events. 
 * These are events that pertain to the change of a windows geometry, or stackmode.
 */
void NonNull configurerequest(XCBGenericEvent *event);
/* Handles Map Events.
 * This event requests to map a window, this request Can be ignored, 
 * Though you wouldnt render anything if you did.
 */
void NonNull maprequest(XCBGenericEvent *event);
/* This supposedly is used for clients requesting geometry changes of windows from configurerequest();
 * Though I am not sure what this does exactly/
 */
void NonNull resizerequest(XCBGenericEvent *event);
/* Handles Circulate Notify Events.
 * Event reports successfull circulate requests done for a window.
 * (And what they changed/did.)
 */
void NonNull circulatenotify(XCBGenericEvent *event);
/* Handles Configure Notify Events.
 * Event reports successfull configure requests done for a window.
 * (And what they changed/did.)
 */
void NonNull configurenotify(XCBGenericEvent *event);
/* Handles Window Create Events.
 * Event reports any client requesting for the creation of a new window.
 * Do note that it only reports creation of a Window, it doesnt report Window mapping.
 */
void NonNull createnotify(XCBGenericEvent *event);
/* Handles Window Destroy Events.
 * Event reports any destroyed windows.
 */
void NonNull destroynotify(XCBGenericEvent *event);
/*
 */
void NonNull gravitynotify(XCBGenericEvent *event);
/*
 */
void NonNull mapnotify(XCBGenericEvent *event);
/*
 */
void NonNull mappingnotify(XCBGenericEvent *event);
/*
 */
void NonNull unmapnotify(XCBGenericEvent *event);
/*
 */
void NonNull visibilitynotify(XCBGenericEvent *event);
/*
 */
void NonNull reparentnotify(XCBGenericEvent *event);
/*
 */
void NonNull colormapnotify(XCBGenericEvent *event);
/* Handles Window Message Events.
 * These events inform other windows of stuff they want to change to the Window manager. (For your job anyway.)
 * This is a very broad event and is ussualy interpreted by what atom was sent with.
 */
void NonNull clientmessage(XCBGenericEvent *event);
/* Handles Property Notify Events.
 * These events inform windows of changes the XServer made to properties of other windows.
 * This is a very broad event and is ussually interpreted by what atom was sent with.
 */
void NonNull propertynotify(XCBGenericEvent *event);
/* Handles Selection Clear Events 
 * Im not sure what this event does.
 */
void NonNull selectionclear(XCBGenericEvent *event);
/* Handles Selection Clear Events 
 * Im not sure what this event does.
 */
void NonNull selectionnotify(XCBGenericEvent *event);
/* Handles Selection Clear Events 
 * Im not sure what this event does.
 */
void NonNull selectionrequest(XCBGenericEvent *event);
/* This event shouldnt be possible, but is here as some "events" sent by clients may be broken/
 * AKA this is just a placeholder. (It does nothing.) 
 */
void NonNull genericevent(XCBGenericEvent *event);

void NonNull errorhandler(XCBGenericEvent *error);

#endif
