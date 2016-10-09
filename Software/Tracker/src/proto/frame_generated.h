// automatically generated by the FlatBuffers compiler, do not modify

#ifndef FLATBUFFERS_GENERATED_FRAME_PROTO_H_
#define FLATBUFFERS_GENERATED_FRAME_PROTO_H_

#include "flatbuffers/flatbuffers.h"

namespace proto {

struct Detection;

struct Frame;

MANUALLY_ALIGNED_STRUCT(4) Detection FLATBUFFERS_FINAL_CLASS {
 private:
  int32_t x_;
  int32_t y_;
  int32_t width_;
  int32_t height_;
  float weight_;

 public:
  Detection() { memset(this, 0, sizeof(Detection)); }
  Detection(const Detection &_o) { memcpy(this, &_o, sizeof(Detection)); }
  Detection(int32_t _x, int32_t _y, int32_t _width, int32_t _height, float _weight)
    : x_(flatbuffers::EndianScalar(_x)), y_(flatbuffers::EndianScalar(_y)), width_(flatbuffers::EndianScalar(_width)), height_(flatbuffers::EndianScalar(_height)), weight_(flatbuffers::EndianScalar(_weight)) { }

  int32_t x() const { return flatbuffers::EndianScalar(x_); }
  int32_t y() const { return flatbuffers::EndianScalar(y_); }
  int32_t width() const { return flatbuffers::EndianScalar(width_); }
  int32_t height() const { return flatbuffers::EndianScalar(height_); }
  float weight() const { return flatbuffers::EndianScalar(weight_); }
};
STRUCT_END(Detection, 20);

struct Frame FLATBUFFERS_FINAL_CLASS : private flatbuffers::Table {
  enum {
    VT_TIMESTAMP = 4,
    VT_DEPTH = 6,
    VT_DETECTIONS = 8
  };
  double timestamp() const { return GetField<double>(VT_TIMESTAMP, 0.0); }
  const flatbuffers::Vector<uint8_t> *depth() const { return GetPointer<const flatbuffers::Vector<uint8_t> *>(VT_DEPTH); }
  const flatbuffers::Vector<const Detection *> *detections() const { return GetPointer<const flatbuffers::Vector<const Detection *> *>(VT_DETECTIONS); }
  bool Verify(flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyField<double>(verifier, VT_TIMESTAMP) &&
           VerifyField<flatbuffers::uoffset_t>(verifier, VT_DEPTH) &&
           verifier.Verify(depth()) &&
           VerifyField<flatbuffers::uoffset_t>(verifier, VT_DETECTIONS) &&
           verifier.Verify(detections()) &&
           verifier.EndTable();
  }
};

struct FrameBuilder {
  flatbuffers::FlatBufferBuilder &fbb_;
  flatbuffers::uoffset_t start_;
  void add_timestamp(double timestamp) { fbb_.AddElement<double>(Frame::VT_TIMESTAMP, timestamp, 0.0); }
  void add_depth(flatbuffers::Offset<flatbuffers::Vector<uint8_t>> depth) { fbb_.AddOffset(Frame::VT_DEPTH, depth); }
  void add_detections(flatbuffers::Offset<flatbuffers::Vector<const Detection *>> detections) { fbb_.AddOffset(Frame::VT_DETECTIONS, detections); }
  FrameBuilder(flatbuffers::FlatBufferBuilder &_fbb) : fbb_(_fbb) { start_ = fbb_.StartTable(); }
  FrameBuilder &operator=(const FrameBuilder &);
  flatbuffers::Offset<Frame> Finish() {
    auto o = flatbuffers::Offset<Frame>(fbb_.EndTable(start_, 3));
    return o;
  }
};

inline flatbuffers::Offset<Frame> CreateFrame(flatbuffers::FlatBufferBuilder &_fbb,
    double timestamp = 0.0,
    flatbuffers::Offset<flatbuffers::Vector<uint8_t>> depth = 0,
    flatbuffers::Offset<flatbuffers::Vector<const Detection *>> detections = 0) {
  FrameBuilder builder_(_fbb);
  builder_.add_timestamp(timestamp);
  builder_.add_detections(detections);
  builder_.add_depth(depth);
  return builder_.Finish();
}

inline flatbuffers::Offset<Frame> CreateFrameDirect(flatbuffers::FlatBufferBuilder &_fbb,
    double timestamp = 0.0,
    const std::vector<uint8_t> *depth = nullptr,
    const std::vector<const Detection *> *detections = nullptr) {
  return CreateFrame(_fbb, timestamp, depth ? _fbb.CreateVector<uint8_t>(*depth) : 0, detections ? _fbb.CreateVector<const Detection *>(*detections) : 0);
}

inline const proto::Frame *GetFrame(const void *buf) { return flatbuffers::GetRoot<proto::Frame>(buf); }

inline bool VerifyFrameBuffer(flatbuffers::Verifier &verifier) { return verifier.VerifyBuffer<proto::Frame>(nullptr); }

inline void FinishFrameBuffer(flatbuffers::FlatBufferBuilder &fbb, flatbuffers::Offset<proto::Frame> root) { fbb.Finish(root); }

}  // namespace proto

#endif  // FLATBUFFERS_GENERATED_FRAME_PROTO_H_
