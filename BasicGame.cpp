#include <stdio.h>
#include <allegro5/allegro5.h>
#include <allegro5/allegro_image.h>

void assure_init(bool test, const char* description)
{
  if (test) return;
  printf("Initialization error %s\n", description);
  exit(1);
}

#define DISP_BUFFER_W 380
#define DISP_BUFFER_H 200
#define DISP_SCALE 4
#define DISP_W (DISP_BUFFER_W * DISP_SCALE)
#define DISP_H (DISP_BUFFER_H * DISP_SCALE)
ALLEGRO_DISPLAY* disp;
ALLEGRO_BITMAP* buffer;

char key[ALLEGRO_KEY_MAX];

#define HERO_W 19
#define HERO_H 24
#define HERO_SPEED 3
#define HERO_MAX_X (DISP_BUFFER_W - HERO_W)
#define HERO_MAX_Y (DISP_BUFFER_H - HERO_H)

typedef struct HERO
{
  ALLEGRO_BITMAP* img;
  int x, y;
} HERO;
HERO hero;

void hero_init()
{
  hero.img = al_load_bitmap("resources/Hero.png");
  assure_init(hero.img, "hero image");
  hero.x = (DISP_BUFFER_W / 2) - (HERO_W / 2);
  hero.y = (DISP_BUFFER_H / 2) - (HERO_H / 2);
}

void hero_move()
{
  if (key[ALLEGRO_KEY_LEFT]) hero.x -= HERO_SPEED;
  if (key[ALLEGRO_KEY_RIGHT]) hero.x += HERO_SPEED;
  if (key[ALLEGRO_KEY_UP]) hero.y -= HERO_SPEED;
  if (key[ALLEGRO_KEY_DOWN]) hero.y += HERO_SPEED;
  if (hero.x < 0) hero.x = 0;
  if (hero.y < 0) hero.y = 0;
  if (hero.x > HERO_MAX_X) hero.x = HERO_MAX_X;
  if (hero.y > HERO_MAX_Y) hero.y = HERO_MAX_Y;
}

void read_keyboard(ALLEGRO_EVENT* event)
{
  switch (event->type)
  {
    case ALLEGRO_EVENT_KEY_DOWN:
      key[event->keyboard.keycode] = true;
      break;
    case ALLEGRO_EVENT_KEY_UP:
      key[event->keyboard.keycode] = false;
      break;
  }
}

int main()
{
  assure_init(al_init(), "allegro");
  assure_init(al_install_keyboard(), "keyboard");
  ALLEGRO_TIMER* timer = al_create_timer(1.0 / 30.0);
  assure_init(timer, "timer");
  ALLEGRO_EVENT_QUEUE* queue = al_create_event_queue();
  assure_init(queue, "queue");
  disp = al_create_display(DISP_W, DISP_H);
  assure_init(disp, "display");
  buffer = al_create_bitmap(DISP_BUFFER_W, DISP_BUFFER_H);  
  assure_init(al_init_image_addon(), "image");
  al_register_event_source(queue, al_get_keyboard_event_source());
  al_register_event_source(queue, al_get_display_event_source(disp));
  al_register_event_source(queue, al_get_timer_event_source(timer));

  hero_init();
  bool gamefinished = false;
  ALLEGRO_EVENT event;
  al_start_timer(timer);
  while (true)
  {
    al_wait_for_event(queue, &event);
    read_keyboard(&event);
    switch (event.type)
    {
      case ALLEGRO_EVENT_TIMER:
        hero_move();
        if (key[ALLEGRO_KEY_ESCAPE]) gamefinished=true;
        break;

      case ALLEGRO_EVENT_DISPLAY_CLOSE:
        gamefinished=true;
        break;
    }

    if (gamefinished) break;

    al_set_target_bitmap(buffer);
    al_clear_to_color(al_map_rgb(50, 10, 10));
    al_draw_bitmap(hero.img, hero.x, hero.y, 0);
    al_set_target_bitmap(al_get_backbuffer(disp));
    al_draw_scaled_bitmap(buffer, 0, 0, DISP_BUFFER_W, DISP_BUFFER_H, 0, 0, DISP_W, DISP_H, 0);
    al_flip_display();
  }

  al_destroy_bitmap(hero.img);
  al_destroy_bitmap(buffer);
  al_destroy_display(disp);
  al_destroy_timer(timer);
  al_destroy_event_queue(queue);

  return 0;
}