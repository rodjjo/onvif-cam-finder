file(GLOB Sources "*.cpp")
set(CAMFINDER_HEADERS "camfinder/camfinder.h")

add_library(cam-finder STATIC ${Sources})

SET_TARGET_PROPERTIES(cam-finder PROPERTIES LINKER_LANGUAGE C)

install(TARGETS cam-finder
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
        COMPONENT library)

install(FILES ${CAMFINDER_HEADERS}
        DESTINATION include/camfinder)
