cmake_minimum_required(VERSION 3.0)

if(NOT BIICODE)
	message(FATAL "You need to use bii for building MultiMC. Get it here: http://www.biicode.com/")
endif()

if(WIN32)
	# In Qt 5.1+ we have our own main() function, don't autolink to qtmain on Windows
	cmake_policy(SET CMP0020 OLD)
endif()


######## Set CMake options ########
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

######## Set module path ########
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/;${CMAKE_MODULE_PATH}")
set(MMC_SRC "${CMAKE_CURRENT_SOURCE_DIR}")
set(MMC_BIN "${PROJECT_BINARY_DIR}")

# Output all executables and shared libs in the main build folder, not in subfolders.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR})

if(UNIX)
	set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR})
endif()

set(CMAKE_JAVA_TARGET_OUTPUT_DIR ${PROJECT_BINARY_DIR}/jars)

######## Set compiler flags ########
include(UseCXX11)
include(Coverage)
set(CMAKE_CXX_FLAGS " -Wall ${CMAKE_CXX_FLAGS}")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Werror=return-type")

# cmake code needed for the coverity scan upload
include(Coverity)

################################ 3rd Party Libs ################################

# Find the required Qt parts
find_package(Qt5 REQUIRED COMPONENTS Core Widgets Concurrent Network Test Xml X11Extras)

include_directories(
	${Qt5Core_INCLUDE_DIRS}
	${Qt5Widgets_INCLUDE_DIRS}
	${Qt5Concurrent_INCLUDE_DIRS}
	${Qt5Test_INCLUDE_DIRS}
	${Qt5Network_INCLUDE_DIRS}
	${Qt5Xml_INCLUDE_DIRS}
)

# The Qt5 cmake files don't provide its install paths, so ask qmake.
include(QMakeQuery)
query_qmake(QT_INSTALL_PLUGINS QT_PLUGINS_DIR)
query_qmake(QT_INSTALL_IMPORTS QT_IMPORTS_DIR)
query_qmake(QT_INSTALL_LIBS QT_LIBS_DIR)
query_qmake(QT_INSTALL_LIBEXECS QT_LIBEXECS_DIR)
query_qmake(QT_HOST_DATA QT_DATA_DIR)
set(QT_MKSPECS_DIR ${QT_DATA_DIR}/mkspecs)

if (Qt5_POSITION_INDEPENDENT_CODE)
    SET(CMAKE_POSITION_INDEPENDENT_CODE ON)
endif()

################################ Included Libs ################################

# Add the java launcher and checker
add_subdirectory(depends/launcher)
add_subdirectory(depends/javacheck)

include(Coverity)

############################### Built Artifacts ###############################

add_subdirectory(tests)
add_subdirectory(application)
add_subdirectory(logic)

set(BII_LIB_SRC ${MultiMC_LOGIC_SRC})
list(APPEND BII_LIB_DEPS Qt5::Core Qt5::Xml Qt5::Widgets Qt5::Network Qt5::Concurrent ${MultiMC_LINK_ADDITIONAL_LIBS})
set(BII_LIB_TYPE SHARED)
set(BII_application_main_SRC ${MultiMC_APPLICATION_SRC})
list(REMOVE_ITEM BII_BLOCK_TESTS application_main)
set(BII_tests_tst_DownloadTask_SRC tests/tst_DownloadTask.cpp)
set(BII_tests_tst_UpdateChecker_SRC tests/tst_UpdateChecker.cpp)

add_bii_targets()

target_include_directories(${BII_application_main_TARGET} PRIVATE "${MMC_SRC}/application")

set_target_properties(${BII_application_main_TARGET} PROPERTIES OUTPUT_NAME "MultiMC")

foreach(target ${BII_BLOCK_TARGETS})
	if("${target}" MATCHES "_tests_")
		target_link_libraries(${target} PRIVATE Qt5::Test ${BII_LIB_DEPS})
		target_include_directories(${target} PRIVATE ${CMAKE_BINARY_DIR})
	endif()
endforeach()
