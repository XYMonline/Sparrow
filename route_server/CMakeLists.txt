cmake_minimum_required (VERSION 3.20)

# 如果支持，请为 MSVC 编译器启用热重载。
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("route_server" VERSION 1.0)

aux_source_directory (../base BASE_SRC)
aux_source_directory (../tools/proto PROTO_SRC)
aux_source_directory (../tools TOOL_SRC)
aux_source_directory (../tools/service SERVICE_SRC)

set (SOURCE_FILES
    "../tools/cancellation_signals.cpp"
    "../base/listener.cpp"
    "../base/server.cpp"
)

add_executable (${PROJECT_NAME} "route_main.cpp" ${SOURCE_FILES} ${BASE_SRC} ${PROTO_SRC}  ${TOOL_SRC} ${SERVICE_SRC} "route_session.cpp" "auth_session.cpp" "business_session.cpp" "route_server.cpp" "controller_session.cpp")

set_target_properties(${PROJECT_NAME} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

target_link_libraries(${PROJECT_NAME} PRIVATE redis++::redis++ OpenSSL::SSL  protobuf::libprotobuf spdlog::spdlog)
