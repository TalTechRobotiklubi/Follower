#include "Decode.h"
#include <libyuv.h>
#include <stdio.h>
#include <stdlib.h>
#include <vpx/vp8dx.h>
#include <vpx/vpx_decoder.h>

struct Decoder {
  vpx_codec_dec_cfg_t decoderCfg;
  vpx_codec_ctx_t codec;
  RgbaImage unscaled;
  RgbaImage scaledOutput;
};

Decoder* DecoderCreate() {
  Decoder* dc = (Decoder*)calloc(1, sizeof(Decoder));

  vpx_codec_dec_cfg_t* cfg = &dc->decoderCfg;
  cfg->threads = 2;

  vpx_codec_flags_t flags = 0;
  vpx_codec_err_t ret =
      vpx_codec_dec_init(&dc->codec, vpx_codec_vp8_dx(), cfg, flags);

  if (ret != VPX_CODEC_OK) {
    printf("Failed to initialize decoder codec");
  }

  return dc;
}

bool DecodeFrame(Decoder* dc, const uint8_t* data, int len, int targetWidth,
                 int targetHeight, RgbaImage* out) {
  vpx_codec_ctx_t* codec = &dc->codec;
  vpx_codec_err_t ret = vpx_codec_decode(codec, data, len, nullptr, 0);

  if (ret != VPX_CODEC_OK) {
    return false;
  }

  vpx_codec_iter_t iter = nullptr;
  vpx_image_t* image = vpx_codec_get_frame(codec, &iter);
  if (!image) {
    printf("No video frame available\n");
    return false;
  }

  if (image->fmt != VPX_IMG_FMT_I420) {
    printf("Invalid format (none I420)\n");
    return false;
  }

  if (dc->unscaled.width != int(image->r_w) ||
      dc->unscaled.height != int(image->r_h)) {
    if (dc->unscaled.data) {
      free(dc->unscaled.data);
    }

    RgbaImageInit(&dc->unscaled, image->r_w, image->r_h);
  }

  libyuv::I420ToARGB(image->planes[0], image->stride[0], image->planes[1],
                     image->stride[1], image->planes[2], image->stride[2],
                     dc->unscaled.data, dc->unscaled.stride, image->r_w,
                     image->r_h);

  if (targetWidth != int(image->r_w) || targetHeight != int(image->r_h)) {
    if (dc->scaledOutput.width != targetWidth ||
        dc->scaledOutput.height != targetHeight) {
      if (dc->scaledOutput.data) {
        free(dc->scaledOutput.data);
      }

      RgbaImageInit(&dc->scaledOutput, targetWidth, targetHeight);

      const RgbaImage* src = &dc->unscaled;
      RgbaImage* dst = &dc->scaledOutput;
      libyuv::ARGBScale(src->data, src->stride, src->width, src->height,
                        dst->data, dst->stride, dst->width, dst->height,
                        libyuv::kFilterBilinear);

      *out = dc->scaledOutput;
    }
  }

  *out = dc->unscaled;

  return true;
}
