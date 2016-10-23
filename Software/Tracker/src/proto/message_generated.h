// automatically generated by the FlatBuffers compiler, do not modify

#ifndef FLATBUFFERS_GENERATED_MESSAGE_PROTO_H_
#define FLATBUFFERS_GENERATED_MESSAGE_PROTO_H_

#include "flatbuffers/flatbuffers.h"

namespace proto {

struct Vec2;

struct Vec3;

struct Target;

struct Detection;

struct Frame;

struct LuaMainScript;

struct StatusMessage;

struct Message;

enum Payload {
  Payload_NONE = 0,
  Payload_Frame = 1,
  Payload_LuaMainScript = 2,
  Payload_StatusMessage = 3,
  Payload_MIN = Payload_NONE,
  Payload_MAX = Payload_StatusMessage
};

inline const char **EnumNamesPayload() {
  static const char *names[] = { "NONE", "Frame", "LuaMainScript", "StatusMessage", nullptr };
  return names;
}

inline const char *EnumNamePayload(Payload e) { return EnumNamesPayload()[static_cast<int>(e)]; }

inline bool VerifyPayload(flatbuffers::Verifier &verifier, const void *union_obj, Payload type);

MANUALLY_ALIGNED_STRUCT(4) Vec2 FLATBUFFERS_FINAL_CLASS {
 private:
  float x_;
  float y_;

 public:
  Vec2() { memset(this, 0, sizeof(Vec2)); }
  Vec2(const Vec2 &_o) { memcpy(this, &_o, sizeof(Vec2)); }
  Vec2(float _x, float _y)
    : x_(flatbuffers::EndianScalar(_x)), y_(flatbuffers::EndianScalar(_y)) { }

  float x() const { return flatbuffers::EndianScalar(x_); }
  float y() const { return flatbuffers::EndianScalar(y_); }
};
STRUCT_END(Vec2, 8);

MANUALLY_ALIGNED_STRUCT(4) Vec3 FLATBUFFERS_FINAL_CLASS {
 private:
  float x_;
  float y_;
  float z_;

 public:
  Vec3() { memset(this, 0, sizeof(Vec3)); }
  Vec3(const Vec3 &_o) { memcpy(this, &_o, sizeof(Vec3)); }
  Vec3(float _x, float _y, float _z)
    : x_(flatbuffers::EndianScalar(_x)), y_(flatbuffers::EndianScalar(_y)), z_(flatbuffers::EndianScalar(_z)) { }

  float x() const { return flatbuffers::EndianScalar(x_); }
  float y() const { return flatbuffers::EndianScalar(y_); }
  float z() const { return flatbuffers::EndianScalar(z_); }
};
STRUCT_END(Vec3, 12);

MANUALLY_ALIGNED_STRUCT(4) Target FLATBUFFERS_FINAL_CLASS {
 private:
  float timeToLive_;
  Vec2 position_;
  Vec3 metricPosition_;

 public:
  Target() { memset(this, 0, sizeof(Target)); }
  Target(const Target &_o) { memcpy(this, &_o, sizeof(Target)); }
  Target(float _timeToLive, const Vec2 &_position, const Vec3 &_metricPosition)
    : timeToLive_(flatbuffers::EndianScalar(_timeToLive)), position_(_position), metricPosition_(_metricPosition) { }

  float timeToLive() const { return flatbuffers::EndianScalar(timeToLive_); }
  const Vec2 &position() const { return position_; }
  const Vec3 &metricPosition() const { return metricPosition_; }
};
STRUCT_END(Target, 24);

MANUALLY_ALIGNED_STRUCT(4) Detection FLATBUFFERS_FINAL_CLASS {
 private:
  Vec2 position_;
  Vec3 metricPosition_;
  float weight_;

 public:
  Detection() { memset(this, 0, sizeof(Detection)); }
  Detection(const Detection &_o) { memcpy(this, &_o, sizeof(Detection)); }
  Detection(const Vec2 &_position, const Vec3 &_metricPosition, float _weight)
    : position_(_position), metricPosition_(_metricPosition), weight_(flatbuffers::EndianScalar(_weight)) { }

  const Vec2 &position() const { return position_; }
  const Vec3 &metricPosition() const { return metricPosition_; }
  float weight() const { return flatbuffers::EndianScalar(weight_); }
};
STRUCT_END(Detection, 24);

struct Frame FLATBUFFERS_FINAL_CLASS : private flatbuffers::Table {
  enum {
    VT_TIMESTAMP = 4,
    VT_CAMERA = 6,
    VT_DEPTH = 8,
    VT_DETECTIONS = 10,
    VT_TARGETS = 12
  };
  double timestamp() const { return GetField<double>(VT_TIMESTAMP, 0.0); }
  const Vec2 *camera() const { return GetStruct<const Vec2 *>(VT_CAMERA); }
  const flatbuffers::Vector<uint8_t> *depth() const { return GetPointer<const flatbuffers::Vector<uint8_t> *>(VT_DEPTH); }
  const flatbuffers::Vector<const Detection *> *detections() const { return GetPointer<const flatbuffers::Vector<const Detection *> *>(VT_DETECTIONS); }
  const flatbuffers::Vector<const Target *> *targets() const { return GetPointer<const flatbuffers::Vector<const Target *> *>(VT_TARGETS); }
  bool Verify(flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyField<double>(verifier, VT_TIMESTAMP) &&
           VerifyField<Vec2>(verifier, VT_CAMERA) &&
           VerifyField<flatbuffers::uoffset_t>(verifier, VT_DEPTH) &&
           verifier.Verify(depth()) &&
           VerifyField<flatbuffers::uoffset_t>(verifier, VT_DETECTIONS) &&
           verifier.Verify(detections()) &&
           VerifyField<flatbuffers::uoffset_t>(verifier, VT_TARGETS) &&
           verifier.Verify(targets()) &&
           verifier.EndTable();
  }
};

struct FrameBuilder {
  flatbuffers::FlatBufferBuilder &fbb_;
  flatbuffers::uoffset_t start_;
  void add_timestamp(double timestamp) { fbb_.AddElement<double>(Frame::VT_TIMESTAMP, timestamp, 0.0); }
  void add_camera(const Vec2 *camera) { fbb_.AddStruct(Frame::VT_CAMERA, camera); }
  void add_depth(flatbuffers::Offset<flatbuffers::Vector<uint8_t>> depth) { fbb_.AddOffset(Frame::VT_DEPTH, depth); }
  void add_detections(flatbuffers::Offset<flatbuffers::Vector<const Detection *>> detections) { fbb_.AddOffset(Frame::VT_DETECTIONS, detections); }
  void add_targets(flatbuffers::Offset<flatbuffers::Vector<const Target *>> targets) { fbb_.AddOffset(Frame::VT_TARGETS, targets); }
  FrameBuilder(flatbuffers::FlatBufferBuilder &_fbb) : fbb_(_fbb) { start_ = fbb_.StartTable(); }
  FrameBuilder &operator=(const FrameBuilder &);
  flatbuffers::Offset<Frame> Finish() {
    auto o = flatbuffers::Offset<Frame>(fbb_.EndTable(start_, 5));
    return o;
  }
};

inline flatbuffers::Offset<Frame> CreateFrame(flatbuffers::FlatBufferBuilder &_fbb,
    double timestamp = 0.0,
    const Vec2 *camera = 0,
    flatbuffers::Offset<flatbuffers::Vector<uint8_t>> depth = 0,
    flatbuffers::Offset<flatbuffers::Vector<const Detection *>> detections = 0,
    flatbuffers::Offset<flatbuffers::Vector<const Target *>> targets = 0) {
  FrameBuilder builder_(_fbb);
  builder_.add_timestamp(timestamp);
  builder_.add_targets(targets);
  builder_.add_detections(detections);
  builder_.add_depth(depth);
  builder_.add_camera(camera);
  return builder_.Finish();
}

inline flatbuffers::Offset<Frame> CreateFrameDirect(flatbuffers::FlatBufferBuilder &_fbb,
    double timestamp = 0.0,
    const Vec2 *camera = 0,
    const std::vector<uint8_t> *depth = nullptr,
    const std::vector<const Detection *> *detections = nullptr,
    const std::vector<const Target *> *targets = nullptr) {
  return CreateFrame(_fbb, timestamp, camera, depth ? _fbb.CreateVector<uint8_t>(*depth) : 0, detections ? _fbb.CreateVector<const Detection *>(*detections) : 0, targets ? _fbb.CreateVector<const Target *>(*targets) : 0);
}

struct LuaMainScript FLATBUFFERS_FINAL_CLASS : private flatbuffers::Table {
  enum {
    VT_CONTENT = 4
  };
  const flatbuffers::String *content() const { return GetPointer<const flatbuffers::String *>(VT_CONTENT); }
  bool Verify(flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyField<flatbuffers::uoffset_t>(verifier, VT_CONTENT) &&
           verifier.Verify(content()) &&
           verifier.EndTable();
  }
};

struct LuaMainScriptBuilder {
  flatbuffers::FlatBufferBuilder &fbb_;
  flatbuffers::uoffset_t start_;
  void add_content(flatbuffers::Offset<flatbuffers::String> content) { fbb_.AddOffset(LuaMainScript::VT_CONTENT, content); }
  LuaMainScriptBuilder(flatbuffers::FlatBufferBuilder &_fbb) : fbb_(_fbb) { start_ = fbb_.StartTable(); }
  LuaMainScriptBuilder &operator=(const LuaMainScriptBuilder &);
  flatbuffers::Offset<LuaMainScript> Finish() {
    auto o = flatbuffers::Offset<LuaMainScript>(fbb_.EndTable(start_, 1));
    return o;
  }
};

inline flatbuffers::Offset<LuaMainScript> CreateLuaMainScript(flatbuffers::FlatBufferBuilder &_fbb,
    flatbuffers::Offset<flatbuffers::String> content = 0) {
  LuaMainScriptBuilder builder_(_fbb);
  builder_.add_content(content);
  return builder_.Finish();
}

inline flatbuffers::Offset<LuaMainScript> CreateLuaMainScriptDirect(flatbuffers::FlatBufferBuilder &_fbb,
    const char *content = nullptr) {
  return CreateLuaMainScript(_fbb, content ? _fbb.CreateString(content) : 0);
}

struct StatusMessage FLATBUFFERS_FINAL_CLASS : private flatbuffers::Table {
  enum {
    VT_MESSAGE = 4
  };
  const flatbuffers::String *message() const { return GetPointer<const flatbuffers::String *>(VT_MESSAGE); }
  bool Verify(flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyField<flatbuffers::uoffset_t>(verifier, VT_MESSAGE) &&
           verifier.Verify(message()) &&
           verifier.EndTable();
  }
};

struct StatusMessageBuilder {
  flatbuffers::FlatBufferBuilder &fbb_;
  flatbuffers::uoffset_t start_;
  void add_message(flatbuffers::Offset<flatbuffers::String> message) { fbb_.AddOffset(StatusMessage::VT_MESSAGE, message); }
  StatusMessageBuilder(flatbuffers::FlatBufferBuilder &_fbb) : fbb_(_fbb) { start_ = fbb_.StartTable(); }
  StatusMessageBuilder &operator=(const StatusMessageBuilder &);
  flatbuffers::Offset<StatusMessage> Finish() {
    auto o = flatbuffers::Offset<StatusMessage>(fbb_.EndTable(start_, 1));
    return o;
  }
};

inline flatbuffers::Offset<StatusMessage> CreateStatusMessage(flatbuffers::FlatBufferBuilder &_fbb,
    flatbuffers::Offset<flatbuffers::String> message = 0) {
  StatusMessageBuilder builder_(_fbb);
  builder_.add_message(message);
  return builder_.Finish();
}

inline flatbuffers::Offset<StatusMessage> CreateStatusMessageDirect(flatbuffers::FlatBufferBuilder &_fbb,
    const char *message = nullptr) {
  return CreateStatusMessage(_fbb, message ? _fbb.CreateString(message) : 0);
}

struct Message FLATBUFFERS_FINAL_CLASS : private flatbuffers::Table {
  enum {
    VT_PAYLOAD_TYPE = 4,
    VT_PAYLOAD = 6
  };
  Payload payload_type() const { return static_cast<Payload>(GetField<uint8_t>(VT_PAYLOAD_TYPE, 0)); }
  const void *payload() const { return GetPointer<const void *>(VT_PAYLOAD); }
  bool Verify(flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyField<uint8_t>(verifier, VT_PAYLOAD_TYPE) &&
           VerifyField<flatbuffers::uoffset_t>(verifier, VT_PAYLOAD) &&
           VerifyPayload(verifier, payload(), payload_type()) &&
           verifier.EndTable();
  }
};

struct MessageBuilder {
  flatbuffers::FlatBufferBuilder &fbb_;
  flatbuffers::uoffset_t start_;
  void add_payload_type(Payload payload_type) { fbb_.AddElement<uint8_t>(Message::VT_PAYLOAD_TYPE, static_cast<uint8_t>(payload_type), 0); }
  void add_payload(flatbuffers::Offset<void> payload) { fbb_.AddOffset(Message::VT_PAYLOAD, payload); }
  MessageBuilder(flatbuffers::FlatBufferBuilder &_fbb) : fbb_(_fbb) { start_ = fbb_.StartTable(); }
  MessageBuilder &operator=(const MessageBuilder &);
  flatbuffers::Offset<Message> Finish() {
    auto o = flatbuffers::Offset<Message>(fbb_.EndTable(start_, 2));
    return o;
  }
};

inline flatbuffers::Offset<Message> CreateMessage(flatbuffers::FlatBufferBuilder &_fbb,
    Payload payload_type = Payload_NONE,
    flatbuffers::Offset<void> payload = 0) {
  MessageBuilder builder_(_fbb);
  builder_.add_payload(payload);
  builder_.add_payload_type(payload_type);
  return builder_.Finish();
}

inline bool VerifyPayload(flatbuffers::Verifier &verifier, const void *union_obj, Payload type) {
  switch (type) {
    case Payload_NONE: return true;
    case Payload_Frame: return verifier.VerifyTable(reinterpret_cast<const Frame *>(union_obj));
    case Payload_LuaMainScript: return verifier.VerifyTable(reinterpret_cast<const LuaMainScript *>(union_obj));
    case Payload_StatusMessage: return verifier.VerifyTable(reinterpret_cast<const StatusMessage *>(union_obj));
    default: return false;
  }
}

inline const proto::Message *GetMessage(const void *buf) { return flatbuffers::GetRoot<proto::Message>(buf); }

inline bool VerifyMessageBuffer(flatbuffers::Verifier &verifier) { return verifier.VerifyBuffer<proto::Message>(nullptr); }

inline void FinishMessageBuffer(flatbuffers::FlatBufferBuilder &fbb, flatbuffers::Offset<proto::Message> root) { fbb.Finish(root); }

}  // namespace proto

#endif  // FLATBUFFERS_GENERATED_MESSAGE_PROTO_H_