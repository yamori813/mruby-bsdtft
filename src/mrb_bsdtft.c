/*
** mrb_bsdtft.c - BsdTft class
**
** Copyright (c) Hiroki Mori 2017
**
** See Copyright Notice in LICENSE
*/

#include "mruby.h"
#include "mruby/data.h"
#include "mruby/array.h"
#include "mrb_bsdtft.h"

#include <err.h>
#include <errno.h>
#include <sysexits.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/endian.h>

#include <sys/spigenio.h>
#include <fcntl.h>
#include <libgpio.h>

#define DONE mrb_gc_arena_restore(mrb, 0);

typedef struct {
  int model;
  int fd;
  int gpio;
  int width;
  int hight;
  int reset;
  int rs;
} mrb_bsdtft_data;

static const struct mrb_data_type mrb_bsdtft_data_type = {
  "mrb_bsdtft_data", mrb_free,
};

static mrb_value mrb_bsdtft_init(mrb_state *mrb, mrb_value self)
{
  mrb_bsdtft_data *data;
  char device[32];
  mrb_int num, gpio, model;

  data = (mrb_bsdtft_data *)DATA_PTR(self);
  if (data) {
    mrb_free(mrb, data);
  }
  DATA_TYPE(self) = &mrb_bsdtft_data_type;
  DATA_PTR(self) = NULL;

  mrb_get_args(mrb, "iii", &num, &gpio, &model);
  data = (mrb_bsdtft_data *)mrb_malloc(mrb, sizeof(mrb_bsdtft_data));
  snprintf(device, sizeof(device), "/dev/spigen%u.0", num);
  data->fd = open(device, O_RDWR);
  data->gpio = gpio_open(gpio);
  data->model = model;
  DATA_PTR(self) = data;

  return self;
}

static mrb_value mrb_bsdtft_setsize(mrb_state *mrb, mrb_value self)
{
  mrb_bsdtft_data *data = DATA_PTR(self);
  mrb_int w, h;

  mrb_get_args(mrb, "ii", &w, &h);

  data->width = w;
  data->hight = h;

  return mrb_fixnum_value(0);
}

static mrb_value mrb_bsdtft_setreset(mrb_state *mrb, mrb_value self)
{
  mrb_bsdtft_data *data = DATA_PTR(self);
  mrb_int pin;

  mrb_get_args(mrb, "i", &pin);

  data->reset = pin;

  return mrb_fixnum_value(0);
}

static mrb_value mrb_bsdtft_setrs(mrb_state *mrb, mrb_value self)
{
  mrb_bsdtft_data *data = DATA_PTR(self);
  mrb_int pin;

  mrb_get_args(mrb, "i", &pin);

  data->rs = pin;

  return mrb_fixnum_value(0);
}

static mrb_value mrb_bsdtft_width(mrb_state *mrb, mrb_value self)
{
  mrb_bsdtft_data *data = DATA_PTR(self);

  return mrb_fixnum_value(data->width);
}

static mrb_value mrb_bsdtft_hight(mrb_state *mrb, mrb_value self)
{
  mrb_bsdtft_data *data = DATA_PTR(self);

  return mrb_fixnum_value(data->hight);
}

static mrb_value mrb_bsdtft_reset(mrb_state *mrb, mrb_value self)
{
  mrb_bsdtft_data *data = DATA_PTR(self);

  return mrb_fixnum_value(data->reset);
}

static mrb_value mrb_bsdtft_rs(mrb_state *mrb, mrb_value self)
{
  mrb_bsdtft_data *data = DATA_PTR(self);

  return mrb_fixnum_value(data->rs);
}

static mrb_value mrb_bsdtft_model(mrb_state *mrb, mrb_value self)
{
  mrb_bsdtft_data *data = DATA_PTR(self);

  return mrb_fixnum_value(data->model);
}

static mrb_value mrb_bsdtft_getclk(mrb_state *mrb, mrb_value self)
{
  mrb_bsdtft_data *data = DATA_PTR(self);
  int clk;

  ioctl(data->fd, SPIGENIOC_GET_CLOCK_SPEED, &clk);

  return mrb_fixnum_value(clk);
}

static mrb_value mrb_bsdtft_setclk(mrb_state *mrb, mrb_value self)
{
  mrb_bsdtft_data *data = DATA_PTR(self);
  mrb_int val;
  int clk;

  mrb_get_args(mrb, "i", &val);
  clk = val;
  ioctl(data->fd, SPIGENIOC_SET_CLOCK_SPEED, &clk);

  return mrb_fixnum_value(clk);
}

#define MAXBUF (1024 * 4)

static mrb_value mrb_bsdtft_transfer(mrb_state *mrb, mrb_value self)
{
  mrb_bsdtft_data *data = DATA_PTR(self);
  struct spigen_transfer cmd;
  int error;
  unsigned char txbuf[MAXBUF];
  unsigned char rxbuf[MAXBUF];
  int len;
  mrb_value arr;
  mrb_value res;
  mrb_int rxsize;
  int i;

  mrb_get_args(mrb, "Ai", &arr, &rxsize);
  len = RARRAY_LEN( arr );
  if (len > MAXBUF || rxsize > MAXBUF) {
    return mrb_fixnum_value(0);
  }
  for (i = 0; i < len; ++i)
    txbuf[i] = mrb_fixnum( mrb_ary_ref( mrb, arr, i ) );

  cmd.st_command.iov_base = txbuf;
  cmd.st_command.iov_len = len;
  cmd.st_data.iov_base = rxbuf;
  cmd.st_data.iov_len = rxsize;

  error = ioctl(data->fd, SPIGENIOC_TRANSFER, &cmd);

  res = mrb_ary_new(mrb);
  for (i = 0; i < rxsize; ++i)
    mrb_ary_push(mrb, res, mrb_fixnum_value(rxbuf[i]));
  
  return res;
}

