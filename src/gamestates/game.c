/*! \file empty.c
 *  \brief Empty gamestate.
 */
/*
 * Copyright (c) Sebastian Krzyszkowiak <dos@dosowisko.net>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "../common.h"
#include <libsuperderpy.h>

struct GamestateResources {
	// This struct is for every resource allocated and used by your gamestate.
	// It gets created on load and then gets passed around to all other function calls.

    ALLEGRO_SAMPLE *music;
    ALLEGRO_SAMPLE_INSTANCE *music_instance;

    ALLEGRO_BITMAP *bg, *escher, *vinyl, *vinyl2;
    double counter;

    double popout;
    double cooldown;
    double shoot;
    bool out;

    int stats, caught;

    double pos;
bool reverse;
struct {
    double rot;
    bool on;
} tables[9];

    struct {
        bool on;
        double x, y;
        double dx, dy;
        double counter;
    } vinyls[32];

    char* result;
    double alpha;

    ALLEGRO_FONT *font;
};

int Gamestate_ProgressCount = 4; // number of loading steps as reported by Gamestate_Load; 0 when missing

static void Shoot(struct Game* game, struct GamestateResources *data) {
    for (int i=0; i<32; i++) {
      if(!data->vinyls[i].on) {
          data->stats++;
          data->vinyls[i].on = true;
          data->vinyls[i].x = data->reverse ? 0.9 : 0.1;
          data->vinyls[i].y = 0.2;
          data->vinyls[i].counter = 0;
          data->vinyls[i].dx = (rand() / (double)RAND_MAX) * 0.02;
          if (data->reverse) data->vinyls[i].dx = -data->vinyls[i].dx;
          data->vinyls[i].dy = -0.005 + (rand() / (double)RAND_MAX) * 0.007;
          break;
      }
    }
}

void Gamestate_Logic(struct Game* game, struct GamestateResources* data, double delta) {
	// Here you should do all your game logic as if <delta> seconds have passed.
    data->counter += delta * 1.5;
    if (data->alpha > 0) {
       data->alpha -= delta * 100;
    } else {data->alpha = 0; }
    for (int i=0; i<9; i++) {
        data->tables[i].rot += delta * 1.5;
    }
    for (int i=0; i<32; i++) {
        data->vinyls[i].counter += delta * 1.5;
        data->vinyls[i].x += data->vinyls[i].dx;
        data->vinyls[i].y += data->vinyls[i].dy;
        data->vinyls[i].dy += delta / 16.0;
        if (data->vinyls[i].y > 1.2) {
            data->vinyls[i].on = false;
        }
if(data->vinyls[i].on) {
        if ((data->vinyls[i].y > 0.9) && (data->vinyls[i].y < 1.0)) {
            for (int j = -4; j <= 4; j++) {
                if (!data->tables[j+4].on) {
                int x = 1920 * data->pos - 200 + j * 400, x2 = 1920 * data->pos + 400 - 200 + j * 400;
                if (data->vinyls[i].x * 1920 > x && data->vinyls[i].x * 1920 < x2) {
                    data->tables[j+4].on = true;
                    data->vinyls[i].on = false;
                    data->caught++;
                }
                }
            }
        }
}
    }

    if (data->cooldown <= 0) {
        if (data->cooldown < 0 && !data->out) {
            for (int j = -4; j <= 4; j++) {
                data->tables[j+4].on = false;

            }
            if (!data->alpha) {
            snprintf(data->result, 255, "%d/%d", data->caught, data->stats);
            data->stats = 0;
            data->caught = 0;
            data->alpha = 255;
            }
        }
        if (data->out) {
         data->popout -= delta * 1.75;
        } else {
       data->popout += delta * 1.5;
        }
       if (data->popout >= ALLEGRO_PI / 2) {
           data->cooldown = 1.5;
           data->out = true;
           data->shoot = 0;
           data->popout = ALLEGRO_PI / 2;
        }
       if (data->popout < 0) {
           data->popout = 0;
           data->out = false;
           data->reverse = rand() % 2;
           data->cooldown = 1;
       }
    } else {
        data->cooldown -= delta;
    }


    if (data->popout > ALLEGRO_PI * 0.4) {
            data->shoot -= delta;
            if (data->shoot < 0) {
                Shoot(game, data);
                data->shoot = 0.2 + (rand() / (double)RAND_MAX) * 0.2;
            }

    }



}

void Gamestate_Draw(struct Game* game, struct GamestateResources* data) {
	// Draw everything to the screen here.
    al_draw_bitmap(data->bg, 0, 0, 0);
    ALLEGRO_TRANSFORM t, p, o, ot;
    al_copy_transform(&o, al_get_current_projection_transform());
    al_copy_transform(&ot, al_get_current_transform());

    for (int i = -4; i <= 4; i++) {
        al_draw_filled_rectangle(1920 * data->pos - 200 + i * 400, 1080-300, 1920 * data->pos + 400 - 200 + i * 400, 1080, al_premul_rgba(255, 255, 255, i % 2 ? 64 : 92));
        al_draw_tinted_scaled_rotated_bitmap(data->tables[i+4].on ? data->vinyl : data->vinyl2, data->tables[i+4].on ? al_premul_rgba(255, 255, 255, 255) : al_premul_rgba(255, 255, 255, 64), al_get_bitmap_width(data->vinyl2) / 2, al_get_bitmap_height(data->vinyl2) / 2,
                                      1920 * data->pos - 200 + i * 400 + 150, 1080 - 150,
                                      300 / (double)al_get_bitmap_width(data->vinyl2),
                                      300 / (double)al_get_bitmap_width(data->vinyl2),
                                      data->tables[i+4].on ? data->tables[i+4].rot : 0, 0);
    }

    al_copy_transform(&t, &ot);
    if (data->reverse) {
    al_scale_transform(&t, -1, 1);
    al_translate_transform(&t, game->clip_rect.w, 0);
    }
    al_use_transform(&t);
    al_draw_scaled_rotated_bitmap(data->escher, al_get_bitmap_width(data->escher) / 2, al_get_bitmap_height(data->escher), 20 + sin(data->popout) * 200 - 200, 420, 0.5, 0.5, 0.2 + 0.15 * sin(data->popout), 0);

    for (int i=0; i<32; i++) {
    if (!data->vinyls[i].on) continue;
    al_identity_transform(&p);
    al_orthographic_transform(&p, 0, 0, -150, 1920, 1080, 150);
    al_identity_transform(&t);
    al_rotate_transform_3d(&t, 1, 0, 0, fabs((sin(data->vinyls[i].counter + ALLEGRO_PI / 2)) * 0.7 + 0.1) * ALLEGRO_PI / 2);
    al_translate_transform(&t, data->vinyls[i].x * 1920, data->vinyls[i].y * 1080);
    al_use_transform(&t);
    al_use_projection_transform(&p);

    al_draw_scaled_rotated_bitmap(data->vinyl, al_get_bitmap_width(data->vinyl) / 2, al_get_bitmap_height(data->vinyl) / 2,
                                  0, 0,
                                  300 / (double)al_get_bitmap_width(data->vinyl),
                                  300 / (double)al_get_bitmap_width(data->vinyl),
                                  data->vinyls[i].counter, 0);


    }
    al_use_projection_transform(&o);
    al_use_transform(&ot);

    if(data->alpha) {
        al_draw_text(data->font, al_map_rgba_f(0.46,0.24,0.83, data->alpha / 255.0), 1920 / 2, 1080 / 3, ALLEGRO_ALIGN_CENTER, data->result);
    }

}

void Gamestate_ProcessEvent(struct Game* game, struct GamestateResources* data, ALLEGRO_EVENT* ev) {
	// Called for each event in Allegro event queue.
	// Here you can handle user input, expiring timers etc.
	if ((ev->type == ALLEGRO_EVENT_KEY_DOWN) && (ev->keyboard.keycode == ALLEGRO_KEY_ESCAPE)) {
		UnloadCurrentGamestate(game); // mark this gamestate to be stopped and unloaded
		// When there are no active gamestates, the engine will quit.
	}

    if (ev->type == ALLEGRO_EVENT_MOUSE_AXES) {
        data->pos = Clamp(0, 1, (ev->mouse.x - game->clip_rect.x) / (double)game->clip_rect.w);
    }

    if (ev->type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) data->counter = 0;
}

void* Gamestate_Load(struct Game* game, void (*progress)(struct Game*)) {
	// Called once, when the gamestate library is being loaded.
	// Good place for allocating memory, loading bitmaps etc.
	//
	// Keep in mind that there's no OpenGL context available here. If you want to prerender something,
	// create VBOs, etc. do it in Gamestate_PostLoad.

	struct GamestateResources* data = calloc(1, sizeof(struct GamestateResources));
    data->bg = al_load_bitmap(GetDataFilePath(game, "bg.png"));
	progress(game); // report that we progressed with the loading, so the engine can move a progress bar
    data->escher = al_load_bitmap(GetDataFilePath(game, "escher.png"));
    progress(game);
    data->vinyl = al_load_bitmap(GetDataFilePath(game, "vinyl.png"));
    progress(game);
    data->vinyl2 = al_load_bitmap(GetDataFilePath(game, "vinyl2.png"));
    progress(game);

    data->music = al_load_sample(GetDataFilePath(game, "aaa.flac"));
    data->music_instance = al_create_sample_instance(data->music);
    al_set_sample_instance_playmode(data->music_instance, ALLEGRO_PLAYMODE_LOOP);
    al_attach_sample_instance_to_mixer(data->music_instance, game->audio.music);

    data->result = malloc(255);
    for (int i=0; i<9; i++) {

      data->tables[i].rot = (rand() / (double)RAND_MAX) * ALLEGRO_PI;
    }

    data->font = al_load_font(GetDataFilePath(game, "Monoton-Regular.ttf"), 255, 0);
	return data;
}

void Gamestate_Unload(struct Game* game, struct GamestateResources* data) {
	// Called when the gamestate library is being unloaded.
	// Good place for freeing all allocated memory and resources.
    al_destroy_font(data->font);
    al_destroy_bitmap(data->bg);
    al_destroy_bitmap(data->escher);
    al_destroy_bitmap(data->vinyl);
    al_destroy_sample_instance(data->music_instance);
    al_destroy_sample(data->music);
    free(data->result);
    free(data);
}

void Gamestate_Start(struct Game* game, struct GamestateResources* data) {
	// Called when this gamestate gets control. Good place for initializing state,
	// playing music etc.
    al_play_sample_instance(data->music_instance);
}

void Gamestate_Stop(struct Game* game, struct GamestateResources* data) {
	// Called when gamestate gets stopped. Stop timers, music etc. here.
    al_stop_sample_instance(data->music_instance);
}

// Optional endpoints:

void Gamestate_PostLoad(struct Game* game, struct GamestateResources* data) {
	// This is called in the main thread after Gamestate_Load has ended.
	// Use it to prerender bitmaps, create VBOs, etc.
}

void Gamestate_Pause(struct Game* game, struct GamestateResources* data) {
	// Called when gamestate gets paused (so only Draw is being called, no Logic nor ProcessEvent)
	// Pause your timers and/or sounds here.
}

void Gamestate_Resume(struct Game* game, struct GamestateResources* data) {
	// Called when gamestate gets resumed. Resume your timers and/or sounds here.
}

void Gamestate_Reload(struct Game* game, struct GamestateResources* data) {
	// Called when the display gets lost and not preserved bitmaps need to be recreated.
	// Unless you want to support mobile platforms, you should be able to ignore it.
}
