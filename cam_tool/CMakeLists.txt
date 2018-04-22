file(GLOB Sources "*.cpp")

add_executable(cam-tool ${Sources})

target_include_directories (cam-tool PUBLIC ${PROJECT_SOURCE_DIR}/cam_finder)

target_link_libraries(cam-tool
                      udp-radar
                      ${Boost_FILESYSTEM_LIBRARY}
                      ${Boost_SYSTEM_LIBRARY}
                      ${Boost_THREAD_LIBRARY}
                      ${OPENSSL_LIBRARIES}
                      ${CMAKE_THREAD_LIBS_INIT})

SET_TARGET_PROPERTIES(cam-tool PROPERTIES LINKER_LANGUAGE C)
