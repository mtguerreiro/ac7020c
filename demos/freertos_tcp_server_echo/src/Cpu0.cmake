
set(USER_COMPILE_SOURCES )

list(
    APPEND
    USER_COMPILE_SOURCES
    "${TCP_SERVER_ECHO_SRC}/main.c"
)

list(
    APPEND
    USER_COMPILE_SOURCES
    "${AC7020C_COMMON}/tasks/blink.c"
    "${AC7020C_COMMON}/ac7020c/get_uid.c"
)

list(
    APPEND
    USER_INCLUDE_DIRECTORIES
    "${AC7020C_COMMON}/tasks"
    "${AC7020C_COMMON}/ac7020c"
    "${AC7020C_EXT_LIBS}"
)

list(
    APPEND
    USER_COMPILE_SOURCES
    "${Z7_TASKS_COMMON}/netinit.c"
    "${Z7_TASKS_COMMON}/tcpecho.c"
)

list(
    APPEND
    USER_INCLUDE_DIRECTORIES
    "${Z7_TASKS_COMMON}"
)
