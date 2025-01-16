/*
 * Copyright (c) 2000, 2003 Greg Haerr <greg@censoft.com>
 *
 * Microwindows /dev/tty console scancode keyboard driver for Linux
 */
/* 
 * modified by Daniel Leo <grayrover@gmail.com>
 *       
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <linux/keyboard.h>
#include <linux/kd.h>
#include <linux/vt.h>
#include "kbd.h"


#define KEYBOARD	"/dev/tty0"	/* console kbd to open*/

static int  TTY_Open(void);
static void TTY_Close(void);
static void TTY_GetModifierInfo(KEYMOD *modifiers, KEYMOD *curmodifiers);
static int  TTY_Read(KEY *kbuf, KEYMOD *modifiers, SCANCODE *scancode);

#if 0
KBDDEVICE kbddev = {
	TTY_Open,
	TTY_Close,
	TTY_GetModifierInfo,
	TTY_Read,
	NULL
};
#endif

#define RELEASED	0
#define PRESSED		1

static	int		fd;		/* file descriptor for keyboard */
static	struct termios	old;		/* original terminal modes */
static  int 		old_kbd_mode;
static unsigned char 	key_state[128];	
static KEYMOD 	key_modstate;

/* kernel unicode tables per shiftstate and scancode*/
#define NUM_VGAKEYMAPS	(1<<KG_CAPSSHIFT)	/* kernel key maps*/
static unsigned short	os_keymap[NUM_VGAKEYMAPS][NR_KEYS];


/* Pick the right scancode conversion table */

#include "keymap_standard.h"


static int	UpdateKeyState(int pressed, KEY _key);
static void	UpdateLEDState(KEYMOD modstate);
static KEY	TranslateScancode(int scancode, KEYMOD modstate);
static void	LoadKernelKeymaps(int fd);
static int	switch_vt(unsigned short which);

/*
 * Open the keyboard.
 * This is real simple, we just use a special file handle
 * that allows non-blocking I/O, and put the terminal into
 * character mode.
 */
static int
TTY_Open(void)
{
	int		i;
	int		ledstate = 0;
	char *		kbd;
	struct termios	new;

	kbd = KEYBOARD;
	fd = open(kbd, O_NONBLOCK);
	if (fd < 0)
		return -1;

	/* Save previous settings*/
	if (ioctl(fd, KDGKBMODE, &old_kbd_mode) < 0) {
		perror("KDGKMODE");
		goto err;
	}
	if (tcgetattr(fd, &old) < 0)
		goto err;

	/* Set medium-raw keyboard mode */
	new = old;
	/* ISIG and BRKINT must be set otherwise '2' is ^C (scancode 3)!!*/
	new.c_lflag &= ~(ICANON | ECHO | ISIG);
	new.c_iflag &= ~(ISTRIP | IGNCR | ICRNL | INLCR | IXOFF | IXON 
			| BRKINT);
	new.c_cc[VMIN] = 0;
	new.c_cc[VTIME] = 0;

	if (tcsetattr(fd, TCSAFLUSH, &new) < 0) {
		TTY_Close();
		return -1;
	}
	if (ioctl(fd, KDSKBMODE, K_MEDIUMRAW) < 0) {
		TTY_Close();
		return -1;
	}

	/* Load OS keymappings*/
	LoadKernelKeymaps(fd);

	/* Initialize keyboard state*/
	key_modstate = _KMOD_NONE;
	for (i=0; i<128; ++i)
		key_state[i] = RELEASED;
	
	/* preset CAPSLOCK and NUMLOCK from startup LED state*/
	if (ioctl(fd, KDGETLED, &ledstate) == 0) {
		if (ledstate & LED_CAP) {
			key_modstate |= _KMOD_CAPS;
			key_state[_KEY_CAPSLOCK] = PRESSED;
		}
		if (ledstate & LED_NUM) {
			key_modstate |= _KMOD_NUM;
			key_state[_KEY_NUMLOCK] = PRESSED;
		}
	}
	UpdateLEDState(key_modstate);

	return fd;

err:
	close(fd);
	fd = -1;
	return -1;
}

/*
 * Close the keyboard.
 * This resets the terminal modes.
 */
static void
TTY_Close(void)
{
	int	ledstate = 0x80000000L;

	if (fd >= 0) {
		/* revert LEDs to follow key modifiers*/
		if (ioctl(fd, KDSETLED, ledstate) < 0)
			perror("KDSETLED");

		/* reset terminal mode*/
		if (ioctl(fd, KDSKBMODE, old_kbd_mode) < 0)
			perror("KDSKBMODE");
		tcsetattr(fd, TCSAFLUSH, &old);

		close(fd);
	}
	fd = -1;
}

/*
 * Return the possible modifiers and current modifiers for the keyboard.
 */
static  void
TTY_GetModifierInfo(KEYMOD *modifiers, KEYMOD *curmodifiers)
{
	
}

/*
 * This reads one keystroke from the keyboard, and the current state of
 * the modifier keys (ALT, SHIFT, etc).  Returns -1 on error, 0 if no data
 * is ready, 1 on a keypress, and 2 on keyrelease.
 * This is a non-blocking call.
 */
