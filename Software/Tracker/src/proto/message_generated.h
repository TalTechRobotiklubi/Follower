// automatically generated by the FlatBuffers compiler, do not modify

#ifndef FLATBUFFERS_GENERATED_MESSAGE_PROTO_H_
#define FLATBUFFERS_GENERATED_MESSAGE_PROTO_H_

#include "flatbuffers/flatbuffers.h"

namespace proto {

struct LuaMainScript;

struct StatusMessage;

struct Message;

enum MessageType {
  MessageType_LuaMainScript = 0,
  MessageType_MIN = MessageType_LuaMainScript,
  MessageType_MAX = MessageType_LuaMainScript
};

inline const char **EnumNamesMessageType() {
  static const char *names[] = { "LuaMainScript", nullptr };
  return names;
}

inline const char *EnumNameMessageType(MessageType e) { return EnumNamesMessageType()[static_cast<int>(e)]; }

enum Payload {
  Payload_NONE = 0,
  Payload_LuaMainScript = 1,
  Payload_StatusMessage = 2,
  Payload_MIN = Payload_NONE,
  Payload_MAX = Payload_StatusMessage
};

inline const char **EnumNamesPayload() {
  static const char *names[] = { "NONE", "LuaMainScript", "StatusMessage", nullptr };
  return names;
}

inline const char *EnumNamePayload(Payload e) { return EnumNamesPayload()[static_cast<int>(e)]; }

inline bool VerifyPayload(flatbuffers::Verifier &verifier, const void *union_obj, Payload type);

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
