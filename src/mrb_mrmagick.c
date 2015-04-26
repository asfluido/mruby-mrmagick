/*
** mrb_mrmagick.c - Mrmagick class
**
** Copyright (c) Junichi Kajiwara 2015
**
** See Copyright Notice in LICENSE
*/
#include <string.h>
#include <stdio.h>
#include <uuid.h>

#include "mruby.h"
#include "mruby/data.h"
#include "mruby/string.h"
#include "mruby/array.h"
#include "mrb_mrmagick.h"

extern void myputs();
extern void scale(const char *src_path, const char *dst_path, const char*ratio);
extern void blur(const char *src_path, const char *dst_path,
   const double radius, const double sigma);

#define DONE mrb_gc_arena_restore(mrb, 0);

typedef struct {
  char *str;
  int len;
} mrb_mrmagick_data;

static const struct mrb_data_type mrb_mrmagick_data_type = {
  "mrb_mrmagick_data", mrb_free,
};

static mrb_value mrb_mrmagick_init(mrb_state *mrb, mrb_value self)
{
  mrb_mrmagick_data *data;
  char *str;
  int len;

  data = (mrb_mrmagick_data *)DATA_PTR(self);
  if (data) {
    mrb_free(mrb, data);
  }
  DATA_TYPE(self) = &mrb_mrmagick_data_type;
  DATA_PTR(self) = NULL;

  mrb_get_args(mrb, "s", &str, &len);
  data = (mrb_mrmagick_data *)mrb_malloc(mrb, sizeof(mrb_mrmagick_data));
  data->str = str;
  data->len = len;
  DATA_PTR(self) = data;

  return self;
}

static mrb_value mrb_mrmagick_hello(mrb_state *mrb, mrb_value self)
{
  mrb_mrmagick_data *data = DATA_PTR(self);

  return mrb_str_new(mrb, data->str, data->len);
}

static mrb_value mrb_mrmagick_scale(mrb_state *mrb, mrb_value self)
{
  char *src_path, *dst_path, *ratio;
  mrb_get_args(mrb, "zzz", &src_path, &dst_path, &ratio);
  scale(src_path, dst_path, ratio);
  return mrb_str_new_cstr(mrb, "hi!!");
}

static mrb_value mrb_mrmagick_blur(mrb_state *mrb, mrb_value self)
{
  char *src_path, *dst_path;
  mrb_float radius, sigma;
  mrb_get_args(mrb, "zzff", &src_path, &dst_path, &radius, &sigma);
  blur(src_path, dst_path, (double)radius, (double)sigma);
  return mrb_str_new_cstr(mrb, "hi!!");
}

static mrb_value mrb_mrmagick_uuid(mrb_state *mrb, mrb_value self)
{
  char *uuid_str;
  uuid_t *uuid;

  uuid_create(&uuid);
  uuid_make(uuid, UUID_MAKE_V4);
  uuid_str = NULL;
  uuid_export(uuid, UUID_FMT_STR, (void **)&uuid_str, NULL);
  uuid_destroy(uuid);

  return mrb_str_new_cstr(mrb, uuid_str);
}


static mrb_value mrb_mrmagick_rm(mrb_state *mrb, mrb_value self)
{
  mrb_value ary,val;

  int num_files;
  char filepath[1024];

  mrb_get_args(mrb, "A", &ary);
  num_files = RARRAY_LEN(ary);
  printf("delete Files %d\n",num_files);
  for ( int i = 0; i < num_files; ++i ){
    val = mrb_ary_ref(mrb, ary, i);
    //mrb_funcall(mrb, mrb_top_self(mrb), "p", 1, val);
    strncpy(filepath, RSTRING_PTR(val),RSTRING_LEN(val));
    filepath[RSTRING_LEN(val)]='\0';
    printf("array[%d]=%s\n",i,filepath);
    remove(filepath);
  }
  return mrb_str_new_cstr(mrb, "hi!!");
}


static mrb_value mrb_mrmagick_hi(mrb_state *mrb, mrb_value self)
{
	myputs();
  return mrb_str_new_cstr(mrb, "hi!!");
}

void mrb_mruby_mrmagick_gem_init(mrb_state *mrb)
{
    struct RClass *mrmagick_module;
    struct RClass *mrmagick;

    mrmagick_module = mrb_define_module(mrb, "Mrmagick");
    mrmagick = mrb_define_class_under(mrb, mrmagick_module, "Capi", mrb->object_class);

    mrb_define_method(mrb, mrmagick, "initialize", mrb_mrmagick_init, MRB_ARGS_REQ(1));
    mrb_define_method(mrb, mrmagick, "hello", mrb_mrmagick_hello, MRB_ARGS_NONE());
    mrb_define_class_method(mrb, mrmagick, "hi", mrb_mrmagick_hi, MRB_ARGS_NONE());
    mrb_define_class_method(mrb, mrmagick, "scale", mrb_mrmagick_scale, MRB_ARGS_REQ(3));
    mrb_define_class_method(mrb, mrmagick, "blur", mrb_mrmagick_blur, MRB_ARGS_REQ(4));
    mrb_define_class_method(mrb, mrmagick, "rm", mrb_mrmagick_rm, MRB_ARGS_REQ(1));
    mrb_define_class_method(mrb, mrmagick, "uuid", mrb_mrmagick_uuid, MRB_ARGS_NONE());
    DONE;
}

void mrb_mruby_mrmagick_gem_final(mrb_state *mrb)
{
}
