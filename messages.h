#ifndef _MESSAGES_H_
#define _MESSAGES_H_


/***************************************************************
*                           UWM_DRAW_SPRITEFRAME_MSG
* Inputs:
*       WPARAM: ignored, 0
*       LPARAM: the frame number, 0-256
* Result: LRESULT
*	Logically void, 0, always
* Effect:
*	Causes the view to draw the corresponding frame of the sprite
***************************************************************/
#define UWM_DRAW_SPRITEFRAME_MSG _T("UWM_DRAW_SPRITEFRAME-{3502F967-4C83-4107-A4A8-E0667EDB140E}")

/***************************************************************
*                           UWM_DRAW_NEXTSPRITEFRAME_MSG
* Inputs:
*       WPARAM: ignored, 0
*       LPARAM: the frame number, 0-256
* Result: LRESULT
*	Logically void, 0, always
* Effect:
*	Causes the view to draw the corresponding frame of the sprite
***************************************************************/
#define UWM_DRAW_NEXTSPRITEFRAME_MSG _T("UWM_DRAW_NEXTSPRITEFRAME-{3502F967-4C83-4107-A4A8-E0667EDB140E}")


#define DECLARE_USER_MESSAGE(name) \
     static const UINT name = ::RegisterWindowMessage(name##_MSG)



#define UWM_DRAW_NEXTSPRITEFRAME (WM_APP + 1)
//DECLARE_USER_MESSAGE( UWM_DRAW_NEXTSPRITEFRAME );



#endif