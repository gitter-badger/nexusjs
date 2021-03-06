add_subdirectory(js)

project(nexus)

find_package(Boost 1.58 COMPONENTS program_options system thread filesystem coroutine context REQUIRED)
find_package(Iconv REQUIRED)

include(FindJavaScriptCore)

add_executable(nexus
  main.cpp nexus.cpp scheduler.cpp task.cpp object.cpp value.cpp context.cpp util.cpp
  globals/global.cpp
  globals/console.cpp
  globals/scheduler.cpp
  globals/promise.cpp
  globals/module.cpp
  globals/loader.cpp
  globals/filesystem.cpp
  globals/context.cpp
  globals/io.cpp
  classes/io/stream.cpp
  classes/io/filter.cpp
  classes/io/device.cpp
  classes/io/devices/file.cpp
  classes/io/filters/encoding.cpp
  classes/io/filters/utf8stringfilter.cpp
  classes/context.cpp
)

target_link_libraries(nexus js_bundle ${JAVASCRIPTCORE_LIBRARY} ${Boost_LIBRARIES} ${ICONV_LIBRARIES})
target_include_directories(nexus
  PRIVATE ${CMAKE_SOURCE_DIR}/include ${CMAKE_BINARY_DIR}/generated/
  SYSTEM ${JAVASCRIPTCORE_INCLUDE_DIR} ${BOOST_INCLUDE_DIR} ${ICONV_INCLUDE_DIR}
)

link_directories(${Boost_LIBRARY_DIRS})

install(TARGETS nexus RUNTIME DESTINATION bin)