static int
TTY_Read(KEY *kbuf, KEYMOD *modifiers, SCANCODE *pscancode)
{
	int	cc;			/* characters read */
	int 	pressed;
	int 	scancode;
	KEY	key;
	unsigned char buf[128];

	cc = read(fd, buf, 1);
	if (cc > 0) {
		pressed = (*buf & 0x80) ? RELEASED: PRESSED;
		scancode = *buf & 0x7f;
		key = keymap[scancode];

		/**if(pressed) {
			printf("scan %02x really: %08x\n", *buf&0x7F, *buf);
			printf("_key: %02x (%c)\n", _key, _key);
		}**/

		/* Handle Alt-FN for vt switch */
		switch (key) {
		case _KEY_F1:
		case _KEY_F2:
		case _KEY_F3:
		case _KEY_F4:
		case _KEY_F5:
		case _KEY_F6:
		case _KEY_F7:
		case _KEY_F8:
		case _KEY_F9:
		case _KEY_F10:
		case _KEY_F11:
		case _KEY_F12:
			if (key_modstate & _KMOD_ALT) {
				if (switch_vt(key-_KEY_F1+1)) {
					key = _KEY_REDRAW;
				}
			}
			break;
			/* Fall through to normal processing */
		default:
			/* update internal key states*/
			if (!UpdateKeyState(pressed, key))
				return 0;

			/* key is 0 if only a modifier is hit */
			if(key != _KEY_LCTRL && 
			   key != _KEY_RCTRL &&
			   key != _KEY_LALT &&
			   key != _KEY_RALT &&
			   key != _KEY_RSHIFT &&
			   key != _KEY_LSHIFT) {
				/* translate scancode to key value*/
				key = TranslateScancode(scancode, key_modstate);
			}
			break;
		}	
		*kbuf = key;
		*modifiers = key_modstate;
		*pscancode = scancode;

	
		return pressed ? 1 : 2;
	}

	if ((cc < 0) && (errno != EINTR) && (errno != EAGAIN))
		return -1;
	return 0;
}

/* Update the internal keyboard state, return TRUE if changed*/
static int
UpdateKeyState(int pressed, KEY _key)
{
	KEYMOD modstate = key_modstate;


	if (pressed == PRESSED) {
		switch (_key) {
		case _KEY_NUMLOCK:
		case _KEY_CAPSLOCK:
			/* change state on release because of auto-repeat*/
			return 0;
		case _KEY_LCTRL:
			modstate |= _KMOD_LCTRL;
			break;
		case _KEY_RCTRL:
			modstate |= _KMOD_RCTRL;
			break;
		case _KEY_LSHIFT:
			modstate |= _KMOD_LSHIFT;
			break;
		case _KEY_RSHIFT:
			modstate |= _KMOD_RSHIFT;
			break;
		case _KEY_LALT:
			modstate |= _KMOD_LALT;
			break;
		case _KEY_RALT:
			modstate |= _KMOD_RALT;
			break;
		case _KEY_LMETA:
			modstate |= _KMOD_LMETA;
			break;
		case _KEY_RMETA:
			modstate |= _KMOD_RMETA;
			break;
		case _KEY_ALTGR:
			modstate |= _KMOD_ALTGR;
			break;
		default:
			break;
		}
	} else {
		switch (_key) {
		case _KEY_NUMLOCK:
			key_modstate ^= _KMOD_NUM;
			key_state[_KEY_NUMLOCK] ^= PRESSED;
			UpdateLEDState(key_modstate);
			return 1;
		case _KEY_CAPSLOCK:
			key_modstate ^= _KMOD_CAPS;
			key_state[_KEY_CAPSLOCK] ^= PRESSED;
			UpdateLEDState(key_modstate);
			return 1;
		case _KEY_LCTRL:
			modstate &= ~_KMOD_LCTRL;
			break;
		case _KEY_RCTRL:
			modstate &= ~_KMOD_RCTRL;
			break;
		case _KEY_LSHIFT:
			modstate &= ~_KMOD_LSHIFT;
			break;
		case _KEY_RSHIFT:
			modstate &= ~_KMOD_RSHIFT;
			break;
		case _KEY_LALT:
			modstate &= ~_KMOD_LALT;
			break;
		case _KEY_RALT:
			modstate &= ~_KMOD_RALT;
			break;
		case _KEY_LMETA:
			modstate &= ~_KMOD_LMETA;
			break;
		case _KEY_RMETA:
			modstate &= ~_KMOD_RMETA;
			break;
		case _KEY_ALTGR:
			modstate &= ~_KMOD_ALTGR;
			break;
		default:
			break;
		}
	}

	/* Update internal keyboard state */
	key_state[_key] = (unsigned char)pressed;
	key_modstate = modstate;
	return 1;
}

static void
UpdateLEDState(KEYMOD modstate)
{
	int	ledstate = 0;

	if (modstate & _KMOD_CAPS)
		ledstate |= LED_CAP;
	if (modstate & _KMOD_NUM)
		ledstate |= LED_NUM;
	ioctl(fd, KDSETLED, ledstate);
}

