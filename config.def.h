/* See LICENSE file for copyright and license details. */
#include <X11/XF86keysym.h>
#include <X11/Xutil.h>

#define TERMINAL "ghostty"
#define BROWSER  "google-chrome-stable"

/* appearance */
static const unsigned int borderpx       = 3;        /* border pixel of windows */
static const unsigned int snap           = 32;       /* snap pixel */
static const unsigned int systraypinning = 0;   /* 0: sloppy systray follows selected monitor, >0: pin systray to monitor X */
static const unsigned int systrayonleft  = 0;    /* 0: systray in the right corner, >0: systray on left of status text */
static const unsigned int systrayspacing = 2;   /* systray spacing */
static const int systraypinningfailfirst = 1;   /* 1: if pinning fails, display systray on the first monitor, False: display systray on the last monitor*/
static const int showsystray             = 1;        /* 0 means no systray */
static const int showbar                 = 1;        /* 0 means no bar */
static const int topbar                  = 1;        /* 0 means bottom bar */
static const char *fonts[]               = { "JetBrainsMono Nerd Font:size=11", "Noto Sans:size=11", "Noto Color Emoji:size=16" };
static const char dmenufont[]            = "monospace:size=11";
static const char col_gray1[]            = "#222222";
static const char col_gray2[]            = "#444444";
static const char col_gray3[]            = "#bbbbbb";
static const char col_gray4[]            = "#eeeeee";
static const char col_cyan[]             = "#005577";
static const char col_red[]              = "#770000";
static const char *colors[][3]           = {
	/*               fg         bg         border   */
	[SchemeNorm] = { col_gray3, col_gray1, col_gray2 },
	[SchemeInv]  = { col_gray1, col_gray3, col_gray2 },
	[SchemeSel]  = { col_gray4, col_cyan,  col_red },
};

typedef struct {
	const char *name;
	const void *cmd;
} Sp;
// the window title here must match the rules below
const char *spcmd1[] = {"st", "-n", "spterm", "-g", "120x34", NULL };
const char *spcmd2[] = {"st", "-n", "spcalc", "-g", "50x20", "-e", "bc", "-lq", NULL };
const char *spcmd3[] = { TERMINAL, "--x11-instance-name=spyazi", "--title=spyazi", "--window-height=50", "--window-width=240", "-e", "yazi", NULL };
const char *spcmd4[] = { TERMINAL, "--x11-instance-name=spnotes", "--title=spnotes", "--window-height=50", "--window-width=240", "-e", "nvim --cmd 'cd ~/Dropbox/slug/PMG/notes/Obsidian/personal'", NULL };
const char *spcmd5[] = { TERMINAL, "--x11-instance-name=spnotes", "--title=spnotes", "--window-height=50", "--window-width=240", "-e", "nvim --cmd 'cd ~/Dropbox/slug/PMG/notes/Obsidian/gebit/Gebit'", NULL };
static Sp scratchpads[] = {
	/* name          cmd  */
	{"spterm",      spcmd1},
	{"spcalc",    spcmd2},
	{"spyazi",   spcmd3},
    {"spnotes",  spcmd4},
    {"spgebitnotes", spcmd5},
};

/* tagging */
static char *tags[] = { "1", "2", "3", "4", "5", "6", "7", "8", "9" };
static const Rule rules[] = {
	/* xprop(1):
	 *	WM_CLASS(STRING) = instance, class
	 *	WM_NAME(STRING) = title
	 */
	/* class      instance    title       tags mask     isfloating   monitor */
	{ "Gimp",	  NULL,			  NULL,		0,				1,			 -1 },
	{ "Firefox",  NULL,			  NULL,		1 << 8,			0,			 -1 },
	{ NULL,		  "spterm",		  NULL,		SPTAG(0),		1,			 -1 },
	{ NULL,		  "spcalc",		  NULL,		SPTAG(1),		1,			 -1 },
	{ NULL,		  "spyazi",	      NULL,		SPTAG(2),		1,			 -1 },
    { NULL,		  "spnotes",      NULL,		SPTAG(3),		1,			 -1 },
    { NULL,       "spgebitnotes", NULL,     SPTAG(4),       1,           -1 },
};

/* layout(s) */
static const float mfact     = 0.55; /* factor of master area size [0.05..0.95] */
static const int nmaster     = 1;    /* number of clients in master area */
static const int resizehints = 1;    /* 1 means respect size hints in tiled resizals */
static const int lockfullscreen = 1; /* 1 will force focus on the fullscreen window */

