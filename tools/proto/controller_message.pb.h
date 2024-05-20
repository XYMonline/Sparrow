// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: controller_message.proto

#ifndef GOOGLE_PROTOBUF_INCLUDED_controller_5fmessage_2eproto
#define GOOGLE_PROTOBUF_INCLUDED_controller_5fmessage_2eproto

#include <limits>
#include <string>

#include <google/protobuf/port_def.inc>
#if PROTOBUF_VERSION < 3021000
#error This file was generated by a newer version of protoc which is
#error incompatible with your Protocol Buffer headers. Please update
#error your headers.
#endif
#if 3021012 < PROTOBUF_MIN_PROTOC_VERSION
#error This file was generated by an older version of protoc which is
#error incompatible with your Protocol Buffer headers. Please
#error regenerate this file with a newer version of protoc.
#endif

#include <google/protobuf/port_undef.inc>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/arena.h>
#include <google/protobuf/arenastring.h>
#include <google/protobuf/generated_message_util.h>
#include <google/protobuf/metadata_lite.h>
#include <google/protobuf/generated_message_reflection.h>
#include <google/protobuf/message.h>
#include <google/protobuf/repeated_field.h>  // IWYU pragma: export
#include <google/protobuf/extension_set.h>  // IWYU pragma: export
#include <google/protobuf/generated_enum_reflection.h>
#include <google/protobuf/unknown_field_set.h>
// @@protoc_insertion_point(includes)
#include <google/protobuf/port_def.inc>
#define PROTOBUF_INTERNAL_EXPORT_controller_5fmessage_2eproto
PROTOBUF_NAMESPACE_OPEN
namespace internal {
class AnyMetadata;
}  // namespace internal
PROTOBUF_NAMESPACE_CLOSE

