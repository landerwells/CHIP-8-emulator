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

static void frame(void) {
  for (int y = 0; y < FB_HEIGHT; y++) {
    for (int x = 0; x < FB_WIDTH; x++) {
      if (chip.pixels[y][x] == BLACK) {
        chip.pixels[y][x] = WHITE;
      } else {
        chip.pixels[y][x] = BLACK;
      }
    }
  }

  // update framebuffer with plasma pixels outside a sokol-gfx pass
  sfb_update(state.fb, &(sfb_update_desc){.pixels = SG_RANGE(chip.pixels)});

  // draw framebuffer in sokol-gfx render pass
  sg_begin_pass(&(sg_pass){
      .action = {.colors[0] = {.load_action = SG_LOADACTION_DONTCARE}},
      .swapchain = sglue_swapchain(),
      });
  sfb_render(state.fb);
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

  printf("%s\n", argv[1]);

  load_rom(argv[1], &chip);
  // Here we are going to read in the ROM into the chips memory.

  return (sapp_desc){
    .init_cb = init,
    .frame_cb = frame,
    .cleanup_cb = cleanup,
    .width = 800,
    .height = 600,
    .window_title = "framebuffer-sapp.c",
    .icon.sokol_default = true,
    .logger.func = slog_func,
  };
}
