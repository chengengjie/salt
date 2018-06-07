file(GLOB SALT_SRCS *.cpp */*.cpp base/flute/*.c)

add_library(salt STATIC ${SALT_SRCS})