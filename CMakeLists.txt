cmake_minimum_required(VERSION 3.0)
set(CROSS_COMPILE avr)
set(CMAKE_TOOLCHAIN_FILE cmake/toolchain.cmake)

include(cmake/common.cmake)
include(cmake/lufa.cmake)
include(cmake/vusb.cmake)

project(uHID-avr-loader C ASM)
add_compile_options(-Os -Wall  -ffunction-sections)
include_directories(include)



function(add_loader_target name)
  include(${name})
  set(target ${CONFIG_FILENAME})

  add_executable(${target}
    ${SRC_${UHID_USB_STACK}}
    src/main-${UHID_USB_STACK}.c
    )

  target_include_directories(${target} PRIVATE ${CMAKE_BINARY_DIR}/include-${target})
  target_include_directories(${target} PRIVATE ${INC_${UHID_USB_STACK}})
  target_compile_options(${target} PRIVATE -mmcu=${CONFIG_MCU})
  target_compile_options(${target} PRIVATE -DF_CPU=${CONFIG_F_CPU})
  target_compile_options(${target} PRIVATE -DF_USB=${CONFIG_F_CPU})

  target_compile_options(${target} PRIVATE -DUHID_LOAD_ADDRESS=${CONFIG_LOAD_ADDR})
  target_compile_options(${target} PRIVATE ${CFLAGS_${UHID_USB_STACK}})

  target_link_libraries(${target} -Wl,--section-start=.text=${CONFIG_LOAD_ADDR})
  target_link_libraries(${target} -Wl,--gc-sections -Wl,--relax)
  target_link_libraries(${target} -mmcu=${CONFIG_MCU})

  if (UHID_USB_STACK MATCHES "vusb")
    generate_config_vusb(${target})
    target_compile_options(${target} PRIVATE -DUSB_STACK_VUSB)
  elseif( UHID_USB_STACK MATCHES "lufa")
    generate_config_lufa(${target})
    target_compile_options(${target} PRIVATE -DUSB_STACK_LUFA)
  else()
    message(FATAL_ERROR "USB stack ${UHID_USB_STACK} not supported")
  endif()

  if (CONFIG_UHID_EEPROM)
    target_compile_options(${target} PRIVATE -DCONFIG_UHID_EEPROM=y)
  endif()

  if (CONFIG_UHID_EEPROM_READBACK)
    target_compile_options(${target} PRIVATE -DCONFIG_UHID_EEPROM_READBACK=y)
  endif()

add_custom_command(
  TARGET ${target}
  COMMAND ${CROSS_OBJCOPY} -j .text -j .data -O ihex ${target} ${target}.hex
  COMMAND ${CROSS_SIZE_TOOL} ${CROSS_SIZE_FLAGS} ${target}
  DEPENDS ${target}
)

add_custom_command(
TARGET ${target}
COMMAND
  ${CROSS_OBJCOPY} -j .eeprom --set-section-flags=.eeprom=alloc,load
  --change-section-lma .eeprom=0 --no-change-warnings
  -O ihex ${target} ${target}.eep
DEPENDS uisp-loader
)
endfunction()

FILE(GLOB DEVS devices/*.cmake)
foreach(dev ${DEVS})
  add_loader_target(${dev})
endforeach()
