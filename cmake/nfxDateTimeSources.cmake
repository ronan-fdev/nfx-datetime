#==============================================================================
# nfx-datetime - CMake Sources
#==============================================================================

#----------------------------------------------
# Conditional headers and sources
#----------------------------------------------

set(PUBLIC_HEADERS)
set(PRIVATE_HEADERS)
set(PRIVATE_SOURCES)

list(APPEND PUBLIC_HEADERS
	# --- Time handling headers ---
	${NFX_DATETIME_INCLUDE_DIR}/nfx/datetime/DateTime.h
	${NFX_DATETIME_INCLUDE_DIR}/nfx/datetime/DateTimeOffset.h
	${NFX_DATETIME_INCLUDE_DIR}/nfx/datetime/TimeSpan.h

	# --- Time handling implementations ---
	${NFX_DATETIME_INCLUDE_DIR}/nfx/detail/datetime/DateTime.inl
	${NFX_DATETIME_INCLUDE_DIR}/nfx/detail/datetime/DateTimeOffset.inl
	${NFX_DATETIME_INCLUDE_DIR}/nfx/detail/datetime/TimeSpan.inl
)
list(APPEND PRIVATE_HEADERS
	# --- Time handling private headers files ---
	${NFX_DATETIME_SOURCE_DIR}/Constants.h
)
list(APPEND PRIVATE_SOURCES
	# --- Time handling source files ---
	${NFX_DATETIME_SOURCE_DIR}/DateTime.cpp
	${NFX_DATETIME_SOURCE_DIR}/DateTimeOffset.cpp
	${NFX_DATETIME_SOURCE_DIR}/TimeSpan.cpp
)

#----------------------------------------------
# Library definition
#----------------------------------------------

# --- Create shared library if requested ---
if(NFX_DATETIME_BUILD_SHARED)
	add_library(${PROJECT_NAME} SHARED)
	target_sources(${PROJECT_NAME}
		PRIVATE
			${PUBLIC_HEADERS}
			${PRIVATE_HEADERS}
			${PRIVATE_SOURCES}
	)

	set_target_properties(${PROJECT_NAME} PROPERTIES
		LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
		ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
	)

	add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})
endif()

# --- Create static library if requested ---
if(NFX_DATETIME_BUILD_STATIC)
	add_library(${PROJECT_NAME}-static STATIC)
	target_sources(${PROJECT_NAME}-static
		PRIVATE
			${PUBLIC_HEADERS}
			${PRIVATE_HEADERS}
			${PRIVATE_SOURCES}
	)

	set_target_properties(${PROJECT_NAME}-static PROPERTIES
		OUTPUT_NAME ${PROJECT_NAME}-static-${PROJECT_VERSION}
		ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
	)

	add_library(${PROJECT_NAME}::static ALIAS ${PROJECT_NAME}-static)
endif()

#----------------------------------------------
# Target properties
#----------------------------------------------

function(configure_target target_name)
	# --- Include directories ---
	target_include_directories(${target_name}
		PUBLIC
			$<BUILD_INTERFACE:${NFX_DATETIME_INCLUDE_DIR}>
			$<INSTALL_INTERFACE:include>
		PRIVATE
			${NFX_DATETIME_SOURCE_DIR}
	)

	# --- Properties ---
	set_target_properties(${target_name} PROPERTIES
		CXX_STANDARD 20
		CXX_STANDARD_REQUIRED ON
		CXX_EXTENSIONS OFF
		VERSION ${PROJECT_VERSION}
		SOVERSION ${PROJECT_VERSION_MAJOR}
		POSITION_INDEPENDENT_CODE ON
		DEBUG_POSTFIX "-d"
	)
endfunction()

# --- Apply configuration to both targets ---
if(NFX_DATETIME_BUILD_SHARED)
	configure_target(${PROJECT_NAME})
	if(WIN32)
		set_target_properties(${PROJECT_NAME} PROPERTIES
			WINDOWS_EXPORT_ALL_SYMBOLS TRUE
		)

		configure_file(
			${CMAKE_CURRENT_SOURCE_DIR}/cmake/nfxDateTimeVersion.rc.in
			${CMAKE_BINARY_DIR}/nfxDateTime.rc
			@ONLY
		)
		target_sources(${PROJECT_NAME} PRIVATE ${CMAKE_BINARY_DIR}/nfxDateTime.rc)
	endif()
endif()

if(NFX_DATETIME_BUILD_STATIC)
	configure_target(${PROJECT_NAME}-static)
endif()
