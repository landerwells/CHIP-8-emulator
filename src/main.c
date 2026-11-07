#include "chip.h"

#define SOKOL_IMPL
#define SOKOL_METAL

#include "sokol_app.h"
#include "sokol_gfx.h"
#include "sokol_glue.h"
#include "sokol_log.h"

#include "sokol_framebuffer.h"
#include "sokol_audio.h"

#include <stdint.h>
#include <stdio.h>

static struct {
  sfb_framebuffer fb;
} state;

static Chip8 chip;

// I know this function is bad.
static void stream(float* buffer, int num_frames, int num_channels) {
  uint32_t even_odd = 0;
  if (chip.sound_timer > 0) {
    for (int i = 0; i < num_frames; i++) {
      if (even_odd++ & (1<<5)) {
        buffer[i] = 0.05f;
      } else {
        buffer[i] = -0.05f;
      }
    }
  } else {
    for (int i = 0; i < num_frames; i++) {
      buffer[i] = 0.0f;
    }
  }
}

static void init(void) {
  init_chip(&chip);

  sg_setup(&(sg_desc){
      .environment = sglue_environment(),
      .logger.func = slog_func,
      });

  sfb_setup(&(sfb_desc){
      .logger.func = slog_func,
      });

  saudio_setup(&(saudio_desc){
      .stream_cb = stream,
      .logger.func = slog_func,
      });

  saudio_isvalid();

  state.fb = sfb_make_framebuffer(&(sfb_framebuffer_desc){
      .width = FB_WIDTH,
      .height = FB_HEIGHT,
      });
}

static void event(const sapp_event *e) {
  switch (e->type) {
    case SAPP_EVENTTYPE_KEY_DOWN:
      switch (e->key_code) {
        case SAPP_KEYCODE_1:
          chip.keypad[1] = 1;
          break;
        case SAPP_KEYCODE_2:
          chip.keypad[2] = 1;
          break;
        case SAPP_KEYCODE_3:
          chip.keypad[3] = 1;
          break;
        case SAPP_KEYCODE_4:
          chip.keypad[0xC] = 1;
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
          chip.keypad[0xD] = 1;
          break;
        case SAPP_KEYCODE_A:
          chip.keypad[7] = 1;
          break;
        case SAPP_KEYCODE_S:
          chip.keypad[8] = 1;
          break;
        case SAPP_KEYCODE_D:
          chip.keypad[9] = 1;
          break;
        case SAPP_KEYCODE_F:
          chip.keypad[0xE] = 1;
          break;
        case SAPP_KEYCODE_Z:
          chip.keypad[0xA] = 1;
          break;
        case SAPP_KEYCODE_X:
          chip.keypad[0] = 1;
          break;
        case SAPP_KEYCODE_C:
          chip.keypad[0xB] = 1;
          break;
        case SAPP_KEYCODE_V:
          chip.keypad[0xF] = 1;
          break;
        default:
          break;
      }
      break;
    case SAPP_EVENTTYPE_KEY_UP:
      switch (e->key_code) {
        case SAPP_KEYCODE_1:
          chip.keypad[1] = 0;
          break;
        case SAPP_KEYCODE_2:
          chip.keypad[2] = 0;
          break;
        case SAPP_KEYCODE_3:
          chip.keypad[3] = 0;
          break;
        case SAPP_KEYCODE_4:
          chip.keypad[0xC] = 0;
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
          chip.keypad[0xD] = 0;
          break;
        case SAPP_KEYCODE_A:
          chip.keypad[7] = 0;
          break;
        case SAPP_KEYCODE_S:
          chip.keypad[8] = 0;
          break;
        case SAPP_KEYCODE_D:
          chip.keypad[9] = 0;
          break;
        case SAPP_KEYCODE_F:
          chip.keypad[0xE] = 0;
          break;
        case SAPP_KEYCODE_Z:
          chip.keypad[0xA] = 0;
          break;
        case SAPP_KEYCODE_X:
          chip.keypad[0] = 0;
          break;
        case SAPP_KEYCODE_C:
          chip.keypad[0xB] = 0;
          break;
        case SAPP_KEYCODE_V:
          chip.keypad[0xF] = 0;
          break;
        default:
          break;
      }
    default:
      break;
  }
}


const double fixed_delta_time = 1.0 / 60.0;
double ticks = 0.0;
double previous_ticks = 0.0;
const double hertz = 1.0 / 500.0;

static void frame(void) {
  ticks += sapp_frame_duration();
  if (ticks -  previous_ticks >= fixed_delta_time) {
    update_timers(&chip);
    previous_ticks = ticks;
  }

  double accumulator = sapp_frame_duration();
  while (accumulator >= hertz) {
    cycle(&chip);
    accumulator -= hertz;
  }

  sfb_update(state.fb, &(sfb_update_desc){.pixels = SG_RANGE(chip.pixels)});

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
  saudio_shutdown();
  sg_shutdown();
}

sapp_desc sokol_main(int argc, char *argv[]) {
  if (argc != 2) {
    printf("Usage: %s <ROM>", argv[0]);
    exit(EXIT_FAILURE);
  }

  char const *rom_filename = argv[1];

  load_rom(rom_filename, &chip);

  return (sapp_desc){
    .init_cb = init,
    .frame_cb = frame,
    .cleanup_cb = cleanup,
    .event_cb = event,
    .width = 1000,
    .height = 400,
    .window_title = "CHIP-8",
    .icon.sokol_default = true,
    .logger.func = slog_func,
  };
}
