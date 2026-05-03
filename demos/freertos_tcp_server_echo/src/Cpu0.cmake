
set(USER_COMPILE_SOURCES )

list(
    APPEND
    USER_COMPILE_SOURCES
    "${TCP_SERVER_ECHO_SRC}/main.c"
)

list(
    APPEND
    USER_COMPILE_SOURCES
    "${AC7020C_ROOT}/common/tasks/blink.c"
    "${AC7020C_ROOT}/common/ac7020c/get_uid.c"
)

list(
    APPEND
    USER_INCLUDE_DIRECTORIES
    "${AC7020C_ROOT}/common/tasks"
    "${AC7020C_ROOT}/common/ac7020c"
    "${AC7020C_ROOT}/libs"
)

list(
    APPEND
    USER_COMPILE_SOURCES
    "${AC7020C_ROOT}/xil_z7_scripts/tasks/netinit.c"
    "${AC7020C_ROOT}/xil_z7_scripts/tasks/tcpecho.c"
)

list(
    APPEND
    USER_INCLUDE_DIRECTORIES
    "${AC7020C_ROOT}/xil_z7_scripts/tasks"
)