static mrb_value mrb_bsdtft_transfer2(mrb_state *mrb, mrb_value self)
{
  mrb_bsdtft_data *data = DATA_PTR(self);
  struct spigen_transfer cmd;
  int error;
  unsigned char txbuf[MAXBUF];
  unsigned char rxbuf[MAXBUF];
  mrb_int ptr;
  int color;
  int x, y;
  unsigned char *framedata;
  int i;

  mrb_get_args(mrb, "i", &ptr);
  framedata = (unsigned char *)ptr;

  for (y = 0; y < data->hight / 4; ++y) {
    i = 0;
    if (data->model == S6D0151) {
      txbuf[i] = 0x72;
      ++i;
    }
    for (x = 0; x < data->width * 4; ++x) {
#if BYTE_ORDER == BIG_ENDIAN
      txbuf[i] = *framedata++;
      txbuf[i + 1] = *framedata++;
#else
      txbuf[i + 1] = *framedata++;
      txbuf[i] = *framedata++;
#endif
      i += 2;
    }

    cmd.st_command.iov_base = txbuf;
    cmd.st_command.iov_len = i;
    cmd.st_data.iov_base = rxbuf;
    cmd.st_data.iov_len = 0;

    error = ioctl(data->fd, SPIGENIOC_TRANSFER, &cmd);
  }

  return mrb_fixnum_value(0);
}

static mrb_value mrb_bsdtft_gpio_setflags(mrb_state *mrb, mrb_value self)
{
  mrb_bsdtft_data *data = DATA_PTR(self);
  mrb_int pin, flag;
  gpio_config_t conf;

  mrb_get_args(mrb, "ii", &pin, &flag);

  conf.g_pin = pin;
  conf.g_flags = flag;
  gpio_pin_set_flags(data->gpio, &conf);

  return mrb_fixnum_value(0);
}

static mrb_value mrb_bsdtft_gpio_set(mrb_state *mrb, mrb_value self)
{
  mrb_bsdtft_data *data = DATA_PTR(self);
  mrb_int pin, val;

  mrb_get_args(mrb, "ii", &pin, &val);

  gpio_pin_set(data->gpio, pin, val);

  return mrb_fixnum_value(0);
}

void mrb_mruby_bsdtft_gem_init(mrb_state *mrb)
{
  struct RClass *bsdtft;
  bsdtft = mrb_define_class(mrb, "BsdTft", mrb->object_class);
    mrb_define_const(mrb, bsdtft, "INPUT",  mrb_fixnum_value(GPIO_PIN_INPUT));
    mrb_define_const(mrb, bsdtft, "OUTPUT",  mrb_fixnum_value(GPIO_PIN_OUTPUT));
    mrb_define_const(mrb, bsdtft, "S6D0151",  mrb_fixnum_value(S6D0151));
    mrb_define_const(mrb, bsdtft, "ST7735",  mrb_fixnum_value(ST7735));
    mrb_define_const(mrb, bsdtft, "ILI9341",  mrb_fixnum_value(ILI9341));
  mrb_define_method(mrb, bsdtft, "initialize", mrb_bsdtft_init, MRB_ARGS_REQ(1));
  mrb_define_method(mrb, bsdtft, "setsize", mrb_bsdtft_setsize, MRB_ARGS_REQ(2));
  mrb_define_method(mrb, bsdtft, "setreset", mrb_bsdtft_setreset, MRB_ARGS_REQ(1));
  mrb_define_method(mrb, bsdtft, "setrs", mrb_bsdtft_setrs, MRB_ARGS_REQ(1));
  mrb_define_method(mrb, bsdtft, "width", mrb_bsdtft_width, MRB_ARGS_NONE());
  mrb_define_method(mrb, bsdtft, "hight", mrb_bsdtft_hight, MRB_ARGS_NONE());
  mrb_define_method(mrb, bsdtft, "reset", mrb_bsdtft_reset, MRB_ARGS_NONE());
  mrb_define_method(mrb, bsdtft, "rs", mrb_bsdtft_rs, MRB_ARGS_NONE());
  mrb_define_method(mrb, bsdtft, "model", mrb_bsdtft_model, MRB_ARGS_NONE());
  mrb_define_method(mrb, bsdtft, "transfer", mrb_bsdtft_transfer, MRB_ARGS_REQ(2));
  mrb_define_method(mrb, bsdtft, "transfer2", mrb_bsdtft_transfer2, MRB_ARGS_REQ(1));
  mrb_define_method(mrb, bsdtft, "gpio_setflags", mrb_bsdtft_gpio_setflags, MRB_ARGS_REQ(2));
  mrb_define_method(mrb, bsdtft, "gpio_set", mrb_bsdtft_gpio_set, MRB_ARGS_REQ(2));
  mrb_define_method(mrb, bsdtft, "getclk", mrb_bsdtft_getclk, MRB_ARGS_NONE());
  mrb_define_method(mrb, bsdtft, "setclk", mrb_bsdtft_setclk, MRB_ARGS_REQ(1));
  DONE;
}

void mrb_mruby_bsdtft_gem_final(mrb_state *mrb)
{
}

