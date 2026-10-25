#include "chip.h"

#define SOKOL_IMPL
#define SOKOL_METAL

#include "sokol_app.h"
#include "sokol_gfx.h"
#include "sokol_glue.h"
#include "sokol_log.h"

#include "sokol_framebuffer.h"

#include <stdint.h>
#include <stdio.h>

#define BLACK 0x000000
#define WHITE 0xFFFFFF

static struct {
  sfb_framebuffer fb;
  float time;
} state;

static Chip8 chip;

static void init(void) {
  init_chip(&chip);

  sg_setup(&(sg_desc){
      .environment = sglue_environment(),
      .logger.func = slog_func,
      });

  sfb_setup(&(sfb_desc){
      .logger.func = slog_func,
      });

  state.fb = sfb_make_framebuffer(&(sfb_framebuffer_desc){
      .width = FB_WIDTH,
      .height = FB_HEIGHT,
      });
}

static void event(const sapp_event* e) {
  switch (e->type) {
    case SAPP_EVENTTYPE_KEY_DOWN:
      switch (e->key_code) {
        case SAPP_KEYCODE_1:
          chip.keypad[0] = 1;
          break;
        case SAPP_KEYCODE_2:
          chip.keypad[1] = 1;
          break;
        case SAPP_KEYCODE_3:
          chip.keypad[2] = 1;
          break;
        case SAPP_KEYCODE_4:
          chip.keypad[3] = 1;
          break;
        case SAPP_KEYCODE_Q:
          chip.keypad[4] = 1;
          break;
        case SAPP_KEYCODE_W:
          chip.keypad[5] = 1;
          break;
        case SAPP_KEYCODE_E:
          chip.keypad[6] = 1;
          break;
        case SAPP_KEYCODE_R:
          chip.keypad[7] = 1;
          break;
        case SAPP_KEYCODE_A:
          chip.keypad[8] = 1;
          break;
        case SAPP_KEYCODE_S:
          chip.keypad[9] = 1;
          break;
        case SAPP_KEYCODE_D:
          chip.keypad[10] = 1;
          break;
        case SAPP_KEYCODE_F:
          chip.keypad[11] = 1;
          break;
        case SAPP_KEYCODE_Z:
          chip.keypad[12] = 1;
          break;
        case SAPP_KEYCODE_X:
          chip.keypad[13] = 1;
          break;
        case SAPP_KEYCODE_C:
          chip.keypad[14] = 1;
          break;
        case SAPP_KEYCODE_V:
          chip.keypad[15] = 1;
          break;
        default:
          break;
      }
      break;
    case SAPP_EVENTTYPE_KEY_UP:
      switch (e->key_code) {
        case SAPP_KEYCODE_1:
          chip.keypad[0] = 0;
          break;
        case SAPP_KEYCODE_2:
          chip.keypad[1] = 0;
          break;
        case SAPP_KEYCODE_3:
          chip.keypad[2] = 0;
          break;
        case SAPP_KEYCODE_4:
          chip.keypad[3] = 0;
          break;
        case SAPP_KEYCODE_Q:
          chip.keypad[4] = 0;
          break;
        case SAPP_KEYCODE_W:
          chip.keypad[5] = 0;
          break;
        case SAPP_KEYCODE_E:
          chip.keypad[6] = 0;
          break;
        case SAPP_KEYCODE_R:
          chip.keypad[7] = 0;
          break;
        case SAPP_KEYCODE_A:
          chip.keypad[8] = 0;
          break;
        case SAPP_KEYCODE_S:
          chip.keypad[9] = 0;
          break;
        case SAPP_KEYCODE_D:
          chip.keypad[10] = 0;
          break;
        case SAPP_KEYCODE_F:
          chip.keypad[11] = 0;
          break;
        case SAPP_KEYCODE_Z:
          chip.keypad[12] = 0;
          break;
        case SAPP_KEYCODE_X:
          chip.keypad[13] = 0;
          break;
        case SAPP_KEYCODE_C:
          chip.keypad[14] = 0;
          break;
        case SAPP_KEYCODE_V:
          chip.keypad[15] = 0;
          break;
        default:
          break;
      }
    default:
      break;
  }
}

static void frame(void) {
  cycle(&chip);

  // update framebuffer with plasma pixels outside a sokol-gfx pass
  sfb_update(state.fb, &(sfb_update_desc){.pixels = SG_RANGE(chip.pixels)});

  // draw framebuffer in sokol-gfx render pass
  sg_begin_pass(&(sg_pass){
      .action = {.colors[0] = {.load_action = SG_LOADACTION_DONTCARE}},
      .swapchain = sglue_swapchain(),
      });
  sfb_render(state.fb);
  sfb_render_ex(state.fb, &(sfb_render_desc){
      .use_nearest_filter = true,
      });
  sg_end_pass();
  sg_commit();
}

static void cleanup(void) {
  sfb_shutdown();
  sg_shutdown();
}

sapp_desc sokol_main(int argc, char *argv[]) {
  (void)argc;
  (void)argv;

  // Process the command line arguments
  if (argc != 2) {
    printf("Try passing a filename and nothing else");
    exit(EXIT_FAILURE);
  }

  load_rom(argv[1], &chip);
  // Here we are going to read in the ROM into the chips memory.

  return (sapp_desc){
    .init_cb = init,
    .frame_cb = frame,
    .cleanup_cb = cleanup,
    .event_cb = event,
    .width = 800,
    .height = 600,
    .window_title = "framebuffer-sapp.c",
    .icon.sokol_default = true,
    .logger.func = slog_func,
  };
}
