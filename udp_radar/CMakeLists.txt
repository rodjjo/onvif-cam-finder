file(GLOB Sources "*.cpp")
file(GLOB Testing "test/*.cpp" "test/mocks/*.cpp")
set(udpradar_HEADERS "udpradar/udpradar.h")

add_library(udp-radar STATIC ${Sources})

SET_TARGET_PROPERTIES(udp-radar PROPERTIES LINKER_LANGUAGE C)

install(TARGETS udp-radar
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
        COMPONENT library)

install(FILES ${udpradar_HEADERS}
        DESTINATION include/udpradar)
