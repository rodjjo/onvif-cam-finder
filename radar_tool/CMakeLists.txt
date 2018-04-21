file(GLOB Sources "*.cpp")

add_executable(radar-tool ${Sources})

target_include_directories (radar-tool PUBLIC ${PROJECT_SOURCE_DIR}/udp_radar)

target_link_libraries(radar-tool
                      udp-radar
                      ${Boost_FILESYSTEM_LIBRARY}
                      ${Boost_SYSTEM_LIBRARY}
                      ${Boost_THREAD_LIBRARY}
                      ${OPENSSL_LIBRARIES}
                      ${CMAKE_THREAD_LIBS_INIT})

SET_TARGET_PROPERTIES(radar-tool PROPERTIES LINKER_LANGUAGE C)
