/* See LICENSE file for copyright and license details. */
#include <X11/XF86keysym.h>
#include <X11/Xutil.h>

// #define TERMINAL "ghostty"
#define TERMINAL "kitty"
#define BROWSER  "google-chrome-stable"

/* appearance */
static const unsigned int borderpx       = 4;        /* border pixel of windows */
static const unsigned int snap           = 32;       /* snap pixel */
static const unsigned int systraypinning = 0;   /* 0: sloppy systray follows selected monitor, >0: pin systray to monitor X */
static const unsigned int systrayonleft  = 0;    /* 0: systray in the right corner, >0: systray on left of status text */
static const unsigned int systrayspacing = 2;   /* systray spacing */
static const int systraypinningfailfirst = 1;   /* 1: if pinning fails, display systray on the first monitor, False: display systray on the last monitor*/
static const int showsystray             = 1;        /* 0 means no systray */
static const int showbar            = 1;        /* 0 means no standard bar */
static const int topbar             = 1;        /* 0 means standard bar at bottom */
static const int extrabar           = 0;        /* 0 means no extra bar */
static const char statussep         = ';';      /* separator between statuses */
#define ICONSIZE bh   /* icon size */
#define ICONSPACING 12 /* space between icon and title */
static const int vertpad            = 10;       /* vertical padding of bar */
static const int sidepad            = 10;       /* horizontal padding of bar */
static const char *fonts[]               = { "JetBrainsMono Nerd Font:size=10", "Noto Sans:size=10", "Noto Color Emoji:size=15" };
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
	[SchemeHov]  = { col_gray4, col_cyan,  col_cyan  },
	[SchemeHid]  = { col_cyan,  col_gray1, col_cyan  },
};

typedef struct {
	const char *name;
	const void *cmd;
} Sp;
// the window title here must match the rules below
#ifdef GHOSTTY
const char *spcmd1[] = {"st", "-n", "spterm", "-g", "120x34", NULL };
const char *spcmd2[] = {"st", "-n", "spcalc", "-g", "50x20", "-e", "bc", "-lq", NULL };
const char *spcmd3[] = { TERMINAL, "--x11-instance-name=spyazi", "--title=spyazi", "--window-height=50", "--window-width=240", "-e", "yazi", NULL };
const char *spcmd4[] = { TERMINAL, "--x11-instance-name=spnotes", "--title=spnotes", "--window-height=50", "--window-width=240", "-e", "nvim --cmd 'cd ~/Dropbox/slug/PMG/notes/Obsidian/personal'", NULL };
const char *spcmd5[] = { TERMINAL, "--x11-instance-name=spgebitnotes", "--title=spgebitnotes", "--window-height=50", "--window-width=240", "-e", "nvim --cmd 'cd ~/Dropbox/slug/PMG/notes/Obsidian/gebit/Gebit'", NULL };
const char *spcmd6[] = { TERMINAL, "--x11-instance-name=speditscratch", "--title=speditscratch", "--window-height=50", "--window-width=240", "-e", "cd ~/tmp && nvim -c 'bd' 'bd'", NULL };
#else
const char *spcmd1[] = {TERMINAL, "--name", "spterm", NULL };
const char *spcmd2[] = {TERMINAL, "--name", "spcalc", "sh", "-c", "bc -lq", NULL };
const char *spcmd3[] = { TERMINAL, "--name=spyazi", "--title=spyazi", "yazi", NULL };
const char *spcmd4[] = { TERMINAL, "--name=spnotes", "--title=spnotes", "sh", "-c", "nvim --cmd 'cd ~/Dropbox/slug/PMG/notes/Obsidian/personal'", NULL };
const char *spcmd5[] = { TERMINAL, "--name=spgebitnotes", "--title=spgebitnotes", "sh", "-c", "nvim --cmd 'cd ~/Dropbox/slug/PMG/notes/Obsidian/gebit/Gebit'", NULL };
const char *spcmd6[] = { TERMINAL, "--name=speditscratch", "--title=speditscratch", "-d", "~/tmp", "sh", "-c", "nvim -c 'bd' 'bd'", NULL };
#endif
static Sp scratchpads[] = {
	/* name          cmd  */
	{"spterm",      spcmd1},
	{"spcalc",    spcmd2},
	{"spyazi",   spcmd3},
    {"spnotes",  spcmd4},
    {"spgebitnotes", spcmd5},
    {"speditscratch", spcmd6},
};

