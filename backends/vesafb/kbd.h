#ifndef _KBD_H
#define _KBD_H
/*
 * Copyright (c) 1999, 2000, 2001, 2002, 2003 Greg Haerr <greg@censoft.com>
 * Portions Copyright (c) 2002 by Koninklijke Philips Electronics N.V.
 *
 * Exported Microwindows engine typedefs and defines
 */
/**
 * modified by Daniel Leo <grayrover@gmail.com>
 *
 */

/* Keyboard state modifiers*/
typedef unsigned int	KEYMOD;
typedef unsigned short	KEY;
typedef unsigned short	SCANCODE;

typedef struct _kbddevice {
	int  (*Open)(void);
	void (*Close)(void);
	void (*GetModifierInfo)(KEYMOD *modifiers, KEYMOD *curmodifiers);
	int  (*Read)(KEY *buf,KEYMOD *modifiers,SCANCODE *scancode);
	int  (*Poll)(void);		/* not required if have select()*/
} KBDDEVICE;

/* Keyboard values*/


#define _KEY_UNKNOWN		0
/* Following special control keysyms are mapped to ASCII*/
#define _KEY_BACKSPACE		8
#define _KEY_TAB		9
#define _KEY_ENTER		13
#define _KEY_ESCAPE		27
/* Keysyms from 32-126 are mapped to ASCII*/

#define _KEY_NONASCII_MASK	0xFF00
/* Following keysyms are mapped to private use portion of Unicode-16*/
/* arrows + home/end pad*/
#define _KEY_FIRST		0xF800
#define _KEY_LEFT		0xF800
#define _KEY_RIGHT		0xF801
#define _KEY_UP	        	0xF802
#define _KEY_DOWN		0xF803
#define _KEY_INSERT		0xF804
#define _KEY_DELETE		0xF805
#define _KEY_HOME		0xF806
#define _KEY_END		0xF807
#define _KEY_PAGEUP		0xF808
#define _KEY_PAGEDOWN		0xF809

/* Numeric keypad*/
#define _KEY_KP0		0xF80A
#define _KEY_KP1		0xF80B
#define _KEY_KP2		0xF80C
#define _KEY_KP3		0xF80D
#define _KEY_KP4		0xF80E
#define _KEY_KP5		0xF80F
#define _KEY_KP6		0xF810
#define _KEY_KP7		0xF811
#define _KEY_KP8		0xF812
#define _KEY_KP9		0xF813
#define _KEY_KP_PERIOD		0xF814
#define _KEY_KP_DIVIDE		0xF815
#define _KEY_KP_MULTIPLY	0xF816
#define _KEY_KP_MINUS		0xF817
#define _KEY_KP_PLUS		0xF818
#define _KEY_KP_ENTER		0xF819
#define _KEY_KP_EQUALS		0xF81A

/* Function keys */
#define _KEY_F1		0xF81B
#define _KEY_F2		0xF81C
#define _KEY_F3		0xF81D
#define _KEY_F4		0xF81E
#define _KEY_F5		0xF81F
#define _KEY_F6		0xF820
#define _KEY_F7		0xF821
#define _KEY_F8		0xF822
#define _KEY_F9		0xF823
#define _KEY_F10		0xF824
#define _KEY_F11		0xF825
#define _KEY_F12		0xF827

/* Key state modifier keys*/
#define _KEY_NUMLOCK		0xF828
#define _KEY_CAPSLOCK		0xF829
#define _KEY_SCROLLOCK		0xF82A
#define _KEY_LSHIFT		0xF82B
#define _KEY_RSHIFT		0xF82C
#define _KEY_LCTRL		0xF82D
#define _KEY_RCTRL		0xF82E
#define _KEY_LALT		0xF82F
#define _KEY_RALT		0xF830
#define _KEY_LMETA		0xF831
#define _KEY_RMETA		0xF832
#define _KEY_ALTGR		0xF833

/* Misc function keys*/
#define _KEY_PRINT		0xF834
#define _KEY_SYSREQ		0xF835
#define _KEY_PAUSE		0xF836
#define _KEY_BREAK		0xF837
#define _KEY_QUIT		0xF838	/* virtual key*/
#define _KEY_MENU		0xF839	/* virtual key*/
#define _KEY_REDRAW		0xF83A	/* virtual key*/

