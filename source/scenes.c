#include <string.h>
#include <stdio.h>
#include <3ds.h>
#include <sf2d.h>
#include <sfil.h>
#include <sftd.h>
#include "api.h"

lge_vec_t leagues;
short cur_scene = 0;
const u32 WHITE = RGBA8(255,255,255,255);
const u32 BLUE1 = RGBA8(101,125,133,255);
const u32 BLUE2 = RGBA8(41,75,86,255);
const sport sports[6] = {
	{"Baseball", "baseball", 0},
	{"Basketball", "basketball", 1},
	{"Football", "american-football", 2},
	{"Hockey", "ice-hockey", 3},
	{"Soccer", "football", 4},
	{"Tennis", "tennis", 5}
};

scroll scenes[3] = {
	{ 0, 6, 4}, // sport selection
	{ 0, 0, 0}, // league selection
	{ 0, 0, 0}  // scores
};

void draw_sports_selection(sftd_font *font_sb, sf2d_texture *textures[]){
	sf2d_start_frame(GFX_TOP, GFX_LEFT);
		sf2d_draw_texture(textures[sports[scenes[0].index].texture_index], 0, 0);
	sf2d_end_frame();

	sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);
		sf2d_draw_rectangle(0, 0, 320, 240, BLUE2);
		if (scenes[cur_scene].index == 0) {
			sf2d_draw_rectangle(18, 33, 284, 39, WHITE);
		} else if (scenes[cur_scene].index == 1) {
			sf2d_draw_rectangle(18, 78, 284, 39, WHITE);
		} else if (scenes[cur_scene].index == 2) {
			sf2d_draw_rectangle(18, 123, 284, 39, WHITE);
		} else {
			sf2d_draw_rectangle(18, 168, 284, 39, WHITE);
		}

		sf2d_draw_rectangle(20, 35, 280, 35, BLUE1);
		sf2d_draw_rectangle(20, 80, 280, 35, BLUE1);
		sf2d_draw_rectangle(20, 125, 280, 35, BLUE1);
		sf2d_draw_rectangle(20, 170, 280, 35, BLUE1);

		// // text
		if (scenes[cur_scene].index <= 3) {
			sftd_draw_text(font_sb, 25, 37, WHITE, 24, sports[0].full);
			sftd_draw_text(font_sb, 25, 82, WHITE, 24, sports[1].full);
			sftd_draw_text(font_sb, 25, 127, WHITE, 24, sports[2].full);
			sftd_draw_text(font_sb, 25, 172, WHITE, 24, sports[3].full);
		} else {
			sftd_draw_text(font_sb, 25, 37, WHITE, 24, sports[scenes[0].index - 3].full);
			sftd_draw_text(font_sb, 25, 82, WHITE, 24, sports[scenes[0].index - 2].full);
			sftd_draw_text(font_sb, 25, 127, WHITE, 24, sports[scenes[0].index - 1].full);
			sftd_draw_text(font_sb, 25, 172, WHITE, 24, sports[scenes[0].index].full);
		}
		draw_scrollbar();
	sf2d_end_frame();
}

