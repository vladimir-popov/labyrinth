#ifndef __ART__
#define __ART__

#include "u8.h"

// clang-format off

#define WELCOME_SCREEN  \
"░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░\n" \
"░░░░██╗░░░░░░█████╗░██████╗░██╗░░░██╗██████╗░██╗███╗░░██╗████████╗██╗░░██╗░░░\n" \
"░░░░██║░░░░░██╔══██╗██╔══██╗╚██╗░██╔╝██╔══██╗██║████╗░██║╚══██╔══╝██║░░██║░░░\n" \
"░░░░██║░░░░░███████║██████╦╝░╚████╔╝░██████╔╝██║██╔██╗██║░░░██║░░░███████║░░░\n" \
"░░░░██║░░░░░██╔══██║██╔══██╗░░╚██╔╝░░██╔══██╗██║██║╚████║░░░██║░░░██╔══██║░░░\n" \
"░░░░███████╗██║░░██║██████╦╝░░░██║░░░██║░░██║██║██║░╚███║░░░██║░░░██║░░██║░░░\n" \
"░░░░╚══════╝╚═╝░░╚═╝╚═════╝░░░░╚═╝░░░╚═╝░░╚═╝╚═╝╚═╝░░╚══╝░░░╚═╝░░░╚═╝░░╚═╝░░░\n" \
"░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░\n" \
"░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░\n" \
"░░░░░   ┃   ┃   ┃   ┃       ┃   ┃   ┃       ┃   ┃   ┃               ┃   ░░░░░\n" \
"░░░░░━━━┛   ┃   ┃   ┗━━┓        ┃   ┣━━━    ┃   ┃   ┗━━━━━━━━━━━    ┣━━━░░░░░\n" \
"░░░░░       ┃   ┃      ┃        ┃   ┃       ┃   ┃                   ┃   ░░░░░\n" \
"░░░░░━━━    ┃   ┗━━━┓       ┏━━━┫   ┗━━━┓       ┗━━━━━━━━━━━┳━━━    ┃   ░░░░░\n" \
"░░░░░       ┃       ┃       ┃   ┃       ┃                   ┃       ┃   ░░░░░\n" \
"░░░░░━━━┓   ┗━━━    ┗━━━┓   ┃       ━━━━┫   ━━━━┳━━━━━━━┳━━━╋━━━┓       ░░░░░\n" \
"░░░░░   ┃               ┃   ┃           ┃       ┃       ┃   ┃   ┃       ░░░░░\n" \
"░░░░░       ┏━━━━━━┓    ┃   ┗━━━┳━━━┓       ━━━━┻━━━┓   ┃   ┃   ┗━━━━━━━░░░░░\n" \
"░░░░░       ┃      ┃    ┃       ┃   ┃               ┃   ┃   ┃           ░░░░░\n" \
"░░░░░   ━━━━┫   ┏━━┛    ┗━━━┓       ┗━━━┳━━━━━━━    ┃       ┣━━━    ┏━━━░░░░░\n" \
"░░░░░       ┃   ┃           ┃           ┃           ┃       ┃       ┃   ░░░░░\n" \
"░░░░░━━━    ┃   ┃   ┏━━━━━━━┻━━━━━━━    ┗━━━┳━━━    ┗━━━┓   ┗━━━    ┃   ░░░░░\n" \
"░░░░░       ┃       ┃                       ┃           ┃           ┃   ░░░░░\n" \
"░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░\n" \
"░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░\n" 



#define LB_NEW_GAME \
"█▄░█ █▀▀ █░█░█   █▀▀ ▄▀█ █▀▄▀█ █▀▀\n" \
"█░▀█ ██▄ ▀▄▀▄▀   █▄█ █▀█ █░▀░█ ██▄"

#define LB_CONTINUE \
"█▀▀ █▀█ █▄░█ ▀█▀ █ █▄░█ █░█ █▀▀\n" \
"█▄▄ █▄█ █░▀█ ░█░ █ █░▀█ █▄█ ██▄"

#define LB_EXIT \
"█▀▀ ▀▄▀ █ ▀█▀\n" \
"██▄ █░█ █ ░█░"

#define LB_YES \
"█▄█ █▀▀ █▀\n" \
"░█░ ██▄ ▄█"

#define LB_NO \
"█▄░█ █▀█\n" \
"█░▀█ █▄█"

#define LB_QUESTION \
"▀█\n" \
"░▄"

// clang-format on

typedef struct
{
  char *vert;
  char *hor;
  char *luc;
  char *ruc;
  char *lbc;
  char *rbc;
} art_border;

#define ART_SINGLE_BORDER                                                     \
  {                                                                           \
    "┃", "━", "┏", "┓", "┗", "┛"                                            \
  }

void art_create_frame (u8buf *buf, int height, int width,
                       const art_border border);
#endif