/* Handheld function keys*/
/* #define _KEY_RECORD		0xF840 -- Replaced by HAVi code */
/* #define _KEY_PLAY		0xF841 -- Replaced by HAVi code */
#define _KEY_CONTRAST		0xF842
#define _KEY_BRIGHTNESS	0xF843
#define _KEY_SELECTUP		0xF844
#define _KEY_SELECTDOWN	0xF845
#define _KEY_ACCEPT		0xF846
#define _KEY_CANCEL		0xF847
#define _KEY_APP1		0xF848
#define _KEY_APP2		0xF849
#define _KEY_APP3              0xF84A
#define _KEY_APP4              0xF84B
#define _KEY_SUSPEND           0xF84C
#define _KEY_END_NORMAL	0xF84D	/* insert additional keys before this*/

/*
 * The following keys are useful for remote controls on consumer
 * electronics devices (e.g. TVs, videos, DVD players, cable
 * boxes, satellite boxes, digital terrestrial recievers, ...)
 *
 * The codes are taken from the HAVi specification:
 *   HAVi Level 2 User Interface version 1.1, May 15th 2001
 * They are listed in section 8.7.
 *
 * For more information see http://www.havi.org/
 */

/* _KEY code for first HAVi key */
#define _KEY_HAVI_KEY_BASE   (_KEY_END_NORMAL+1)

/* HAVi code for first HAVi key */
#define _KEY_HAVI_CODE_FIRST  403

/* HAVi code for last HAVi key */
#define _KEY_HAVI_CODE_LAST   460

/* HRcEvent.VK_... code to _KEY_... code */
#define _KEY_FROM_HAVI_CODE(h) ((h) + (_KEY_HAVI_KEY_BASE - _KEY_HAVI_CODE_FIRST))

/* _KEY_... code to HRcEvent.VK_... code */
#define _KEY_TO_HAVI_CODE(m)   ((m) - (_KEY_HAVI_KEY_BASE - _KEY_HAVI_CODE_FIRST))

/* Can an _KEY_... code be converted into a HRcEvent.VK_... code? */
#define _KEY_IS_HAVI_CODE(m)   (  (unsigned)((m) - _KEY_HAVI_KEY_BASE) \
               <= (unsigned)(_KEY_HAVI_CODE_LAST - _KEY_HAVI_CODE_FIRST) )


