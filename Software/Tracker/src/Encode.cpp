#include "Encode.h"
#include <assert.h>
#include <libyuv/convert.h>
#include <stdio.h>
#include <stdlib.h>
#include <vpx/vp8cx.h>
#include <vpx/vpx_codec.h>
#include <vpx/vpx_encoder.h>
#include "BlockDiff.h"
#include "Clock.h"

struct Encoder {
  int frameCount;
  int frameWidth;
  int frameHeight;
  int dstWidth;
  int dstHeight;
  double tsBegin;
  double tsMs;
  vpx_codec_iface_t* codecIface;
  vpx_codec_enc_cfg encoderCfg;
  vpx_codec_ctx_t codec;
  vpx_image_t rawImage;
};

Encoder* EncoderCreate(int w, int h) {
  Encoder* e = (Encoder*)calloc(1, sizeof(Encoder));
  e->frameCount = 1;
  e->frameWidth = w;
  e->frameHeight = h;
  e->dstWidth = e->frameWidth;
  e->dstHeight = e->frameHeight;
  e->tsBegin = ms_now();
  e->tsMs = ms_now();

  vpx_codec_iface_t* codecIface = vpx_codec_vp8_cx();
  vpx_codec_err_t confRes =
      vpx_codec_enc_config_default(codecIface, &e->encoderCfg, 0);

  if (confRes == VPX_CODEC_OK) {
    printf("%s\n", vpx_codec_iface_name(codecIface));
    e->encoderCfg.g_threads = 2;
    e->encoderCfg.g_lag_in_frames = 0;
    e->encoderCfg.rc_target_bitrate = 400;
    e->encoderCfg.g_w = e->dstWidth;
    e->encoderCfg.g_h = e->dstHeight;
    e->encoderCfg.g_timebase.num = 1000;
    e->encoderCfg.g_timebase.den = 30000;
    e->encoderCfg.rc_min_quantizer = 20;
    e->encoderCfg.rc_max_quantizer = 30;
    e->encoderCfg.g_pass = VPX_RC_ONE_PASS;
    e->codecIface = codecIface;
    e->encoderCfg.g_profile = 2;
  }

  printf("g_threads: %d\n", e->encoderCfg.g_threads);

  // Image width, height
  printf("g_w: %d, g_h: %d\n", e->encoderCfg.g_w, e->encoderCfg.g_h);
  printf("bit depth: %d\n", e->encoderCfg.g_bit_depth);
  printf("input bit depth: %d\n", e->encoderCfg.g_input_bit_depth);
  printf("multipass encoding: %d\n", e->encoderCfg.g_pass);
  printf("lag in frames: %d\n", e->encoderCfg.g_lag_in_frames);
  printf("rate control framedrop thresh: %d\n",
         e->encoderCfg.rc_dropframe_thresh);
  printf("rate control algo: %d\n", e->encoderCfg.rc_end_usage);
  printf("rate control target bitrate: %d kbit/s\n",
         e->encoderCfg.rc_target_bitrate);
  printf("kf control: %d\n", e->encoderCfg.kf_mode);
  printf("min quant: %d, max quant: %d\n", e->encoderCfg.rc_min_quantizer,
         e->encoderCfg.rc_max_quantizer);
  printf("g_profile: %d\n", e->encoderCfg.g_profile);

  if (vpx_codec_enc_init(&e->codec, e->codecIface, &e->encoderCfg, 0)) {
    printf("Failed to initialize codec\n");
  }

  vpx_codec_control(&e->codec, VP8E_SET_NOISE_SENSITIVITY, 0);
  vpx_codec_control(&e->codec, VP8E_SET_CPUUSED, 16);

  if (!vpx_img_alloc(&e->rawImage, VPX_IMG_FMT_I420, e->frameWidth,
                     e->frameHeight, 1)) {
    printf("Failed to allocate image\n");
  }

  return e;
}

IoVec EncodeImage(Encoder* encoder, const uint8_t* raw, const ActiveMap* map) {
  vpx_image_t* img = &encoder->rawImage;

  libyuv::ARGBToI420(raw, encoder->frameWidth * 4, img->planes[VPX_PLANE_Y],
                     img->stride[VPX_PLANE_Y], img->planes[VPX_PLANE_U],
                     img->stride[VPX_PLANE_U], img->planes[VPX_PLANE_V],
                     img->stride[VPX_PLANE_V], encoder->frameWidth,
                     encoder->frameHeight);

  vpx_active_map_t amap;
  amap.rows = map->height;
  amap.cols = map->width;
  amap.active_map = map->buf.data;

  if (vpx_codec_control(&encoder->codec, VP8E_SET_ACTIVEMAP, &amap)) {
    printf("encode: set active map failed\n");
  }

  const vpx_codec_err_t res =
      vpx_codec_encode(&encoder->codec, &encoder->rawImage,
                       int64_t((encoder->tsMs - encoder->tsBegin) / 1000.0), 1, 0,
                       VPX_DL_REALTIME);
  if (res != VPX_CODEC_OK) {
    printf("Failed to encode frame\n");
    printf("%s\n", vpx_codec_err_to_string(res));
    printf("%s\n", vpx_codec_error_detail(&encoder->codec));
  }

  const vpx_codec_cx_pkt_t* pkt = NULL;
  vpx_codec_iter_t iter = NULL;

  IoVec result;

  while ((pkt = vpx_codec_get_cx_data(&encoder->codec, &iter))) {
    if (pkt->kind == VPX_CODEC_CX_FRAME_PKT) {
      result.data = (uint8_t*)pkt->data.frame.buf;
      result.len = int(pkt->data.frame.sz);
      printf("%.4f kB\n", double(result.len) / 1000.0);
      break;
    }
  }

  encoder->frameCount += 1;
  encoder->tsMs = ms_now();

  return result;
}