/* translate a scancode and modifier state to an _KEY*/
static KEY
TranslateScancode(int scancode, KEYMOD modstate)
{
	unsigned short	key = 0;
	int		map = 0;

	/*printf("Translate: 0x%04x\n", scancode);*/

	/* determine appropriate kernel table*/
	if (modstate & _KMOD_SHIFT)
		map |= (1<<KG_SHIFT);
	if (modstate & _KMOD_CTRL)
		map |= (1<<KG_CTRL);
	if (modstate & _KMOD_ALT)
		map |= (1<<KG_ALT);
	if (modstate & _KMOD_ALTGR)
		map |= (1<<KG_ALTGR);
	if (KTYP(os_keymap[map][scancode]) == KT_LETTER) {
		if (modstate & _KMOD_CAPS)
			map |= (1<<KG_SHIFT);
	}
	if (KTYP(os_keymap[map][scancode]) == KT_PAD) {
		if (modstate & _KMOD_NUM) {
			switch (keymap[scancode]) {
			case _KEY_KP0:
			case _KEY_KP1:
			case _KEY_KP2:
			case _KEY_KP3:
			case _KEY_KP4:
			case _KEY_KP5:
			case _KEY_KP6:
			case _KEY_KP7:
			case _KEY_KP8:
			case _KEY_KP9:
				key = keymap[scancode] - _KEY_KP0 + '0';
				break;
			case _KEY_KP_PERIOD:
				key = '.';
				break;
			case _KEY_KP_DIVIDE:
				key = '/';
				break;
			case _KEY_KP_MULTIPLY:
				key = '*';
				break;
			case _KEY_KP_MINUS:
				key = '-';
				break;
			case _KEY_KP_PLUS:
				key = '+';
				break;
			case _KEY_KP_ENTER:
				key = _KEY_ENTER;
				break;
			case _KEY_KP_EQUALS:
				key = '-';
				break;
			}
		}
	} else
		key = KVAL(os_keymap[map][scancode]);
	
	if (!key)
		key = keymap[scancode];

	/* perform additional translations*/
	switch (key) {
	case 127:
		key = _KEY_BACKSPACE;
		break;
	case _KEY_BREAK:
	case _KEY_PAUSE:
		key = _KEY_QUIT;
		break;
	case 0x1c:	/* kernel maps print key to ctrl-\ */
	case _KEY_SYSREQ:
		key = _KEY_PRINT;
		break;
	}


	return key;
}

/* load Linux keyboard mappings, used as first try for scancode conversion*/
static void
LoadKernelKeymaps(int fd)
{
	int 		map, i;
	struct kbentry 	entry;

	/* Load all the keysym mappings */
	for (map=0; map<NUM_VGAKEYMAPS; ++map) {
		memset(os_keymap[map], 0, NR_KEYS*sizeof(unsigned short));
		for (i=0; i<NR_KEYS; ++i) {
			entry.kb_table = map;
			entry.kb_index = i;
			if (ioctl(fd, KDGKBENT, &entry) == 0) {
				/* change K_ENTER to \r*/
				if (entry.kb_value == K_ENTER)
					entry.kb_value = K(KT_ASCII,13);

				if ((KTYP(entry.kb_value) == KT_LATIN) ||
				    (KTYP(entry.kb_value) == KT_ASCII) ||
				    (KTYP(entry.kb_value) == KT_PAD) ||
				    (KTYP(entry.kb_value) == KT_LETTER)
				    )
					os_keymap[map][i] = entry.kb_value;
			}
		}
	}

}
/* Handle switching to another VC, returns when our VC is back */
static int
switch_vt(unsigned short which)
{
	struct vt_stat vtstate;
	unsigned short current;
	static unsigned short r[16], g[16], b[16];

	/* Figure out whether or not we're switching to a new console */
	if ((ioctl(fd, VT_GETSTATE, &vtstate) < 0) ||
	    (which == vtstate.v_active)) {
		return 0;
	}
	current = vtstate.v_active;

	
	
	ioctl(fd, KDSETMODE, KD_TEXT);

	/* New console, switch to it */
	if (ioctl(fd, VT_ACTIVATE, which) == 0) {
		/* Wait for our console to be activated again */
		ioctl(fd, VT_WAITACTIVE, which);
		while (ioctl(fd, VT_WAITACTIVE, current) < 0) {
			if ((errno != EINTR) && (errno != EAGAIN)) {
				/* Unknown VT error, cancel*/
				break;
			}
			usleep(100000);
		}
	}

	/* Restore graphics mode and the contents of the screen */
	ioctl(fd, KDSETMODE, KD_GRAPHICS);
	
	return 1;
}


static void
initKBD(void)
{
  TTY_Open();
}

static void
terminKBD(void)
{
  TTY_Close();
}

static int
updateKBD(KEY *kbuf, KEYMOD *modifiers, SCANCODE *pscancode)
{
  TTY_Read(kbuf, modifiers, pscancode);
}