/* tagging */
static char *tags[] = { "1", "2", "3", "4", "5", "6", "7", "8", "9" };
static const Rule rules[] = {
	/* xprop(1):
	 *	WM_CLASS(STRING) = instance, class
	 *	WM_NAME(STRING) = title
	 */
	/* class,    instance,        title,                                   tags mask, isfloating, monitor */
	{ "Gimp",    NULL,            NULL,                                    0,         1,          -1 },
	{ "Firefox", NULL,            NULL,                                    1 << 8,    0,          -1 },
	{ NULL,      NULL,            "TPiSCAN Test DevStation",               1 << 3,    1,          -1 },
	{ NULL,      NULL,            "VSS Dev Tools",                         1 << 3,    1,          -1 },
	{ NULL,      NULL,            "[dev-tools - assco local-core-dn-sco]", 1 << 3,    1,          -1 },
	{ NULL,      NULL,            "Lane Proxy 4711",                       1 << 3,    1,          -1 },
	{ NULL,      "spterm",        NULL,                                    SPTAG(0),  1,          -1 },
	{ NULL,      "spcalc",        NULL,                                    SPTAG(1),  1,          -1 },
	{ NULL,      "spyazi",        NULL,                                    SPTAG(2),  1,          -1 },
    { NULL,      "spnotes",       NULL,                                    SPTAG(3),  1,          -1 },
    { NULL,      "spgebitnotes",  NULL,                                    SPTAG(4),  1,          -1 },
    { NULL,      "speditscratch", NULL,                                    SPTAG(5),  1,          -1 },
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
static const char *rofiwindowcmd[] = { "rofi", "-show", "window", "-show-icons", NULL };
static const char *termcmd[]  = { TERMINAL, NULL };
// static const char *termcmd[]  = { "tabbed", "-cr", "2", "st", "-w", "''", NULL };
static const char *roficlip_cmd[]  = { "rofi-clip", NULL };
static const char *rofipass_cmd[]  = { "rofi-pass", NULL };
static const char *volupcmd[] = { "wpctl", "set-volume", "@DEFAULT_AUDIO_SINK@", "0.05+", NULL };
static const char *voldowncmd[] = { "wpctl", "set-volume", "@DEFAULT_AUDIO_SINK@", "0.05-", NULL };
static const char *volmutecmd[] = { "wpctl", "set-mute", "@DEFAULT_AUDIO_SINK@", "toggle", NULL };
static const char *matrix_cmd[]  = { "cmatrix", "-f", NULL };
static const char *layoutmenu_cmd = "layoutmenu.sh";

static const Key keys[] = {
	/* modifier                     key        function        argument */
	//    STACKKEYS(MODKEY,               focus)
	// STACKKEYS(MODKEY|ShiftMask,        push)
    { 0,                                  XF86XK_AudioRaiseVolume, spawn,          {.v = volupcmd } },
    { 0,                                  XF86XK_AudioLowerVolume, spawn,          {.v = voldowncmd } },
    { 0,                                  XF86XK_AudioMute,        spawn,          {.v = volmutecmd } },
    { 0,                                  XK_Print,                spawn,          SHCMD("maim -s -b 5 | xclip -selection clipboard -t image/png") },
    { ControlMask|Mod4Mask,               XK_c,                    spawn,          {.v = roficlip_cmd } },
    // { ControlMask|Mod4Mask,            XK_v,                    spawn,          SHCMD("rofi-clip -p") },
    { ControlMask|Mod4Mask,               XK_p,                    spawn,          {.v = rofipass_cmd } },
    // { ControlMask|Mod4Mask,            XK_c,                    spawn,          SHCMD("rofi-clip") },
    { MODKEY,                             XK_w,                    spawn,          {.v = (const char*[]){ BROWSER,  NULL } } },
    { MODKEY,                             XK_m,                    spawn,          {.v = matrix_cmd } },
    { MODKEY,                             XK_s,                    togglesticky,   {0} },
	{ MODKEY,                             XK_d,                    spawn,          {.v = dmenucmd } },
	{ MODKEY,                             XK_F2,                   spawn,          {.v = runcmd } },
	{ MODKEY,                             XK_Return,               spawn,          {.v = termcmd } },
	{ Mod1Mask,                           XK_Tab,                  spawn,          {.v = rofiwindowcmd } },
	{ MODKEY,                             XK_b,                    togglebar,      {0} },
	{ MODKEY|ShiftMask,                   XK_b,                    toggleextrabar, {0} },
	{ MODKEY,                       XK_j,      focusstackvis,  {.i = +1 } },
	{ MODKEY,                       XK_k,      focusstackvis,  {.i = -1 } },
	{ MODKEY|ShiftMask,             XK_j,      focusstackhid,  {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_k,      focusstackhid,  {.i = -1 } },
	{ MODKEY,                             XK_o,                    incnmaster,     {.i = +1 } },
	{ MODKEY|ShiftMask,                   XK_o,                    incnmaster,     {.i = -1 } },
    { MODKEY,                             XK_comma,                setmfact,       {.f = -0.05} },                  // Shrink master window
    { MODKEY,                             XK_period,               setmfact,       {.f = +0.05} },                  // Grow master window
	{ MODKEY,                             XK_space,                zoom,           {0} },
	{ MODKEY,                             XK_Tab,                  view,           {0} },
	{ MODKEY,                             XK_q,                    killclient,     {0} },
	{ MODKEY|ShiftMask,                   XK_m,                    setlayout,      {.v = &layouts[0]} },            // monocle
	{ MODKEY|ShiftMask,                   XK_g,                    setlayout,      {.v = &layouts[1]} },            // grid
    { MODKEY|ShiftMask,                   XK_t,                    setlayout,      {.v = &layouts[2]} },            // tile
    { MODKEY|ShiftMask,                   XK_f,                    setlayout,      {.v = &layouts[3]} },            // floating
    // { MODKEY,                          XK_c,                    setlayout,      {.v = &layouts[4]} },            // centeredmaster
    // { MODKEY|ShiftMask,                XK_c,                    setlayout,      {.v = &layouts[5]} },            // centeredfloatingmaster
    { MODKEY|ShiftMask,                   XK_d,                    setlayout,      {.v = &layouts[6]} },            // deck
    // { MODKEY|ShiftMask,                XK_u,                    setlayout,      {.v = &layouts[7]} },            // bstack
    // { MODKEY|ShiftMask,                XK_o,                    setlayout,      {.v = &layouts[8]} },            // bstackhoriz
    { MODKEY|ControlMask,                 XK_l,                    layoutmenu,     {0} },
	{ MODKEY|ControlMask,                 XK_comma,                cyclelayout,    {.i = -1 } },
	{ MODKEY|ControlMask,                 XK_period,               cyclelayout,    {.i = +1 } },
	{ MODKEY,                             XK_space,                setlayout,      {0} },
	{ MODKEY|ShiftMask,                   XK_space,                togglefloating, {0} },
	{ MODKEY,                             XK_0,                    view,           {.ui = ~0 } },
	{ MODKEY|ShiftMask,                   XK_0,                    tag,            {.ui = ~0 } },
	{ MODKEY,                             XK_h,                    focusmon,       {.i = -1 } },
	{ MODKEY,                             XK_l,                    focusmon,       {.i = +1 } },
	{ MODKEY|ShiftMask,                   XK_h,                    tagmon,         {.i = -1 } },
	{ MODKEY|ShiftMask,                   XK_l,                    tagmon,         {.i = +1 } },
	{ MODKEY,                             XK_x,                    movecenter,     {0} },
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
	// { MODKEY|ShiftMask,                XK_r,                    spawn,          {.v = (const char*[]){ TERMINAL, "-e", "htop",       NULL } } },
	// { MODKEY,                          XK_r,                    spawn,          {.v = (const char*[]){ TERMINAL, "-e", "yazi",       NULL } } },
	// { MODKEY|ShiftMask,                XK_w,                    spawn,          {.v = (const char*[]){ TERMINAL, "-e", "connmanctl", NULL } } },
	{ MODKEY|ShiftMask,                   XK_Return,               togglescratch,  {.ui = 0 } },                    // terminal
	{ MODKEY,                             XK_apostrophe,           togglescratch,  {.ui = 1 } },                    // calculator
	{ MODKEY,                             XK_e,                    togglescratch,  {.ui = 2 } },                    // yazi
    { MODKEY,                             XK_n,                    togglescratch,  {.ui = 3 } },                    // notes
    { MODKEY|ShiftMask,                   XK_n,                    togglescratch,  {.ui = 4 } },                    // gebit notes
    { MODKEY|ShiftMask,                   XK_e,                    togglescratch,  {.ui = 5 } },                    // scratch edit
	{ MODKEY,                             XK_Down,                 moveresize,     {.v = "0x 75y 0w 0h" } },
	{ MODKEY,                             XK_Up,                   moveresize,     {.v = "0x -75y 0w 0h" } },
	{ MODKEY,                             XK_Right,                moveresize,     {.v = "75x 0y 0w 0h" } },
	{ MODKEY,                             XK_Left,                 moveresize,     {.v = "-75x 0y 0w 0h" } },
	{ MODKEY|ShiftMask,                   XK_Down,                 moveresize,     {.v = "0x 0y 0w 75h" } },
	{ MODKEY|ShiftMask,                   XK_Up,                   moveresize,     {.v = "0x 0y 0w -75h" } },
	{ MODKEY|ShiftMask,                   XK_Right,                moveresize,     {.v = "0x 0y 75w 0h" } },
	{ MODKEY|ShiftMask,                   XK_Left,                 moveresize,     {.v = "0x 0y -75w 0h" } },
	{ MODKEY|ControlMask,                 XK_Up,                   moveresizeedge, {.v = "t"} },
	{ MODKEY|ControlMask,                 XK_Down,                 moveresizeedge, {.v = "b"} },
	{ MODKEY|ControlMask,                 XK_Left,                 moveresizeedge, {.v = "l"} },
	{ MODKEY|ControlMask,                 XK_Right,                moveresizeedge, {.v = "r"} },
	{ MODKEY|ControlMask|ShiftMask,       XK_Up,                   moveresizeedge, {.v = "T"} },
	{ MODKEY|ControlMask|ShiftMask,       XK_Down,                 moveresizeedge, {.v = "B"} },
	{ MODKEY|ControlMask|ShiftMask,       XK_Left,                 moveresizeedge, {.v = "L"} },
	{ MODKEY|ControlMask|ShiftMask,       XK_Right,                moveresizeedge, {.v = "R"} },
	{ MODKEY,                       XK_a,      show,           {0} },
	{ MODKEY|ShiftMask,             XK_s,      showall,        {0} },
	{ MODKEY,                       XK_i,      hide,           {0} },
};

/* button definitions */
/* click can be ClkTagBar, ClkLtSymbol, ClkStatusText, ClkWinTitle, ClkClientWin, or ClkRootWin */
static const Button buttons[] = {
	/* click                event mask      button          function        argument */
	{ ClkLtSymbol,          0,              Button1,        setlayout,      {0} },
	{ ClkLtSymbol,          0,              Button3,        setlayout,      {.v = &layouts[2]} },
	{ ClkLtSymbol,          0,              Button3,        layoutmenu,     {0} },
	{ ClkWinTitle,          0,              Button1,        togglewin,      {0} },
	{ ClkWinTitle,          0,              Button2,        zoom,           {0} },
	{ ClkStatusText,        0,              Button2,        spawn,          {.v = termcmd } },
	{ ClkExBarLeftStatus,   0,              Button2,        spawn,          {.v = termcmd } },
	{ ClkExBarMiddle,       0,              Button2,        spawn,          {.v = termcmd } },
	{ ClkExBarRightStatus,  0,              Button2,        spawn,          {.v = termcmd } },
	{ ClkClientWin,         MODKEY,         Button1,        movemouse,      {0} },
	{ ClkClientWin,         MODKEY,         Button2,        togglefloating, {0} },
	{ ClkClientWin,         MODKEY,         Button3,        resizemouse,    {0} },
	{ ClkTagBar,            0,              Button1,        view,           {0} },
	{ ClkTagBar,            0,              Button3,        toggleview,     {0} },
	{ ClkTagBar,            MODKEY,         Button1,        tag,            {0} },
	{ ClkTagBar,            MODKEY,         Button3,        toggletag,      {0} },
};