// Internal implementation detail -- do not use these members.
struct TableStruct_controller_5fmessage_2eproto {
  static const uint32_t offsets[];
};
extern const ::PROTOBUF_NAMESPACE_ID::internal::DescriptorTable descriptor_table_controller_5fmessage_2eproto;
namespace message_type {
class route_controller;
struct route_controllerDefaultTypeInternal;
extern route_controllerDefaultTypeInternal _route_controller_default_instance_;
class srever_info;
struct srever_infoDefaultTypeInternal;
extern srever_infoDefaultTypeInternal _srever_info_default_instance_;
}  // namespace message_type
PROTOBUF_NAMESPACE_OPEN
template<> ::message_type::route_controller* Arena::CreateMaybeMessage<::message_type::route_controller>(Arena*);
template<> ::message_type::srever_info* Arena::CreateMaybeMessage<::message_type::srever_info>(Arena*);
PROTOBUF_NAMESPACE_CLOSE
namespace message_type {

enum message_category : int {
  SERVER_LIST = 0,
  FORCE_OFFLINE = 1,
  message_category_INT_MIN_SENTINEL_DO_NOT_USE_ = std::numeric_limits<int32_t>::min(),
  message_category_INT_MAX_SENTINEL_DO_NOT_USE_ = std::numeric_limits<int32_t>::max()
};
bool message_category_IsValid(int value);
constexpr message_category message_category_MIN = SERVER_LIST;
constexpr message_category message_category_MAX = FORCE_OFFLINE;
constexpr int message_category_ARRAYSIZE = message_category_MAX + 1;

const ::PROTOBUF_NAMESPACE_ID::EnumDescriptor* message_category_descriptor();
template<typename T>
inline const std::string& message_category_Name(T enum_t_value) {
  static_assert(::std::is_same<T, message_category>::value ||
    ::std::is_integral<T>::value,
    "Incorrect type passed to function message_category_Name.");
  return ::PROTOBUF_NAMESPACE_ID::internal::NameOfEnum(
    message_category_descriptor(), enum_t_value);
}
inline bool message_category_Parse(
    ::PROTOBUF_NAMESPACE_ID::ConstStringParam name, message_category* value) {
  return ::PROTOBUF_NAMESPACE_ID::internal::ParseNamedEnum<message_category>(
    message_category_descriptor(), name, value);
}
enum server_category : int {
  AUTH = 0,
  ROUTE = 1,
  BUSINESS = 2,
  server_category_INT_MIN_SENTINEL_DO_NOT_USE_ = std::numeric_limits<int32_t>::min(),
  server_category_INT_MAX_SENTINEL_DO_NOT_USE_ = std::numeric_limits<int32_t>::max()
};
bool server_category_IsValid(int value);
constexpr server_category server_category_MIN = AUTH;
constexpr server_category server_category_MAX = BUSINESS;
constexpr int server_category_ARRAYSIZE = server_category_MAX + 1;

const ::PROTOBUF_NAMESPACE_ID::EnumDescriptor* server_category_descriptor();
template<typename T>
inline const std::string& server_category_Name(T enum_t_value) {
  static_assert(::std::is_same<T, server_category>::value ||
    ::std::is_integral<T>::value,
    "Incorrect type passed to function server_category_Name.");
  return ::PROTOBUF_NAMESPACE_ID::internal::NameOfEnum(
    server_category_descriptor(), enum_t_value);
}
inline bool server_category_Parse(
    ::PROTOBUF_NAMESPACE_ID::ConstStringParam name, server_category* value) {
  return ::PROTOBUF_NAMESPACE_ID::internal::ParseNamedEnum<server_category>(
    server_category_descriptor(), name, value);
}
// ===================================================================

class srever_info final :
    public ::PROTOBUF_NAMESPACE_ID::Message /* @@protoc_insertion_point(class_definition:message_type.srever_info) */ {
 public:
  inline srever_info() : srever_info(nullptr) {}
  ~srever_info() override;
  explicit PROTOBUF_CONSTEXPR srever_info(::PROTOBUF_NAMESPACE_ID::internal::ConstantInitialized);

  srever_info(const srever_info& from);
  srever_info(srever_info&& from) noexcept
    : srever_info() {
    *this = ::std::move(from);
  }

  inline srever_info& operator=(const srever_info& from) {
    CopyFrom(from);
    return *this;
  }
  inline srever_info& operator=(srever_info&& from) noexcept {
    if (this == &from) return *this;
    if (GetOwningArena() == from.GetOwningArena()
  #ifdef PROTOBUF_FORCE_COPY_IN_MOVE
        && GetOwningArena() != nullptr
  #endif  // !PROTOBUF_FORCE_COPY_IN_MOVE
    ) {
      InternalSwap(&from);
    } else {
      CopyFrom(from);
    }
    return *this;
  }

  static const ::PROTOBUF_NAMESPACE_ID::Descriptor* descriptor() {
    return GetDescriptor();
  }
  static const ::PROTOBUF_NAMESPACE_ID::Descriptor* GetDescriptor() {
    return default_instance().GetMetadata().descriptor;
  }
  static const ::PROTOBUF_NAMESPACE_ID::Reflection* GetReflection() {
    return default_instance().GetMetadata().reflection;
  }
  static const srever_info& default_instance() {
    return *internal_default_instance();
  }
  static inline const srever_info* internal_default_instance() {
    return reinterpret_cast<const srever_info*>(
               &_srever_info_default_instance_);
  }
  static constexpr int kIndexInFileMessages =
    0;

  friend void swap(srever_info& a, srever_info& b) {
    a.Swap(&b);
  }
  inline void Swap(srever_info* other) {
    if (other == this) return;
  #ifdef PROTOBUF_FORCE_COPY_IN_SWAP
    if (GetOwningArena() != nullptr &&
        GetOwningArena() == other->GetOwningArena()) {
   #else  // PROTOBUF_FORCE_COPY_IN_SWAP
    if (GetOwningArena() == other->GetOwningArena()) {
  #endif  // !PROTOBUF_FORCE_COPY_IN_SWAP
      InternalSwap(other);
    } else {
      ::PROTOBUF_NAMESPACE_ID::internal::GenericSwap(this, other);
    }
  }
  void UnsafeArenaSwap(srever_info* other) {
    if (other == this) return;
    GOOGLE_DCHECK(GetOwningArena() == other->GetOwningArena());
    InternalSwap(other);
  }

  // implements Message ----------------------------------------------

  srever_info* New(::PROTOBUF_NAMESPACE_ID::Arena* arena = nullptr) const final {
    return CreateMaybeMessage<srever_info>(arena);
  }
  using ::PROTOBUF_NAMESPACE_ID::Message::CopyFrom;
  void CopyFrom(const srever_info& from);
  using ::PROTOBUF_NAMESPACE_ID::Message::MergeFrom;
  void MergeFrom( const srever_info& from) {
    srever_info::MergeImpl(*this, from);
  }
  private:
  static void MergeImpl(::PROTOBUF_NAMESPACE_ID::Message& to_msg, const ::PROTOBUF_NAMESPACE_ID::Message& from_msg);
  public:
  PROTOBUF_ATTRIBUTE_REINITIALIZES void Clear() final;
  bool IsInitialized() const final;

  size_t ByteSizeLong() const final;
  const char* _InternalParse(const char* ptr, ::PROTOBUF_NAMESPACE_ID::internal::ParseContext* ctx) final;
  uint8_t* _InternalSerialize(
      uint8_t* target, ::PROTOBUF_NAMESPACE_ID::io::EpsCopyOutputStream* stream) const final;
  int GetCachedSize() const final { return _impl_._cached_size_.Get(); }

  private:
  void SharedCtor(::PROTOBUF_NAMESPACE_ID::Arena* arena, bool is_message_owned);
  void SharedDtor();
  void SetCachedSize(int size) const final;
  void InternalSwap(srever_info* other);

  private:
  friend class ::PROTOBUF_NAMESPACE_ID::internal::AnyMetadata;
  static ::PROTOBUF_NAMESPACE_ID::StringPiece FullMessageName() {
    return "message_type.srever_info";
  }
  protected:
  explicit srever_info(::PROTOBUF_NAMESPACE_ID::Arena* arena,
                       bool is_message_owned = false);
  public:

  static const ClassData _class_data_;
  const ::PROTOBUF_NAMESPACE_ID::Message::ClassData*GetClassData() const final;

  ::PROTOBUF_NAMESPACE_ID::Metadata GetMetadata() const final;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  enum : int {
    kUriFieldNumber = 2,
    kMasterUriFieldNumber = 4,
    kCategoryFieldNumber = 1,
    kLoadFieldNumber = 3,
  };
  // string uri = 2;
  void clear_uri();
  const std::string& uri() const;
  template <typename ArgT0 = const std::string&, typename... ArgT>
  void set_uri(ArgT0&& arg0, ArgT... args);
  std::string* mutable_uri();
  PROTOBUF_NODISCARD std::string* release_uri();
  void set_allocated_uri(std::string* uri);
  private:
  const std::string& _internal_uri() const;
  inline PROTOBUF_ALWAYS_INLINE void _internal_set_uri(const std::string& value);
  std::string* _internal_mutable_uri();
  public:

  // string master_uri = 4;
  void clear_master_uri();
  const std::string& master_uri() const;
  template <typename ArgT0 = const std::string&, typename... ArgT>
  void set_master_uri(ArgT0&& arg0, ArgT... args);
  std::string* mutable_master_uri();
  PROTOBUF_NODISCARD std::string* release_master_uri();
  void set_allocated_master_uri(std::string* master_uri);
  private:
  const std::string& _internal_master_uri() const;
  inline PROTOBUF_ALWAYS_INLINE void _internal_set_master_uri(const std::string& value);
  std::string* _internal_mutable_master_uri();
  public:

  // .message_type.server_category category = 1;
  void clear_category();
  ::message_type::server_category category() const;
  void set_category(::message_type::server_category value);
  private:
  ::message_type::server_category _internal_category() const;
  void _internal_set_category(::message_type::server_category value);
  public:

  // int32 load = 3;
  void clear_load();
  int32_t load() const;
  void set_load(int32_t value);
  private:
  int32_t _internal_load() const;
  void _internal_set_load(int32_t value);
  public:

  // @@protoc_insertion_point(class_scope:message_type.srever_info)
 private:
  class _Internal;

  template <typename T> friend class ::PROTOBUF_NAMESPACE_ID::Arena::InternalHelper;
  typedef void InternalArenaConstructable_;
  typedef void DestructorSkippable_;
  struct Impl_ {
    ::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr uri_;
    ::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr master_uri_;
    int category_;
    int32_t load_;
    mutable ::PROTOBUF_NAMESPACE_ID::internal::CachedSize _cached_size_;
  };
  union { Impl_ _impl_; };
  friend struct ::TableStruct_controller_5fmessage_2eproto;
};
// -------------------------------------------------------------------

class route_controller final :
    public ::PROTOBUF_NAMESPACE_ID::Message /* @@protoc_insertion_point(class_definition:message_type.route_controller) */ {
 public:
  inline route_controller() : route_controller(nullptr) {}
  ~route_controller() override;
  explicit PROTOBUF_CONSTEXPR route_controller(::PROTOBUF_NAMESPACE_ID::internal::ConstantInitialized);

  route_controller(const route_controller& from);
  route_controller(route_controller&& from) noexcept
    : route_controller() {
    *this = ::std::move(from);
  }

  inline route_controller& operator=(const route_controller& from) {
    CopyFrom(from);
    return *this;
  }
  inline route_controller& operator=(route_controller&& from) noexcept {
    if (this == &from) return *this;
    if (GetOwningArena() == from.GetOwningArena()
  #ifdef PROTOBUF_FORCE_COPY_IN_MOVE
        && GetOwningArena() != nullptr
  #endif  // !PROTOBUF_FORCE_COPY_IN_MOVE
    ) {
      InternalSwap(&from);
    } else {
      CopyFrom(from);
    }
    return *this;
  }

  static const ::PROTOBUF_NAMESPACE_ID::Descriptor* descriptor() {
    return GetDescriptor();
  }
  static const ::PROTOBUF_NAMESPACE_ID::Descriptor* GetDescriptor() {
    return default_instance().GetMetadata().descriptor;
  }
  static const ::PROTOBUF_NAMESPACE_ID::Reflection* GetReflection() {
    return default_instance().GetMetadata().reflection;
  }
  static const route_controller& default_instance() {
    return *internal_default_instance();
  }
  static inline const route_controller* internal_default_instance() {
    return reinterpret_cast<const route_controller*>(
               &_route_controller_default_instance_);
  }
  static constexpr int kIndexInFileMessages =
    1;

  friend void swap(route_controller& a, route_controller& b) {
    a.Swap(&b);
  }
  inline void Swap(route_controller* other) {
    if (other == this) return;
  #ifdef PROTOBUF_FORCE_COPY_IN_SWAP
    if (GetOwningArena() != nullptr &&
        GetOwningArena() == other->GetOwningArena()) {
   #else  // PROTOBUF_FORCE_COPY_IN_SWAP
    if (GetOwningArena() == other->GetOwningArena()) {
  #endif  // !PROTOBUF_FORCE_COPY_IN_SWAP
      InternalSwap(other);
    } else {
      ::PROTOBUF_NAMESPACE_ID::internal::GenericSwap(this, other);
    }
  }
  void UnsafeArenaSwap(route_controller* other) {
    if (other == this) return;
    GOOGLE_DCHECK(GetOwningArena() == other->GetOwningArena());
    InternalSwap(other);
  }

  // implements Message ----------------------------------------------

  route_controller* New(::PROTOBUF_NAMESPACE_ID::Arena* arena = nullptr) const final {
    return CreateMaybeMessage<route_controller>(arena);
  }
  using ::PROTOBUF_NAMESPACE_ID::Message::CopyFrom;
  void CopyFrom(const route_controller& from);
  using ::PROTOBUF_NAMESPACE_ID::Message::MergeFrom;
  void MergeFrom( const route_controller& from) {
    route_controller::MergeImpl(*this, from);
  }
  private:
  static void MergeImpl(::PROTOBUF_NAMESPACE_ID::Message& to_msg, const ::PROTOBUF_NAMESPACE_ID::Message& from_msg);
  public:
  PROTOBUF_ATTRIBUTE_REINITIALIZES void Clear() final;
  bool IsInitialized() const final;

  size_t ByteSizeLong() const final;
  const char* _InternalParse(const char* ptr, ::PROTOBUF_NAMESPACE_ID::internal::ParseContext* ctx) final;
  uint8_t* _InternalSerialize(
      uint8_t* target, ::PROTOBUF_NAMESPACE_ID::io::EpsCopyOutputStream* stream) const final;
  int GetCachedSize() const final { return _impl_._cached_size_.Get(); }

  private:
  void SharedCtor(::PROTOBUF_NAMESPACE_ID::Arena* arena, bool is_message_owned);
  void SharedDtor();
  void SetCachedSize(int size) const final;
  void InternalSwap(route_controller* other);

  private:
  friend class ::PROTOBUF_NAMESPACE_ID::internal::AnyMetadata;
  static ::PROTOBUF_NAMESPACE_ID::StringPiece FullMessageName() {
    return "message_type.route_controller";
  }
  protected:
  explicit route_controller(::PROTOBUF_NAMESPACE_ID::Arena* arena,
                       bool is_message_owned = false);
  public:

  static const ClassData _class_data_;
  const ::PROTOBUF_NAMESPACE_ID::Message::ClassData*GetClassData() const final;

  ::PROTOBUF_NAMESPACE_ID::Metadata GetMetadata() const final;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  enum : int {
    kServerListFieldNumber = 2,
    kCategoryFieldNumber = 1,
  };
  // repeated .message_type.srever_info server_list = 2;
  int server_list_size() const;
  private:
  int _internal_server_list_size() const;
  public:
  void clear_server_list();
  ::message_type::srever_info* mutable_server_list(int index);
  ::PROTOBUF_NAMESPACE_ID::RepeatedPtrField< ::message_type::srever_info >*
      mutable_server_list();
  private:
  const ::message_type::srever_info& _internal_server_list(int index) const;
  ::message_type::srever_info* _internal_add_server_list();
  public:
  const ::message_type::srever_info& server_list(int index) const;
  ::message_type::srever_info* add_server_list();
  const ::PROTOBUF_NAMESPACE_ID::RepeatedPtrField< ::message_type::srever_info >&
      server_list() const;

  // .message_type.message_category category = 1;
  void clear_category();
  ::message_type::message_category category() const;
  void set_category(::message_type::message_category value);
  private:
  ::message_type::message_category _internal_category() const;
  void _internal_set_category(::message_type::message_category value);
  public:

  // @@protoc_insertion_point(class_scope:message_type.route_controller)
 private:
  class _Internal;

  template <typename T> friend class ::PROTOBUF_NAMESPACE_ID::Arena::InternalHelper;
  typedef void InternalArenaConstructable_;
  typedef void DestructorSkippable_;
  struct Impl_ {
    ::PROTOBUF_NAMESPACE_ID::RepeatedPtrField< ::message_type::srever_info > server_list_;
    int category_;
    mutable ::PROTOBUF_NAMESPACE_ID::internal::CachedSize _cached_size_;
  };
  union { Impl_ _impl_; };
  friend struct ::TableStruct_controller_5fmessage_2eproto;
};
// ===================================================================


// ===================================================================

#ifdef __GNUC__
  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Wstrict-aliasing"
#endif  // __GNUC__
// srever_info

// .message_type.server_category category = 1;
inline void srever_info::clear_category() {
  _impl_.category_ = 0;
}
inline ::message_type::server_category srever_info::_internal_category() const {
  return static_cast< ::message_type::server_category >(_impl_.category_);
}
inline ::message_type::server_category srever_info::category() const {
  // @@protoc_insertion_point(field_get:message_type.srever_info.category)
  return _internal_category();
}
inline void srever_info::_internal_set_category(::message_type::server_category value) {
  
  _impl_.category_ = value;
}
inline void srever_info::set_category(::message_type::server_category value) {
  _internal_set_category(value);
  // @@protoc_insertion_point(field_set:message_type.srever_info.category)
}

// string uri = 2;
inline void srever_info::clear_uri() {
  _impl_.uri_.ClearToEmpty();
}
inline const std::string& srever_info::uri() const {
  // @@protoc_insertion_point(field_get:message_type.srever_info.uri)
  return _internal_uri();
}
template <typename ArgT0, typename... ArgT>
inline PROTOBUF_ALWAYS_INLINE
void srever_info::set_uri(ArgT0&& arg0, ArgT... args) {
 
 _impl_.uri_.Set(static_cast<ArgT0 &&>(arg0), args..., GetArenaForAllocation());
  // @@protoc_insertion_point(field_set:message_type.srever_info.uri)
}
inline std::string* srever_info::mutable_uri() {
  std::string* _s = _internal_mutable_uri();
  // @@protoc_insertion_point(field_mutable:message_type.srever_info.uri)
  return _s;
}
inline const std::string& srever_info::_internal_uri() const {
  return _impl_.uri_.Get();
}
inline void srever_info::_internal_set_uri(const std::string& value) {
  
  _impl_.uri_.Set(value, GetArenaForAllocation());
}
inline std::string* srever_info::_internal_mutable_uri() {
  
  return _impl_.uri_.Mutable(GetArenaForAllocation());
}
inline std::string* srever_info::release_uri() {
  // @@protoc_insertion_point(field_release:message_type.srever_info.uri)
  return _impl_.uri_.Release();
}
inline void srever_info::set_allocated_uri(std::string* uri) {
  if (uri != nullptr) {
    
  } else {
    
  }
  _impl_.uri_.SetAllocated(uri, GetArenaForAllocation());
#ifdef PROTOBUF_FORCE_COPY_DEFAULT_STRING
  if (_impl_.uri_.IsDefault()) {
    _impl_.uri_.Set("", GetArenaForAllocation());
  }
#endif // PROTOBUF_FORCE_COPY_DEFAULT_STRING
  // @@protoc_insertion_point(field_set_allocated:message_type.srever_info.uri)
}

// int32 load = 3;
inline void srever_info::clear_load() {
  _impl_.load_ = 0;
}
inline int32_t srever_info::_internal_load() const {
  return _impl_.load_;
}
inline int32_t srever_info::load() const {
  // @@protoc_insertion_point(field_get:message_type.srever_info.load)
  return _internal_load();
}
inline void srever_info::_internal_set_load(int32_t value) {
  
  _impl_.load_ = value;
}
inline void srever_info::set_load(int32_t value) {
  _internal_set_load(value);
  // @@protoc_insertion_point(field_set:message_type.srever_info.load)
}

// string master_uri = 4;
inline void srever_info::clear_master_uri() {
  _impl_.master_uri_.ClearToEmpty();
}
inline const std::string& srever_info::master_uri() const {
  // @@protoc_insertion_point(field_get:message_type.srever_info.master_uri)
  return _internal_master_uri();
}
template <typename ArgT0, typename... ArgT>
inline PROTOBUF_ALWAYS_INLINE
void srever_info::set_master_uri(ArgT0&& arg0, ArgT... args) {
 
 _impl_.master_uri_.Set(static_cast<ArgT0 &&>(arg0), args..., GetArenaForAllocation());
  // @@protoc_insertion_point(field_set:message_type.srever_info.master_uri)
}
inline std::string* srever_info::mutable_master_uri() {
  std::string* _s = _internal_mutable_master_uri();
  // @@protoc_insertion_point(field_mutable:message_type.srever_info.master_uri)
  return _s;
}
inline const std::string& srever_info::_internal_master_uri() const {
  return _impl_.master_uri_.Get();
}
inline void srever_info::_internal_set_master_uri(const std::string& value) {
  
  _impl_.master_uri_.Set(value, GetArenaForAllocation());
}
inline std::string* srever_info::_internal_mutable_master_uri() {
  
  return _impl_.master_uri_.Mutable(GetArenaForAllocation());
}
inline std::string* srever_info::release_master_uri() {
  // @@protoc_insertion_point(field_release:message_type.srever_info.master_uri)
  return _impl_.master_uri_.Release();
}
inline void srever_info::set_allocated_master_uri(std::string* master_uri) {
  if (master_uri != nullptr) {
    
  } else {
    
  }
  _impl_.master_uri_.SetAllocated(master_uri, GetArenaForAllocation());
#ifdef PROTOBUF_FORCE_COPY_DEFAULT_STRING
  if (_impl_.master_uri_.IsDefault()) {
    _impl_.master_uri_.Set("", GetArenaForAllocation());
  }
#endif // PROTOBUF_FORCE_COPY_DEFAULT_STRING
  // @@protoc_insertion_point(field_set_allocated:message_type.srever_info.master_uri)
}

// -------------------------------------------------------------------

// route_controller

// .message_type.message_category category = 1;
inline void route_controller::clear_category() {
  _impl_.category_ = 0;
}
inline ::message_type::message_category route_controller::_internal_category() const {
  return static_cast< ::message_type::message_category >(_impl_.category_);
}
inline ::message_type::message_category route_controller::category() const {
  // @@protoc_insertion_point(field_get:message_type.route_controller.category)
  return _internal_category();
}
inline void route_controller::_internal_set_category(::message_type::message_category value) {
  
  _impl_.category_ = value;
}
inline void route_controller::set_category(::message_type::message_category value) {
  _internal_set_category(value);
  // @@protoc_insertion_point(field_set:message_type.route_controller.category)
}

// repeated .message_type.srever_info server_list = 2;
inline int route_controller::_internal_server_list_size() const {
  return _impl_.server_list_.size();
}
inline int route_controller::server_list_size() const {
  return _internal_server_list_size();
}
inline void route_controller::clear_server_list() {
  _impl_.server_list_.Clear();
}
inline ::message_type::srever_info* route_controller::mutable_server_list(int index) {
  // @@protoc_insertion_point(field_mutable:message_type.route_controller.server_list)
  return _impl_.server_list_.Mutable(index);
}
inline ::PROTOBUF_NAMESPACE_ID::RepeatedPtrField< ::message_type::srever_info >*
route_controller::mutable_server_list() {
  // @@protoc_insertion_point(field_mutable_list:message_type.route_controller.server_list)
  return &_impl_.server_list_;
}
inline const ::message_type::srever_info& route_controller::_internal_server_list(int index) const {
  return _impl_.server_list_.Get(index);
}
inline const ::message_type::srever_info& route_controller::server_list(int index) const {
  // @@protoc_insertion_point(field_get:message_type.route_controller.server_list)
  return _internal_server_list(index);
}
inline ::message_type::srever_info* route_controller::_internal_add_server_list() {
  return _impl_.server_list_.Add();
}
inline ::message_type::srever_info* route_controller::add_server_list() {
  ::message_type::srever_info* _add = _internal_add_server_list();
  // @@protoc_insertion_point(field_add:message_type.route_controller.server_list)
  return _add;
}
inline const ::PROTOBUF_NAMESPACE_ID::RepeatedPtrField< ::message_type::srever_info >&
route_controller::server_list() const {
  // @@protoc_insertion_point(field_list:message_type.route_controller.server_list)
  return _impl_.server_list_;
}

#ifdef __GNUC__
  #pragma GCC diagnostic pop
#endif  // __GNUC__
// -------------------------------------------------------------------


// @@protoc_insertion_point(namespace_scope)

}  // namespace message_type

PROTOBUF_NAMESPACE_OPEN

template <> struct is_proto_enum< ::message_type::message_category> : ::std::true_type {};
template <>
inline const EnumDescriptor* GetEnumDescriptor< ::message_type::message_category>() {
  return ::message_type::message_category_descriptor();
}
template <> struct is_proto_enum< ::message_type::server_category> : ::std::true_type {};
template <>
inline const EnumDescriptor* GetEnumDescriptor< ::message_type::server_category>() {
  return ::message_type::server_category_descriptor();
}

PROTOBUF_NAMESPACE_CLOSE

// @@protoc_insertion_point(global_scope)

#include <google/protobuf/port_undef.inc>
#endif  // GOOGLE_PROTOBUF_INCLUDED_GOOGLE_PROTOBUF_INCLUDED_controller_5fmessage_2eproto
