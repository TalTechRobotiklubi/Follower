#include "Decode.h"
#include <libyuv.h>
#include <stdio.h>
#include <stdlib.h>
#include <vpx/vp8dx.h>
#include <vpx/vpx_decoder.h>

struct Decoder {
  vpx_codec_dec_cfg_t decoderCfg;
  vpx_codec_ctx_t codec;
  rgba_image unscaled;
  rgba_image scaledOutput;
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
                 int targetHeight, rgba_image* out) {
  vpx_codec_ctx_t* codec = &dc->codec;
  vpx_codec_err_t ret = vpx_codec_decode(codec, data, len, NULL, 0);

  if (ret != VPX_CODEC_OK) {
#if 0
    const char* error = vpx_codec_error(codec);
    const char* detail = vpx_codec_error_detail(codec);
    printf("Failed to decode, %s: %s\n", (error ? error : "unknown"),
           (detail ? detail : "unknown"));
#endif
    return false;
  }

  vpx_codec_iter_t iter = NULL;
  vpx_image_t* image = vpx_codec_get_frame(codec, &iter);
  if (!image) {
    printf("No video frame available\n");
    return false;
  }

  if (image->fmt != VPX_IMG_FMT_I420) {
    printf("Invalid format (none I420)\n");
    return false;
  }

  if (dc->unscaled.width != image->r_w || dc->unscaled.height != image->r_h) {
    if (dc->unscaled.data) {
      free(dc->unscaled.data);
    }

    rgba_image_init(&dc->unscaled, image->r_w, image->r_h);
  }

  libyuv::I420ToARGB(image->planes[0], image->stride[0], image->planes[1],
                     image->stride[1], image->planes[2], image->stride[2],
                     dc->unscaled.data, dc->unscaled.pitch, image->r_w,
                     image->r_h);

  if (targetWidth != image->r_w || targetHeight != image->r_h) {
    if (dc->scaledOutput.width != targetWidth ||
        dc->scaledOutput.height != targetHeight) {
      if (dc->scaledOutput.data) {
        free(dc->scaledOutput.data);
      }

      rgba_image_init(&dc->scaledOutput, targetWidth, targetHeight);

      const rgba_image* src = &dc->unscaled;
      rgba_image* dst = &dc->scaledOutput;
      libyuv::ARGBScale(src->data, src->pitch, src->width, src->height,
                        dst->data, dst->pitch, dst->width, dst->height,
                        libyuv::kFilterBilinear);

      *out = dc->scaledOutput;
    }
  }

  *out = dc->unscaled;

  return true;
}
