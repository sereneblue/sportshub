#include "vec.h"
#ifndef api__h
#define api__h

void get_scores();

typedef struct game {
	char *home;
	char *away;
	char date[6];
	char *status;
	short winner;
	short home_score;
	short away_score;
} game;

typedef vec_t(game) gm_vec_t;

typedef struct league {
	char *name;
	gm_vec_t gms;
} league;

typedef struct scroll {
	short index;
	short num_items;
	short num_display;
} scroll;

typedef struct sport {
	char *full;
	char *name;
	short texture_index;
} sport;

typedef vec_t(league) lge_vec_t;
extern lge_vec_t leagues;

#endif