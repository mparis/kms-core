set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC -DGST_USE_UNSTABLE_API")

include(GLibHelpers)

set(KMS_TRACERS_SOURCES
  kmstracers.c
  kmslatency.c
)

set(KMS_TRACERS_HEADERS
  kmslatency.h
)

add_library(kmstracers SHARED ${KMS_TRACERS_SOURCES} ${KMS_TRACERS_HEADERS})

include_directories(
  ${gstreamer-1.5_INCLUDE_DIRS}
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
)

target_link_libraries(kmstracers
  ${gstreamer-1.5_LIBRARIES}
)

install(
  TARGETS kmstracers
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_GST_PLUGINS_DIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)
