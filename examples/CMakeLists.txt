
# Check if examples are enabled
if(NOT OPTION_BUILD_EXAMPLES)
    return()
endif()

set(OpenP2P_DIR ${PROJECT_SOURCE_DIR})

# Example applications
add_subdirectory(Buffer)
add_subdirectory(Crypt)
add_subdirectory(Event)
add_subdirectory(HTTP)
add_subdirectory(OFTorrent)
add_subdirectory(RootNetwork)
add_subdirectory(RPC)
add_subdirectory(TCP)
add_subdirectory(Timer)
add_subdirectory(UDP)
