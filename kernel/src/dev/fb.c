// Copyright (C) 2024 Sipaa Projects
// This code is part of the Soaplin kernel and is licensed under the terms of
// the MIT License.

#include <dev/fb.h>
#include <limine.h>

framebuffer *__framebuf;

framebuffer *fb_get() {
  // We are lazy asses
  // What lazy asses do? They use the same structure,
  // with some useless end fields removed, and they casts
  // the original structure to the new!
  return __framebuf;
}