void draw_league_selection(sftd_font *font_sb, sf2d_texture *textures[]){
	sf2d_start_frame(GFX_TOP, GFX_LEFT);
		sf2d_draw_texture(textures[sports[scenes[0].index].texture_index], 0, 0);
	sf2d_end_frame();

	sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);
		sf2d_draw_rectangle(0, 0, 320, 240, BLUE2);

		// selected option
		if (leagues.length) {
			if (scenes[cur_scene].index == 0) {
				sf2d_draw_rectangle(18, 33, 284, 39, WHITE);
			} else if (scenes[cur_scene].index == 1) {
				sf2d_draw_rectangle(18, 78, 284, 39, WHITE);
			} else if (scenes[cur_scene].index == 2) {
				sf2d_draw_rectangle(18, 123, 284, 39, WHITE);
			} else {
				sf2d_draw_rectangle(18, 168, 284, 39, WHITE);
			}
		}

		switch (leagues.length) {
			case 0:
				sftd_draw_text(font_sb, 25, 39, WHITE, 20, "No games today");
				break;
			case 1:
				sf2d_draw_rectangle(20, 35, 280, 35, BLUE1);
				sftd_draw_text(font_sb, 25, 39, WHITE, 20, leagues.data[0].name);
				break;
			case 2:
				sf2d_draw_rectangle(20, 35, 280, 35, BLUE1);
				sf2d_draw_rectangle(20, 80, 280, 35, BLUE1);
				sftd_draw_text(font_sb, 25, 39, WHITE, 20, leagues.data[0].name);
				sftd_draw_text(font_sb, 25, 84, WHITE, 20, leagues.data[1].name);
				break;
			case 3:
				sf2d_draw_rectangle(20, 35, 280, 35, BLUE1);
				sf2d_draw_rectangle(20, 80, 280, 35, BLUE1);
				sf2d_draw_rectangle(20, 125, 280, 35, BLUE1);
				sftd_draw_text(font_sb, 25, 39, WHITE, 20, leagues.data[0].name);
				sftd_draw_text(font_sb, 25, 84, WHITE, 20, leagues.data[1].name);
				sftd_draw_text(font_sb, 25, 129, WHITE, 20, leagues.data[2].name);
				break;
			default:
				sf2d_draw_rectangle(20, 35, 280, 35, BLUE1);
				sf2d_draw_rectangle(20, 80, 280, 35, BLUE1);
				sf2d_draw_rectangle(20, 125, 280, 35, BLUE1);
				sf2d_draw_rectangle(20, 170, 280, 35, BLUE1);

				if (scenes[1].index < 3) {
					sftd_draw_text(font_sb, 25, 39, WHITE, 24, leagues.data[0].name);
					sftd_draw_text(font_sb, 25, 84, WHITE, 24, leagues.data[1].name);
					sftd_draw_text(font_sb, 25, 129, WHITE, 24, leagues.data[2].name);
					sftd_draw_text(font_sb, 25, 174, WHITE, 24, leagues.data[3].name);
				} else {
					sftd_draw_text(font_sb, 25, 39, WHITE, 24, leagues.data[scenes[1].index - 3].name);
					sftd_draw_text(font_sb, 25, 84, WHITE, 24, leagues.data[scenes[1].index - 2].name);
					sftd_draw_text(font_sb, 25, 129, WHITE, 24, leagues.data[scenes[1].index - 1].name);
					sftd_draw_text(font_sb, 25, 174, WHITE, 24, leagues.data[scenes[1].index].name);
				}
				break;
			}
		draw_scrollbar();
	sf2d_end_frame();
}

void draw_scores(sftd_font *font_reg, sftd_font *font_sb, sf2d_texture *textures[]) {
	sf2d_start_frame(GFX_TOP, GFX_LEFT);
		sf2d_draw_texture(textures[sports[scenes[0].index].texture_index], 0, 0);
	sf2d_end_frame();

	sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);
		sf2d_draw_rectangle(0, 0, 320, 240, BLUE2);
		sf2d_draw_rectangle(0, 0, 320, 25, BLUE1);

		if (scenes[cur_scene].num_items > 2) {
			if (scenes[cur_scene].index < 3) {
				draw_gm(35, leagues.data[scenes[1].index].gms.data[0], font_sb, font_reg);
				draw_gm(105, leagues.data[scenes[1].index].gms.data[1], font_sb, font_reg);
				draw_gm(175, leagues.data[scenes[1].index].gms.data[2], font_sb, font_reg);
			} else {
				draw_gm(35, leagues.data[scenes[1].index].gms.data[scenes[cur_scene].index - 2], font_sb, font_reg);
				draw_gm(105, leagues.data[scenes[1].index].gms.data[scenes[cur_scene].index - 1], font_sb, font_reg);
				draw_gm(175, leagues.data[scenes[1].index].gms.data[scenes[cur_scene].index], font_sb, font_reg);
			}
		} else if (scenes[cur_scene].num_items > 1) {
			draw_gm(35, leagues.data[scenes[1].index].gms.data[0], font_sb, font_reg);
			draw_gm(105, leagues.data[scenes[1].index].gms.data[1], font_sb, font_reg);
		} else {
			draw_gm(35, leagues.data[scenes[1].index].gms.data[0], font_sb, font_reg);
		}

		// selected game
		switch (scenes[cur_scene].index) {
			case 0:
				sf2d_draw_rectangle(5, 35, 3, 60, BLUE1);
				break;
			case 1:
				sf2d_draw_rectangle(5, 105, 3, 60, BLUE1);
				break;
			default:
				sf2d_draw_rectangle(5, 175, 3, 60, BLUE1);
				break;
		}
		draw_scrollbar();
	sf2d_end_frame();
}

