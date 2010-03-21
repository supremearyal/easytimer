/* A simple timer that goes up to 999 seconds written in C with
 * SDL. The timer concept here is that from Lazy Foo's Productions
 * (http://lazyfoo.net/) */

#include <stdio.h>
#include <stdlib.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>

#define TRUE 1
#define FALSE 0

#define MAX_ELAPSE 999
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

typedef struct
{
	timer *t;
	int reverse;

	SDL_Surface *screen;
	SDL_Surface *numbers;
	SDL_Event event;
} timer_data;

int clean_up(timer_data *t, int err);
int init_font(timer_data *t, const char *fname);

void init_timer(timer *t);
void start_timer(timer *t);
void stop_timer(timer *t);
void pause_timer(timer *t);
void unpause_timer(timer *t);
Uint32 get_ticks_timer(timer *t);

void draw_timer(timer_data *t);

int main(int argc, char *argv[])
{
	timer_data tdata;

	if((tdata.t = (timer *)malloc(sizeof(timer))) == NULL)
	{
		fprintf(stderr, "Couldn't allocate enough memory for timer.\n");
		return EXIT_FAILURE;
	}
	tdata.reverse = FALSE;
	tdata.screen = NULL;
	tdata.numbers = NULL;

	if(SDL_Init(SDL_INIT_VIDEO) != 0)
	{
		fprintf(stderr, "Couldn't initialize SDL: %s\n", SDL_GetError());
		return EXIT_FAILURE;
	}

	if((tdata.screen = SDL_SetVideoMode(WIDTH, HEIGHT, BPP,
		SDL_SWSURFACE | SDL_NOFRAME)) == NULL)
	{
		fprintf(stderr, "Couldn't set SDL video mode: %s\n",
			SDL_GetError());
		return EXIT_FAILURE;
	}

	/* Set the title of the window. */
	SDL_WM_SetCaption(TITLE, TITLE);

	/* Initialize the font image. */
	if(init_font(&tdata, "numbers.png") == FALSE)
	{
		fprintf(stderr, "Couldn't open the font image: %s\n",
			SDL_GetError());
		return EXIT_FAILURE;
	}

	/* Start the timer. */
	init_timer(tdata.t);

	while(TRUE)
	{
		/* Handle events here. */
		while(SDL_PollEvent(&(tdata.event)))
		{
			if(tdata.event.type == SDL_QUIT)
			{
				return clean_up(&tdata, EXIT_SUCCESS);
			}
			else if(tdata.event.type == SDL_KEYDOWN)
			{
				if(tdata.event.key.keysym.sym == SDLK_ESCAPE)
				{
					return clean_up(&tdata, EXIT_SUCCESS);
				}
				else if(tdata.event.key.keysym.sym == SDLK_r)
				{
					tdata.reverse = (tdata.reverse == TRUE) ? FALSE :
						TRUE;
				}
				else if(tdata.event.key.keysym.sym == SDLK_SPACE)
				{
					/* Toggle timer here. */
					if(tdata.t->paused == FALSE)
						pause_timer(tdata.t);
					else
						unpause_timer(tdata.t);
				}
				else if(tdata.event.key.keysym.sym == SDLK_RETURN)
				{
					/* Reset timer here. */
					if(tdata.t->started == FALSE)
						start_timer(tdata.t);
					else
						stop_timer(tdata.t);
				}
			}
		}

		if(get_ticks_timer(tdata.t) / 1000 <= MAX_ELAPSE)
			draw_timer(&tdata);
		else
			stop_timer(tdata.t);

		/* Draw the time. */
		SDL_Flip(tdata.screen);

		SDL_Delay(34);
	}

	return EXIT_SUCCESS;
}

int clean_up(timer_data *t, int err)
{
	free(t->t);
	SDL_FreeSurface(t->numbers);
	SDL_Quit();
	return err;
}

int init_font(timer_data *t, const char *fname)
{
	/* Open the file. */
	if((t->numbers = IMG_Load(fname)) == NULL)
		return FALSE;
	else
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

void draw_timer(timer_data *t)
{
	Uint32 secs;
	SDL_Rect clip_font, clip_screen;

	/* We want seconds not milliseconds. */
	if(t->reverse == TRUE)
		secs = MAX_ELAPSE - get_ticks_timer(t->t) / 1000;
	else
		secs = get_ticks_timer(t->t) / 1000;

	/* First digit. */
	clip_font.x = (secs % 1000) / 100 * CHAR_WIDTH;
	clip_font.y = 0;
	clip_font.w = CHAR_WIDTH;
	clip_font.h = CHAR_HEIGHT;

	clip_screen.x = 0;
	clip_screen.y = 0;

	SDL_BlitSurface(t->numbers, &clip_font, t->screen, &clip_screen);

	/* Second digit. */
	clip_font.x = (secs % 100) / 10 * CHAR_WIDTH;

	clip_screen.x = CHAR_WIDTH;
	clip_screen.y = 0;
	
	SDL_BlitSurface(t->numbers, &clip_font, t->screen, &clip_screen);

	/* Third digit. */
	clip_font.x = (secs % 10) * CHAR_WIDTH;

	clip_screen.x = 2 * CHAR_WIDTH;
	clip_screen.y = 0;

	SDL_BlitSurface(t->numbers, &clip_font, t->screen, &clip_screen);
}
