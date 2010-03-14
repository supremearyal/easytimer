/* A simple timer that goes up to 999 seconds written in C with
 * SDL. The timer concept here is that from Lazy Foo's Productions
 * (http://lazyfoo.net/) */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>

#define TRUE 1
#define FALSE 0

#define DIGITS 10
#define MAX_ELAPSE DIGITS * DIGITS * DIGITS - 1
#define CHAR_WIDTH 50
#define CHAR_HEIGHT 100

#define WIDTH CHAR_WIDTH * 3
#define HEIGHT CHAR_HEIGHT
#define BPP 32

#define TITLE "Simple Timer"

typedef struct
{
	Uint32 start_ticks;
	Uint32 paused_ticks;
	int paused;
	int started;
} timer;

int clean_up(SDL_Surface **nums, int err);
int init_digits(SDL_Surface **nums, const char *prefix, const char *ext);

void init_timer(timer *t);
void start_timer(timer *t);
void stop_timer(timer *t);
void pause_timer(timer *t);
void unpause_timer(timer *t);
Uint32 get_ticks_timer(timer *t);

void draw_timer(SDL_Surface *screen, SDL_Surface **nums, timer *t);

int main(int argc, char *argv[])
{
	SDL_Surface *screen;
	SDL_Surface *numbers[DIGITS];
	SDL_Event event;
	
	timer tim;

	if(SDL_Init(SDL_INIT_VIDEO) != 0)
	{
		fprintf(stderr, "Couldn't initialize SDL: %s\n", SDL_GetError());
		return EXIT_FAILURE;
	}

	if((screen = SDL_SetVideoMode(WIDTH, HEIGHT, BPP,
				SDL_SWSURFACE | SDL_NOFRAME))
		== NULL)
	{
		fprintf(stderr, "Couldn't set SDL video mode: %s\n",
			SDL_GetError());
		return EXIT_FAILURE;
	}

	SDL_WM_SetCaption(TITLE, TITLE);

	if(init_digits(numbers, "digit", "png") == FALSE)
		return EXIT_FAILURE;

	init_timer(&tim);

	while(TRUE)
	{
		/* Handle events here. */
		while(SDL_PollEvent(&event))
		{
			if(event.type == SDL_QUIT)
			{
				return clean_up(numbers, EXIT_SUCCESS);
			}
			else if(event.type == SDL_KEYDOWN)
			{
				if(event.key.keysym.sym == SDLK_ESCAPE)
				{
					return clean_up(numbers, EXIT_SUCCESS);
				}
				else if(event.key.keysym.sym == SDLK_SPACE)
				{
					/* Toggle timer here. */
					if(tim.paused == FALSE)
						pause_timer(&tim);
					else
						unpause_timer(&tim);
				}
				else if(event.key.keysym.sym == SDLK_RETURN)
				{
					/* Reset timer here. */
					if(tim.started == FALSE)
						start_timer(&tim);
					else
						stop_timer(&tim);
				}
			}
		}

		if(get_ticks_timer(&tim) / 1000 <= MAX_ELAPSE)
			draw_timer(screen, numbers, &tim);
		else
			stop_timer(&tim);

		/* Draw the time. */
		SDL_Flip(screen);

		SDL_Delay(34);
	}

	return EXIT_SUCCESS;
}

int clean_up(SDL_Surface **nums, int err)
{
	int i;

	for(i = 0; i < DIGITS; i++)
		SDL_FreeSurface(nums[i]);

	SDL_Quit();
	return err;
}

int init_digits(SDL_Surface **nums, const char *prefix, const char *ext)
{
	char *tmp;
	int i;
	
	/* Allocate a string long enough for prefix plus the number
	 * plus the extension (and the dot). */
	tmp = (char *)malloc(sizeof(char) * 
				(strlen(prefix) + 2 + strlen(ext)));

	if(tmp == NULL)
	{
		fprintf(stderr, "Couldn't allocate enough memory.\n");
		return FALSE;
	}

	/* Concatenate with a loop and set the surfaces. */
	for(i = 0; i < DIGITS; i++)
	{
		sprintf(tmp, "%s%d.%s", prefix, i, ext);
		/* Initialize surface here and if any of them cannot be
		 * initialized we return false. */
		if((nums[i] = IMG_Load(tmp)) == NULL)
		{
			fprintf(stderr, "Couldn't load a digit (image file): %s\n",
					SDL_GetError());
			free(tmp);
			return FALSE;
		}
	}
	
	free(tmp);
	return TRUE;
}

void init_timer(timer *t)
{
	t->start_ticks = 0;
	t->paused_ticks = 0;
	t->paused = FALSE;
	t->started = FALSE;
}

void start_timer(timer *t)
{
	t->started = TRUE;
	t->paused = FALSE;
	t->start_ticks = SDL_GetTicks();
}

void stop_timer(timer *t)
{
	t->started = FALSE;
	t->paused = FALSE;
}

void pause_timer(timer *t)
{
	if((t->started == TRUE) && (t->paused == FALSE))
	{
		t->paused = TRUE;
		t->paused_ticks = SDL_GetTicks() - t->start_ticks;
	}
}

void unpause_timer(timer *t)
{
	if(t->paused == TRUE)
	{
		t->paused = FALSE;
		t->start_ticks = SDL_GetTicks() - t->paused_ticks;
		t->paused_ticks = 0;
	}
}

Uint32 get_ticks_timer(timer *t)
{
	if(t->started == TRUE)
	{
		if(t->paused == TRUE)
			return t->paused_ticks;
		else
			return SDL_GetTicks() - t->start_ticks;
	}

	return 0;
}

void draw_timer(SDL_Surface *screen, SDL_Surface **nums, timer *t)
{
	Uint32 secs;
	SDL_Rect clip_screen;

	/* We want seconds not milliseconds. */
	secs = get_ticks_timer(t) / 1000;

	/* First digit. */
	clip_screen.x = 0;
	clip_screen.y = 0;
	clip_screen.w = CHAR_WIDTH;
	clip_screen.h = CHAR_HEIGHT;

	SDL_BlitSurface(nums[(secs % 1000) / 100], NULL, screen,
					&clip_screen);

	/* Second digit. */
	clip_screen.x = CHAR_WIDTH;
	clip_screen.y = 0;
	clip_screen.w = CHAR_WIDTH;
	clip_screen.h = CHAR_HEIGHT;
	
	SDL_BlitSurface(nums[(secs % 100) / 10], NULL, screen,
					&clip_screen);

	/* Third digit. */
	clip_screen.x = 2 * CHAR_WIDTH;
	clip_screen.y = 0;
	clip_screen.w = CHAR_WIDTH;
	clip_screen.h = CHAR_HEIGHT;

	SDL_BlitSurface(nums[secs % 10], NULL, screen, &clip_screen);
}