void draw_gm(int y, game gm, sftd_font *font_sb, sftd_font *font_reg) {
	short width;
	sf2d_draw_rectangle(5, y, 305, 60, RGBA8(52,83,92,255));

	// game info
	width = sftd_get_text_width(font_sb, 15, gm.date);
	sftd_draw_text(font_sb, 10 + ((47 - width) / 2), y + 10, WHITE, 15, gm.date);
	width = sftd_get_text_width(font_sb, 15, gm.status);
	sftd_draw_text(font_sb, 10 + ((47 - width) / 2), y + 30, WHITE, 15, gm.status);

	// team names
	switch (gm.winner) {
		case 0:
			sftd_draw_text(font_reg, 60, y + 10, WHITE, 15, gm.home);
			sftd_draw_text(font_reg, 60, y + 30, WHITE, 15, gm.away);
			break;
		case 1:
			sftd_draw_text(font_sb, 60, y + 10, WHITE, 15, gm.home);
			sftd_draw_text(font_reg, 60, y + 30, WHITE, 15, gm.away);
			break;
		case 2:
			sftd_draw_text(font_reg, 60, y + 10, WHITE, 15, gm.home);
			sftd_draw_text(font_sb, 60, y + 30, WHITE, 15, gm.away);
			break;
		default:
			break;
	}

	// scores
	sftd_draw_textf(font_sb, 270, y + 10, WHITE, 15, "%3d", gm.home_score);
	sftd_draw_textf(font_sb, 270, y + 30, WHITE, 15, "%3d", gm.away_score);
}

void draw_scrollbar() {
	if (scenes[cur_scene].num_items > 3) {
		if (scenes[cur_scene].index + 1 == scenes[cur_scene].num_items) {
			sf2d_draw_rectangle(315, ((240 / scenes[cur_scene].num_items) * scenes[cur_scene].index), 5, 240 - ((240 / scenes[cur_scene].num_items) * scenes[cur_scene].index), WHITE);
		} else {
			sf2d_draw_rectangle(315, ((240 / scenes[cur_scene].num_items) * scenes[cur_scene].index), 5, (240 / scenes[cur_scene].num_items), WHITE);
		}
	}
}

void increment(){
	if (scenes[cur_scene].index + 1 == scenes[cur_scene].num_items) {
		scenes[cur_scene].index = 0;
	} else {
		scenes[cur_scene].index++;
	}
}

void increment_more(){
	if (scenes[cur_scene].index + 3 < scenes[cur_scene].num_items) {
		scenes[cur_scene].index = scenes[cur_scene].index + 3;
	} else {
		scenes[cur_scene].index = scenes[cur_scene].num_items - 1;
	}
}

void decrement(){
	if (scenes[cur_scene].index - 1 < 0) {
		scenes[cur_scene].index = scenes[cur_scene].num_items - 1;
	} else {
		scenes[cur_scene].index--;
	}
}

void decrement_more(){
	if (scenes[cur_scene].index - 3 > 0) {
		scenes[cur_scene].index = scenes[cur_scene].index - 3;
	} else {
		scenes[cur_scene].index = 0;
	}
}

void forward(){
	switch (cur_scene) {
		case 0:
			get_scores(sports[scenes[0].index].name);
			cur_scene++;
			scenes[cur_scene].index = 0;
			scenes[cur_scene].num_items = leagues.length;
			break;
		case 1:
			if (scenes[cur_scene].num_items > 0)  {
				cur_scene++;
				scenes[cur_scene].index = 0;
				scenes[cur_scene].num_items = leagues.data[scenes[1].index].gms.length;
			}
			break;
		default:
			break;
	}
}

void back(){
	switch (cur_scene) {
		case 1:
		case 2:
			cur_scene--;
			break;
		default:
			break;
	}
}

void draw_screen(sftd_font *font_reg, sftd_font *font_sb, sf2d_texture *textures){
	switch (cur_scene) {
		case 0:
			draw_sports_selection(font_sb, textures);
			break;
		case 1:
			draw_league_selection(font_sb, textures);
			break;
		case 2:
			draw_scores(font_reg, font_sb, textures);
			break;
		default:
			break;
	}
}

void refresh() {
	if (cur_scene == 2) {
		get_scores(sports[scenes[0].index].name);
	}
}