#include <fmt/core.h>
#include <SDL3/SDL.h>

#define WIDTH 1920
#define HEIGHT 1080

Uint32 FRAMEBUFFER[WIDTH * HEIGHT] = {0};

void draw_pixel(Uint32 x, Uint32 y, Uint32 color) {
    if (x < 0 || x >= WIDTH || y < 0 || y >= HEIGHT) {
        return;
    }
    FRAMEBUFFER[y * WIDTH + x] = color;
}

void clear(Uint32 color) {
    for (int i = 0; i < WIDTH * HEIGHT; i++) {
        FRAMEBUFFER[i] = color;
    }
}

void draw_rect(Uint32 x, Uint32 y, Uint32 w, Uint32 h, Uint32 color) {
    for (Uint32 i = 0; i < w; i++) {
        for (Uint32 j = 0; j < h; j++) {
            draw_pixel(x + i, y + j, color);
        }
    }
}

Uint32 player_x = 10;
Uint32 player_y = 10;
Uint32 player_w = 100;
Uint32 player_h = 100;
Uint32 player_c = 0x00FF00;

const bool* keyboard_state = SDL_GetKeyboardState(NULL);

int main(void) {
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Texture *texture;
    SDL_Event event;
	const double target_frame = 1.0 / 240.0;

    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)) {
		SDL_Log("SDL_Init failed: %s", SDL_GetError());
        return EXIT_FAILURE;
    }

    SDL_AudioSpec wav_spec;
    Uint8* wav_data = NULL;
    Uint32 wav_data_len = 0;

    if (!SDL_LoadWAV("music2.wav", &wav_spec, &wav_data, &wav_data_len)) {
		SDL_Log("SDL_LoadWAV failed: %s", SDL_GetError());
        SDL_Quit();
        return EXIT_FAILURE;
    }

    SDL_AudioStream* stream = SDL_OpenAudioDeviceStream(
        SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &wav_spec, NULL, NULL
    );

    if (stream) {
		SDL_SetAudioStreamGain(stream, 0.5f);

		SDL_ResumeAudioStreamDevice(stream);
    }

    window = SDL_CreateWindow(
        "Framebuffer",
        WIDTH * 1,
        HEIGHT * 1,
        SDL_WINDOW_FULLSCREEN
    );

    if (window == NULL) {
        SDL_Log("SDL_CreateWindow failed: %s", SDL_GetError());
		SDL_Quit();

        return EXIT_FAILURE;
    }

    renderer = SDL_CreateRenderer(
        window,
        NULL
    );

    if (renderer == NULL) {
        SDL_Log("SDL_CreateRenderer failed: %s", SDL_GetError());
		SDL_DestroyWindow(window);
		SDL_Quit();

        return EXIT_FAILURE;
    }

	texture = SDL_CreateTexture(
		renderer,
		SDL_PIXELFORMAT_XRGB8888,
		SDL_TEXTUREACCESS_STREAMING,
		WIDTH,
		HEIGHT
	);

	SDL_SetTextureScaleMode(texture, SDL_ScaleMode::SDL_SCALEMODE_NEAREST);

    if (texture == NULL) {
        SDL_Log("SDL_CreateTexture failed: %s", SDL_GetError());
		SDL_DestroyRenderer(renderer);
		SDL_DestroyWindow(window);
		SDL_Quit();

        return EXIT_FAILURE;
    }

    Uint32 is_running = 1;
    Uint32 frame = 1;

    while (is_running) {
        Uint64 start = SDL_GetPerformanceCounter();
        Uint64 start_time_ns = SDL_GetTicksNS();

		while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                is_running = false;
            }
		}

        if (stream) {
            if (SDL_GetAudioStreamQueued(stream) < 4096) {
				SDL_PutAudioStreamData(stream, wav_data, wav_data_len);
            }
        }

        if (keyboard_state[SDL_SCANCODE_W]) {
            player_y -= 1;
        }
        if (keyboard_state[SDL_SCANCODE_S]) {
            player_y += 1;
        }
        if (keyboard_state[SDL_SCANCODE_A]) {
            player_x -= 1;
        }
        if (keyboard_state[SDL_SCANCODE_D]) {
            player_x += 1;
        }

        if (keyboard_state[SDL_SCANCODE_ESCAPE]) {
            is_running = false;
        }

        clear(0x000000);

        /* draw_rect(100, 100, 200, 200, 0xFF0000);
        draw_rect(300, 100, 200, 200, 0x00FF00);
        draw_rect(500, 100, 200, 200, 0x0000FF);
        draw_rect(0, 980, WIDTH, 100, 0x008080); */

		draw_rect(player_x, player_y, player_w, player_h, player_c);

        SDL_UpdateTexture(
            texture,
            NULL,
            FRAMEBUFFER,
            WIDTH * sizeof(Uint32)
        );

		SDL_RenderClear(renderer);
		SDL_RenderTexture(renderer, texture, NULL, NULL);
        SDL_RenderPresent(renderer);

		Uint64 end = SDL_GetPerformanceCounter();

		double elapsed = (double)(end - start) / (double)SDL_GetPerformanceFrequency();

        if (elapsed < target_frame) {
            SDL_Delay((Uint32)((target_frame - elapsed) * 1000));
        }

        frame++;
    }

    if (stream) {
		SDL_DestroyAudioStream(stream);
    }
    if (wav_data) {
        SDL_free(wav_data);
    }

	SDL_DestroyTexture(texture);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();

    return EXIT_SUCCESS; 
}