#include "layouts.c"
static const Layout layouts[] = {
	/* symbol     arrange function */
	{ "[M]",      monocle },
	{ "HHH",      grid },
	{ "[]=",      tile },    /* first entry is default */
	{ "><>",      NULL },    /* no layout function means floating behavior */
	{ "|M|",      centeredmaster },
	{ ">M>",      centeredfloatingmaster },
	{ "[D]",      deck },
	{ "TTT",      bstack },
	{ "===",      bstackhoriz },
	{ NULL,       NULL },
};

/* key definitions */
// #define MODKEY Mod1Mask
#define MODKEY Mod4Mask
#define TAGKEYS(KEY,TAG) \
	{ MODKEY,                       KEY,      view,           {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask,           KEY,      toggleview,     {.ui = 1 << TAG} }, \
	{ MODKEY|ShiftMask,             KEY,      tag,            {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask|ShiftMask, KEY,      toggletag,      {.ui = 1 << TAG} },
#define STACKKEYS(MOD,ACTION) \
	{ MOD,	XK_j,	ACTION##stack,	{.i = INC(+1) } }, \
	{ MOD,	XK_k,	ACTION##stack,	{.i = INC(-1) } }, \
	{ MOD,  XK_v,   ACTION##stack,  {.i = 0 } }, \

/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd) { .v = (const char*[]){ "/bin/sh", "-c", cmd, NULL } }

/* commands */
static char dmenumon[2] = "0"; /* component of dmenucmd, manipulated in spawn() */
static const char *dmenucmd[] = { "rofi", "-show", "drun", "-show-icons", NULL };
static const char *runcmd[] = { "rofi", "-show", "run", "-show-icons", NULL };
static const char *termcmd[]  = { TERMINAL, NULL };
static const char *roficlip_cmd[]  = { "rofi-clip", NULL };
static const char *rofipass_cmd[]  = { "rofi-pass", NULL };
static const char *volupcmd[] = { "wpctl", "set-volume", "@DEFAULT_AUDIO_SINK@", "0.05+", NULL };
static const char *voldowncmd[] = { "wpctl", "set-volume", "@DEFAULT_AUDIO_SINK@", "0.05-", NULL };
static const char *volmutecmd[] = { "wpctl", "set-mute", "@DEFAULT_AUDIO_SINK@", "toggle", NULL };

static const Key keys[] = {
	/* modifier                     key        function        argument */
	//    STACKKEYS(MODKEY,               focus)
	// STACKKEYS(MODKEY|ShiftMask,        push)
    { 0,                                  XF86XK_AudioRaiseVolume, spawn,          {.v = volupcmd } },
    { 0,                                  XF86XK_AudioLowerVolume, spawn,          {.v = voldowncmd } },
    { 0,                                  XF86XK_AudioMute,        spawn,          {.v = volmutecmd } },
    { 0,                                  XK_Print,                spawn,          SHCMD("maim -s | xclip -selection clipboard -t image/png") },
    { ControlMask|Mod4Mask,               XK_c,                    spawn,          {.v = roficlip_cmd } },
    // { ControlMask|Mod4Mask,            XK_v,                    spawn,          SHCMD("rofi-clip -p") },
    { ControlMask|Mod4Mask,               XK_p,                    spawn,          {.v = rofipass_cmd } },
    // { ControlMask|Mod4Mask,            XK_c,                    spawn,          SHCMD("rofi-clip") },
    { MODKEY,                             XK_w,                    spawn,          {.v = (const char*[]){ BROWSER,  NULL } } },
    { MODKEY,                             XK_s,                    togglesticky,   {0} },
	{ MODKEY,                             XK_d,                    spawn,          {.v = dmenucmd } },
	{ MODKEY,                             XK_F2,                   spawn,          {.v = runcmd } },
	{ MODKEY,                             XK_Return,               spawn,          {.v = termcmd } },
	{ MODKEY,                             XK_b,                    togglebar,      {0} },
	{ MODKEY,                             XK_j,                    focusstack,     {.i = +1 } },
	{ MODKEY,                             XK_k,                    focusstack,     {.i = -1 } },
	{ MODKEY,                             XK_o,                    incnmaster,     {.i = +1 } },
	{ MODKEY|ShiftMask,                   XK_o,                    incnmaster,     {.i = -1 } },
	{ MODKEY,                             XK_h,                    setmfact,       {.f = -0.05} },
	{ MODKEY,                             XK_l,                    setmfact,       {.f = +0.05} },
	{ MODKEY,                             XK_space,                zoom,           {0} },
	{ MODKEY,                             XK_Tab,                  view,           {0} },
	{ MODKEY,                             XK_q,                    killclient,     {0} },
	{ MODKEY|ShiftMask,                   XK_m,                    setlayout,      {.v = &layouts[0]} },            // monocle
	{ MODKEY|ShiftMask,                   XK_g,                    setlayout,      {.v = &layouts[1]} },            // grid
    { MODKEY|ShiftMask,                   XK_t,                    setlayout,      {.v = &layouts[2]} },            // tile
    { MODKEY|ShiftMask,                   XK_f,                    setlayout,      {.v = &layouts[3]} },            // floating
    { MODKEY,                             XK_c,                    setlayout,      {.v = &layouts[4]} },            // centeredmaster
    { MODKEY|ShiftMask,                   XK_c,                    setlayout,      {.v = &layouts[5]} },            // centeredfloatingmaster
    { MODKEY|ShiftMask,                   XK_d,                    setlayout,      {.v = &layouts[6]} },            // deck
    { MODKEY|ShiftMask,                   XK_u,                    setlayout,      {.v = &layouts[7]} },            // bstack
    { MODKEY|ShiftMask,                   XK_o,                    setlayout,      {.v = &layouts[8]} },            // bstackhoriz
	{ MODKEY|ControlMask,                 XK_comma,                cyclelayout,    {.i = -1 } },
	{ MODKEY|ControlMask,                 XK_period,               cyclelayout,    {.i = +1 } },
	// { MODKEY,                          XK_space,                setlayout,      {0} },
	{ MODKEY|ShiftMask,                   XK_space,                togglefloating, {0} },
	{ MODKEY,                             XK_0,                    view,           {.ui = ~0 } },
	{ MODKEY|ShiftMask,                   XK_0,                    tag,            {.ui = ~0 } },
	{ MODKEY,                             XK_comma,                focusmon,       {.i = -1 } },
	{ MODKEY,                             XK_period,               focusmon,       {.i = +1 } },
	{ MODKEY|ShiftMask,                   XK_comma,                tagmon,         {.i = -1 } },
	{ MODKEY|ShiftMask,                   XK_period,               tagmon,         {.i = +1 } },
	TAGKEYS(                        XK_1, 0)
	TAGKEYS(                        XK_2, 1)
	TAGKEYS(                        XK_3, 2)
	TAGKEYS(                        XK_4, 3)
	TAGKEYS(                        XK_5, 4)
	TAGKEYS(                        XK_6, 5)
	TAGKEYS(                        XK_7, 6)
	TAGKEYS(                        XK_8, 7)
	TAGKEYS(                        XK_9, 8)
	{ MODKEY|ShiftMask,                   XK_q,                    quit,           {0} },
    { MODKEY,                             XK_h,                    setmfact,       {.f = -0.05} },                  // Shrink master window
    { MODKEY,                             XK_l,                    setmfact,       {.f = +0.05} },                  // Grow master window
	// { MODKEY|ShiftMask,                   XK_r,                    spawn,          {.v = (const char*[]){ TERMINAL, "-e", "htop",       NULL } } },
	// { MODKEY,                             XK_r,                    spawn,          {.v = (const char*[]){ TERMINAL, "-e", "yazi",       NULL } } },
	// { MODKEY|ShiftMask,                   XK_w,                    spawn,          {.v = (const char*[]){ TERMINAL, "-e", "connmanctl", NULL } } },
	{ MODKEY|ShiftMask,                   XK_Return,                    togglescratch,  {.ui = 0 } }, // terminal
	{ MODKEY,                             XK_apostrophe,                    togglescratch,  {.ui = 1 } }, // calculator
	{ MODKEY,                             XK_e,                    togglescratch,  {.ui = 2 } }, // yazi
    { MODKEY,                             XK_n,                    togglescratch,  {.ui = 3 } }, // notes
    { MODKEY|ShiftMask,                   XK_n,                    togglescratch,  {.ui = 4 } }, // gebit notes
};

/* button definitions */
/* click can be ClkTagBar, ClkLtSymbol, ClkStatusText, ClkWinTitle, ClkClientWin, or ClkRootWin */
static const Button buttons[] = {
	/* click                event mask      button          function        argument */
	{ ClkLtSymbol,          0,              Button1,        setlayout,      {0} },
	{ ClkLtSymbol,          0,              Button3,        setlayout,      {.v = &layouts[2]} },
	{ ClkWinTitle,          0,              Button2,        zoom,           {0} },
	{ ClkStatusText,        0,              Button2,        spawn,          {.v = termcmd } },
	{ ClkClientWin,         MODKEY,         Button1,        movemouse,      {0} },
	{ ClkClientWin,         MODKEY,         Button2,        togglefloating, {0} },
	{ ClkClientWin,         MODKEY,         Button3,        resizemouse,    {0} },
	{ ClkTagBar,            0,              Button1,        view,           {0} },
	{ ClkTagBar,            0,              Button3,        toggleview,     {0} },
	{ ClkTagBar,            MODKEY,         Button1,        tag,            {0} },
	{ ClkTagBar,            MODKEY,         Button3,        toggletag,      {0} },
};

