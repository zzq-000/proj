// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: fec_type.proto
// Protobuf C++ Version: 5.26.1

#include "fec_type.pb.h"

#include <algorithm>
#include "google/protobuf/io/coded_stream.h"
#include "google/protobuf/extension_set.h"
#include "google/protobuf/wire_format_lite.h"
#include "google/protobuf/descriptor.h"
#include "google/protobuf/generated_message_reflection.h"
#include "google/protobuf/reflection_ops.h"
#include "google/protobuf/wire_format.h"
#include "google/protobuf/generated_message_tctable_impl.h"
// @@protoc_insertion_point(includes)

// Must be included last.
#include "google/protobuf/port_def.inc"
PROTOBUF_PRAGMA_INIT_SEG
namespace _pb = ::google::protobuf;
namespace _pbi = ::google::protobuf::internal;
namespace _fl = ::google::protobuf::internal::field_layout;
static const ::_pb::EnumDescriptor* file_level_enum_descriptors_fec_5ftype_2eproto[1];
static constexpr const ::_pb::ServiceDescriptor**
    file_level_service_descriptors_fec_5ftype_2eproto = nullptr;
const ::uint32_t TableStruct_fec_5ftype_2eproto::offsets[1] = {};
static constexpr ::_pbi::MigrationSchema* schemas = nullptr;
static constexpr ::_pb::Message* const* file_default_instances = nullptr;
const char descriptor_table_protodef_fec_5ftype_2eproto[] ABSL_ATTRIBUTE_SECTION_VARIABLE(
    protodesc_cold) = {
    "\n\016fec_type.proto*\332\001\n\007FecType\022\014\n\010FEC_NONE"
    "\020\000\022\013\n\007FEC_2_1\020\001\022\013\n\007FEC_3_1\020\002\022\013\n\007FEC_3_2\020"
    "\003\022\013\n\007FEC_4_1\020\004\022\013\n\007FEC_4_2\020\005\022\013\n\007FEC_4_3\020\006"
    "\022\013\n\007FEC_5_1\020\007\022\013\n\007FEC_5_2\020\010\022\013\n\007FEC_5_3\020\t\022"
    "\013\n\007FEC_5_4\020\n\022\013\n\007FEC_6_1\020\013\022\013\n\007FEC_6_2\020\014\022\013"
    "\n\007FEC_6_3\020\r\022\013\n\007FEC_6_4\020\016\022\013\n\007FEC_7_1\020\017b\006p"
    "roto3"
};
static ::absl::once_flag descriptor_table_fec_5ftype_2eproto_once;
const ::_pbi::DescriptorTable descriptor_table_fec_5ftype_2eproto = {
    false,
    false,
    245,
    descriptor_table_protodef_fec_5ftype_2eproto,
    "fec_type.proto",
    &descriptor_table_fec_5ftype_2eproto_once,
    nullptr,
    0,
    0,
    schemas,
    file_default_instances,
    TableStruct_fec_5ftype_2eproto::offsets,
    nullptr,
    file_level_enum_descriptors_fec_5ftype_2eproto,
    file_level_service_descriptors_fec_5ftype_2eproto,
};

// This function exists to be marked as weak.
// It can significantly speed up compilation by breaking up LLVM's SCC
// in the .pb.cc translation units. Large translation units see a
// reduction of more than 35% of walltime for optimized builds. Without
// the weak attribute all the messages in the file, including all the
// vtables and everything they use become part of the same SCC through
// a cycle like:
// GetMetadata -> descriptor table -> default instances ->
//   vtables -> GetMetadata
// By adding a weak function here we break the connection from the
// individual vtables back into the descriptor table.
PROTOBUF_ATTRIBUTE_WEAK const ::_pbi::DescriptorTable* descriptor_table_fec_5ftype_2eproto_getter() {
  return &descriptor_table_fec_5ftype_2eproto;
}
const ::google::protobuf::EnumDescriptor* FecType_descriptor() {
  ::google::protobuf::internal::AssignDescriptors(&descriptor_table_fec_5ftype_2eproto);
  return file_level_enum_descriptors_fec_5ftype_2eproto[0];
}
PROTOBUF_CONSTINIT const uint32_t FecType_internal_data_[] = {
    1048576u, 0u, };
bool FecType_IsValid(int value) {
  return 0 <= value && value <= 15;
}
// @@protoc_insertion_point(namespace_scope)
namespace google {
namespace protobuf {
}  // namespace protobuf
}  // namespace google
// @@protoc_insertion_point(global_scope)
PROTOBUF_ATTRIBUTE_INIT_PRIORITY2
static ::std::false_type _static_init_ PROTOBUF_UNUSED =
    (::_pbi::AddDescriptors(&descriptor_table_fec_5ftype_2eproto),
     ::std::false_type{});
#include "google/protobuf/port_undef.inc"
