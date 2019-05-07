FILE(GLOB SRC *.cpp)

ADD_EXECUTABLE(nl_sample_app WIN32 ${SRC})

TARGET_LINK_LIBRARIES(nl_sample_app nelmisc)
NL_DEFAULT_PROPS(nl_sample_app "NeL, Samples, Misc: Application")
NL_ADD_RUNTIME_FLAGS(nl_sample_app)

INSTALL(TARGETS nl_sample_app RUNTIME DESTINATION ${NL_BIN_PREFIX} COMPONENT samplesmisc)
