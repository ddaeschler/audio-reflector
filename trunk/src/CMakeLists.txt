PROJECT( Reflector )
cmake_minimum_required(VERSION 2.6)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

find_package( Portaudio )
find_package(Boost COMPONENTS thread system REQUIRED)
include_directories(${PORTAUDIO_INCLUDE_DIRS} ${Boost_INCLUDE_DIR} )
set(LIBS ${LIBS} ${PORTAUDIO_LIBRARIES} ${Boost_THREAD_LIBRARY} ${Boost_SYSTEM_LIBRARY})

set ( Reflector_srcs Reflector.cpp ARServer.cpp StreamSubscriber.cpp ARClient.cpp EncoderStage.cpp IEncoder.cpp )
ADD_EXECUTABLE( Reflector ${Reflector_srcs})
target_link_libraries(Reflector ${LIBS})
