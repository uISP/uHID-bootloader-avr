macro (defconf key)
  file(APPEND ${conf} "#define ${key} ${ARGN}\n")
endmacro()

macro(defstr key)
  string(LENGTH "${ARGN}" len)
  string(REGEX REPLACE "(.)" "'\\1'," tmp "${ARGN}")
  string(REGEX REPLACE ",$" "" tmp2 "${tmp}")
  defconf(${key} ${tmp2})
  defconf(${key}_LEN ${len})
endmacro()

macro(HEXCHAR2DEC VAR VAL)
    if (${VAL} MATCHES "[0-9]")
        SET(${VAR} ${VAL})
    elseif(${VAL} MATCHES "[aA]")
        SET(${VAR} 10)
    elseif(${VAL} MATCHES "[bB]")
        SET(${VAR} 11)
    elseif(${VAL} MATCHES "[cC]")
        SET(${VAR} 12)
    elseif(${VAL} MATCHES "[dD]")
        SET(${VAR} 13)
    elseif(${VAL} MATCHES "[eE]")
        SET(${VAR} 14)
    elseif(${VAL} MATCHES "[fF]")
        SET(${VAR} 15)
    else()
        MESSAGE(FATAL_ERROR "Invalid format for hexidecimal character")
    endif()

endmacro(HEXCHAR2DEC)

macro(HEX2DEC VAR VAL)
    IF (${VAL} EQUAL 0)
        SET(${VAR} 0)
    ELSE()

        SET(CURINDEX 0)
        STRING(LENGTH "${VAL}" CURLENGTH)

        SET(${VAR} 0)

        while (CURINDEX LESS  CURLENGTH)

            STRING(SUBSTRING "${VAL}" ${CURINDEX} 1 CHAR)

            HEXCHAR2DEC(CHAR ${CHAR})

            MATH(EXPR POWAH "(1<<((${CURLENGTH}-${CURINDEX}-1)*4))")
            MATH(EXPR CHAR "(${CHAR}*${POWAH})")
            MATH(EXPR ${VAR} "${${VAR}}+${CHAR}")
            MATH(EXPR CURINDEX "${CURINDEX}+1")
        endwhile()
    ENDIF()
endmacro(HEX2DEC)

macro(defu16 key)
  HEX2DEC(dec ${ARGN})
  math(EXPR lo "${dec} & 255")
  math(EXPR hi "(${dec} >> 8) & 255")
  defconf(${key} ${lo},${hi})
endmacro()

macro(generate_common_config)

  defconf(CONFIG_RUN_BUTTON ${CONFIG_RUN_BUTTON})
  if(NOT CONFIG_RUN_BUTTON MATCHES "none")
    defconf(CONFIG_RUN_BUTTON_PORT PORT${CONFIG_RUN_BUTTON_IOPORT})
    defconf(CONFIG_RUN_BUTTON_PIN  PIN${CONFIG_RUN_BUTTON_IOPORT})
    defconf(CONFIG_RUN_BUTTON_BIT  PIN${CONFIG_RUN_BUTTON_BIT})
  endif()

  if(CONFIG_RUN_BUTTON MATCHES "start")
    defconf(CONFIG_RUN_BUTTON_ON_START y)
  elseif(CONFIG_RUN_BUTTON MATCHES "loop")
    defconf(CONFIG_RUN_BUTTON_ON_START y)
  endif()

  if(CONFIG_UHID_VARIANT MATCHES "small")
    defconf(UHID_TINY y)
  endif()


endmacro()
