file(GLOB Sources "*.cpp")
set(udpradar_HEADERS "camfinder/udpradar.h")

add_library(udp-radar STATIC ${Sources})

SET_TARGET_PROPERTIES(udp-radar PROPERTIES LINKER_LANGUAGE C)

install(TARGETS udp-radar
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
        COMPONENT library)

install(FILES ${udpradar_HEADERS}
        DESTINATION include/udpradar)
