file(GLOB Sources "*.cpp")
set(udpradar_HEADERS "camfinder/udpradar.h")

add_library(cam-finder STATIC ${Sources})

SET_TARGET_PROPERTIES(cam-finder PROPERTIES LINKER_LANGUAGE C)

install(TARGETS cam-finder
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
        COMPONENT library)

install(FILES ${udpradar_HEADERS}
        DESTINATION include/udpradar)
