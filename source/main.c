#include <string.h>
#include <stdio.h>
#include <3ds.h>
#include <sf2d.h>
#include <sftd.h>
#include "api.h"

#include "fira_sans_regular_ttf.h"
#include "fira_sans_semibold_ttf.h"

int main() {
	sf2d_init();
	sf2d_set_clear_color(RGBA8(41,75,86,255));
	sf2d_set_vblank_wait(0);
	httpcInit(0x1000);

	sftd_init();
	sftd_font *font_reg = sftd_load_font_mem(fira_sans_regular_ttf, fira_sans_regular_ttf_size);
	sftd_font *font_sb = sftd_load_font_mem(fira_sans_semibold_ttf, fira_sans_semibold_ttf_size);
	sf2d_texture *textures[6];

	textures[0] = sfil_load_PNG_file("sdmc:/3ds/sportshub/baseball.png", SF2D_PLACE_RAM);
	textures[1] = sfil_load_PNG_file("sdmc:/3ds/sportshub/basketball.png", SF2D_PLACE_RAM);
	textures[2] = sfil_load_PNG_file("sdmc:/3ds/sportshub/football.png", SF2D_PLACE_RAM);
	textures[3] = sfil_load_PNG_file("sdmc:/3ds/sportshub/hockey.png", SF2D_PLACE_RAM);
	textures[4] = sfil_load_PNG_file("sdmc:/3ds/sportshub/soccer.png", SF2D_PLACE_RAM);
	textures[5] = sfil_load_PNG_file("sdmc:/3ds/sportshub/tennis.png", SF2D_PLACE_RAM);

	while (aptMainLoop()) {
		draw_screen(font_reg, font_sb, textures);

		hidScanInput();
		u32 kDown = hidKeysDown();

		if (kDown & KEY_START) break;

		if (kDown & KEY_UP) decrement();
		if (kDown & KEY_DOWN) increment();
		if (kDown & KEY_LEFT) decrement_more();
		if (kDown & KEY_RIGHT) increment_more();
		if (kDown & KEY_B) back();
		if (kDown & KEY_A) forward();
		if (kDown & KEY_X) refresh();

		sf2d_swapbuffers();
	}

	sftd_free_font(font_sb);
	sftd_free_font(font_reg);

	for (short i; i < 6; i++) {
		sf2d_free_texture(textures[i]);
	}

	httpcExit();
	sftd_fini();
	sf2d_fini();
	return 0;
}