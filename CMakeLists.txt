# Add main.cpp file of the project root directory as a source file
project(algorithms-net)
set(SOURCE_LIB_SIMPLELOG_FILES src/simplelog/simplelog.c )
set(SOURCE_LIB_CALG_FILES src/sp_calg/sp_calg.c )
set(SOURCE_LIB_CPPALG_FILES src/sp_cppalg/sp_cppalg.cpp )
set(SOURCE_TEST_ALG_BIN_FILES ../tests/console/main.cpp)


###---------------------------------------------------------------------------------------###
include_directories(./include)
add_library(libsimplelog SHARED ${SOURCE_LIB_SIMPLELOG_FILES})
target_compile_definitions(libsimplelog PRIVATE EXPORT_DLL_API_SIMPLE_LOG=1)
target_compile_definitions(libsimplelog PUBLIC _CRT_SECURE_NO_WARNINGS=1)
if(DEFINED UNIX_LINUX)
	target_compile_definitions(libsimplelog PUBLIC __UNIX_LINUX_CPP11_AND_NEWERS__=1)
else()
	target_compile_definitions(libsimplelog PUBLIC _HAS_STD_BYTE=0)
endif()

###---------------------------------------------------------------------------------------###
include_directories(./include)
add_library(libspcalg SHARED ${SOURCE_LIB_CALG_FILES})
target_compile_definitions(libspcalg PRIVATE EXPORT_DLL_API_SIMPLE_ALGORITHMS_NET=1)
target_compile_definitions(libspcalg PUBLIC _CRT_SECURE_NO_WARNINGS=1)
if(DEFINED UNIX_LINUX)
	target_compile_definitions(libspcalg PUBLIC __UNIX_LINUX_CPP11_AND_NEWERS__=1)
else()
	target_compile_definitions(libspcalg PUBLIC _HAS_STD_BYTE=0)
endif()

###---------------------------------------------------------------------------------------###
include_directories(./include)
add_library(libspcppalg SHARED ${SOURCE_LIB_CPPALG_FILES})
target_compile_definitions(libspcppalg PRIVATE EXPORT_DLL_API_SIMPLE_ALGORITHMS_NET=1)
target_compile_definitions(libspcppalg PUBLIC _CRT_SECURE_NO_WARNINGS=1)
if(DEFINED UNIX_LINUX)
	target_compile_definitions(libspcppalg PUBLIC __UNIX_LINUX_CPP11_AND_NEWERS__=1)
else()
	target_compile_definitions(libspcppalg PUBLIC _HAS_STD_BYTE=0)
endif()


###---------------------------------------------------------------------------------------###
include_directories(./include)
add_executable(spalg_test_console ${SOURCE_TEST_ALG_BIN_FILES})
if(DEFINED UNIX_LINUX)
	target_compile_definitions(spalg_test_console PUBLIC __UNIX_LINUX_CPP11_AND_NEWERS__=1)
else()
	target_compile_definitions(spalg_test_console PUBLIC _HAS_STD_BYTE=0)
endif()


###---------------------------------------------------------------------------------------###

target_link_libraries(libspcalg 
	libsimplelog
)


target_link_libraries( 
	libsimplelog
)

target_link_libraries(spalg_test_console 
	libspcppalg
	libspcalg
)
