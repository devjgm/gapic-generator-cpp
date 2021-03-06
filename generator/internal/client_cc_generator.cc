// Copyright 2019 Google Inc.  All rights reserved
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <memory>
#include <sstream>
#include <string>

#include "gapic_utils.h"
#include "printer.h"
#include "src/google/protobuf/descriptor.h"
#include "generator/internal/client_cc_generator.h"
#include "generator/internal/data_model.h"

namespace pb = google::protobuf;

namespace google {
namespace api {
namespace codegen {
namespace internal {

std::vector<std::string> BuildClientCCIncludes(
    pb::ServiceDescriptor const* service) {
  return {
      LocalInclude(absl::StrCat(
          internal::ServiceNameToFilePath(service->name()), ".gapic.h")),
      LocalInclude(absl::StrCat(
          internal::ServiceNameToFilePath(service->name()), "_stub.gapic.h")),
      LocalInclude("gax/status.h"), LocalInclude("gax/status_or.h"),
  };
}

std::vector<std::string> BuildClientCCNamespaces(
    pb::ServiceDescriptor const* /* service */) {
  return std::vector<std::string>();
}

bool GenerateClientCC(pb::ServiceDescriptor const* service,
                      std::map<std::string, std::string> const& vars,
                      Printer& p, std::string* /* error */) {
  auto includes = BuildClientCCIncludes(service);
  auto namespaces = BuildClientCCNamespaces(service);

  p->Print(vars,
           "// Generated by the GAPIC C++ plugin.\n"
           "// If you make any local changes, they will be lost.\n"
           "// source: $proto_file_name$\n");

  for (auto include : includes) {
    p->Print("#include $include$\n", "include", include);
  }
  for (auto nspace : namespaces) {
    p->Print("namespace $namespace$ {\n", "namespace", nspace);
  }

  p->Print("\n");

  DataModel::PrintMethods(
      service, vars, p,
      "gax::StatusOr<$response_object$>\n"
      "$class_name$::$method_name$(\n"
      "$request_object$ const& request) {\n"
      "  // TODO: actual useful work, e.g. retry, backoff, metadata, "
      "pagination, etc.\n"
      "  grpc::ClientContext context;\n"
      "  $response_object$ response;\n"
      "  grpc::Status status = stub_->$method_name$(&context, request, "
      "&response);\n"
      "  if (status.ok()) {\n"
      "    return response;\n"
      "  } else {\n"
      "    return gax::GrpcStatusToGaxStatus(status);\n"
      "  }\n"
      "}\n"
      "\n",
      NoStreamingPredicate);

  for (auto nspace : namespaces) {
    p->Print("\n} // namespace $namespace$", "namespace", nspace);
  }

  return true;
}  // namespace internal

}  // namespace internal
}  // namespace codegen
}  // namespace api
}  // namespace google
