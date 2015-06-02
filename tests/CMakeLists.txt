set(MultiMC_TEST_DATA_PATH "${CMAKE_CURRENT_BINARY_DIR}/data")
if(UNIX)
	# on unix we get the third / from the filename
	set(MultiMC_TEST_DATA_PATH "file://${MultiMC_TEST_DATA_PATH}")
else()
	# we don't on windows, so we have to add it ourselves
	set(MultiMC_TEST_DATA_PATH "file:///${MultiMC_TEST_DATA_PATH}")
endif()
file(GLOB data_files "data/*")
foreach(data_file ${data_files})
	get_filename_component(filename ${data_file} NAME)
	configure_file(
		${data_file}
		${CMAKE_CURRENT_BINARY_DIR}/data/${filename}
		@ONLY
		NEWLINE_STYLE LF
	)
endforeach()

configure_file(test_config.h.in ${CMAKE_BINARY_DIR}/test_config.h @ONLY)
