#ifndef IMAGE_H
#define IMAGE_H

struct image {
  int width;
  int height;
  int len;
  int pitch;
  int bytes;
  float* data;
};

struct image_region {
  int x;
  int y;
  int width;
  int height;
};

void image_init(image* img, int w, int h);
void image_destroy(image* img);
void image_clear(image* img, float v);
// maps each image value from [a, b] to [c, d]
void image_map_values(image* img, float a, float b, float c, float d);
void copy_sub_image(const image* src, const image_region* src_reg, image* dst,
                    const image_region* dst_reg);
void copy_sub_image_scale(const image* src, const image_region* src_reg,
                              image* dst, const image_region* dst_reg);

#endif