#define _KEY_COLORED_KEY_0         _KEY_FROM_HAVI_CODE(403)
#define _KEY_COLORED_KEY_1         _KEY_FROM_HAVI_CODE(404)
#define _KEY_COLORED_KEY_2         _KEY_FROM_HAVI_CODE(405)
#define _KEY_COLORED_KEY_3         _KEY_FROM_HAVI_CODE(406)
#define _KEY_COLORED_KEY_4         _KEY_FROM_HAVI_CODE(407)
#define _KEY_COLORED_KEY_5         _KEY_FROM_HAVI_CODE(408)
#define _KEY_POWER                 _KEY_FROM_HAVI_CODE(409)
#define _KEY_DIMMER                _KEY_FROM_HAVI_CODE(410)
#define _KEY_WINK                  _KEY_FROM_HAVI_CODE(411)
#define _KEY_REWIND                _KEY_FROM_HAVI_CODE(412)
#define _KEY_STOP                  _KEY_FROM_HAVI_CODE(413)
#define _KEY_EJECT_TOGGLE          _KEY_FROM_HAVI_CODE(414)
#define _KEY_PLAY                  _KEY_FROM_HAVI_CODE(415)
#define _KEY_RECORD                _KEY_FROM_HAVI_CODE(416)
#define _KEY_FAST_FWD              _KEY_FROM_HAVI_CODE(417)
#define _KEY_PLAY_SPEED_UP         _KEY_FROM_HAVI_CODE(418)
#define _KEY_PLAY_SPEED_DOWN       _KEY_FROM_HAVI_CODE(419)
#define _KEY_PLAY_SPEED_RESET      _KEY_FROM_HAVI_CODE(420)
#define _KEY_RECORD_SPEED_NEXT     _KEY_FROM_HAVI_CODE(421)
#define _KEY_GO_TO_START           _KEY_FROM_HAVI_CODE(422)
#define _KEY_GO_TO_END             _KEY_FROM_HAVI_CODE(423)
#define _KEY_TRACK_PREV            _KEY_FROM_HAVI_CODE(424)
#define _KEY_TRACK_NEXT            _KEY_FROM_HAVI_CODE(425)
#define _KEY_RANDOM_TOGGLE         _KEY_FROM_HAVI_CODE(426)
#define _KEY_CHANNEL_UP            _KEY_FROM_HAVI_CODE(427)
#define _KEY_CHANNEL_DOWN          _KEY_FROM_HAVI_CODE(428)
#define _KEY_STORE_FAVORITE_0      _KEY_FROM_HAVI_CODE(429)
#define _KEY_STORE_FAVORITE_1      _KEY_FROM_HAVI_CODE(430)
#define _KEY_STORE_FAVORITE_2      _KEY_FROM_HAVI_CODE(431)
#define _KEY_STORE_FAVORITE_3      _KEY_FROM_HAVI_CODE(432)
#define _KEY_RECALL_FAVORITE_0     _KEY_FROM_HAVI_CODE(433)
#define _KEY_RECALL_FAVORITE_1     _KEY_FROM_HAVI_CODE(434)
#define _KEY_RECALL_FAVORITE_2     _KEY_FROM_HAVI_CODE(435)
#define _KEY_RECALL_FAVORITE_3     _KEY_FROM_HAVI_CODE(436)
#define _KEY_CLEAR_FAVORITE_0      _KEY_FROM_HAVI_CODE(437)
#define _KEY_CLEAR_FAVORITE_1      _KEY_FROM_HAVI_CODE(438)
#define _KEY_CLEAR_FAVORITE_2      _KEY_FROM_HAVI_CODE(439)
#define _KEY_CLEAR_FAVORITE_3      _KEY_FROM_HAVI_CODE(440)
#define _KEY_SCAN_CHANNELS_TOGGLE  _KEY_FROM_HAVI_CODE(441)
#define _KEY_PINP_TOGGLE           _KEY_FROM_HAVI_CODE(442)
#define _KEY_SPLIT_SCREEN_TOGGLE   _KEY_FROM_HAVI_CODE(443)
#define _KEY_DISPLAY_SWAP          _KEY_FROM_HAVI_CODE(444)
#define _KEY_SCREEN_MODE_NEXT      _KEY_FROM_HAVI_CODE(445)
#define _KEY_VIDEO_MODE_NEXT       _KEY_FROM_HAVI_CODE(446)
#define _KEY_VOLUME_UP             _KEY_FROM_HAVI_CODE(447)
#define _KEY_VOLUME_DOWN           _KEY_FROM_HAVI_CODE(448)
#define _KEY_MUTE                  _KEY_FROM_HAVI_CODE(449)
#define _KEY_SURROUND_MODE_NEXT    _KEY_FROM_HAVI_CODE(450)
#define _KEY_BALANCE_RIGHT         _KEY_FROM_HAVI_CODE(451)
#define _KEY_BALANCE_LEFT          _KEY_FROM_HAVI_CODE(452)
#define _KEY_FADER_FRONT           _KEY_FROM_HAVI_CODE(453)
#define _KEY_FADER_REAR            _KEY_FROM_HAVI_CODE(454)
#define _KEY_BASS_BOOST_UP         _KEY_FROM_HAVI_CODE(455)
#define _KEY_BASS_BOOST_DOWN       _KEY_FROM_HAVI_CODE(456)
#define _KEY_INFO                  _KEY_FROM_HAVI_CODE(457)
#define _KEY_GUIDE                 _KEY_FROM_HAVI_CODE(458)
#define _KEY_TELETEXT              _KEY_FROM_HAVI_CODE(459)
#define _KEY_SUBTITLE              _KEY_FROM_HAVI_CODE(460)

#define _KEY_LAST                  _KEY_SUBTITLE

/* Keyboard state modifiers*/
#define _KMOD_NONE  		0x0000
#define _KMOD_LSHIFT		0x0001
#define _KMOD_RSHIFT		0x0002
#define _KMOD_LCTRL 		0x0040
#define _KMOD_RCTRL 		0x0080
#define _KMOD_LALT  		0x0100
#define _KMOD_RALT  		0x0200
#define _KMOD_LMETA 		0x0400		/* Windows key*/
#define _KMOD_RMETA 		0x0800		/* Windows key*/
#define _KMOD_NUM   		0x1000
#define _KMOD_CAPS  		0x2000
#define _KMOD_ALTGR 		0x4000
#define _KMOD_SCR		0x8000

#define _KMOD_CTRL	(_KMOD_LCTRL|_KMOD_RCTRL)
#define _KMOD_SHIFT	(_KMOD_LSHIFT|_KMOD_RSHIFT)
#define _KMOD_ALT	(_KMOD_LALT|_KMOD_RALT)
#define _KMOD_META	(_KMOD_LMETA|_KMOD_RMETA)

#define _KINFO_LED_MASK	(1 << 0)
#define _KINFO_LED_MODE_MASK	(1 << 1)

/* Keyboard info values */
#define _KINFO_LED_CAP		(1 << 0)
#define _KINFO_LED_NUM		(1 << 1)
#define _KINFO_LED_SCR		(1 << 2)

#define _KINFO_LED_MODE_ON	(1 << 3)
#define _KINFO_LED_MODE_OFF	(1 << 4)

typedef struct {
	int led;
	int led_mode;
} _KBINFO, *P_KBINFO;
#endif /* _KBD_H */
