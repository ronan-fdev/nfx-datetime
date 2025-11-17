#==============================================================================
# nfx-datetime - CMake Sources
#==============================================================================

#----------------------------------------------
# Conditional headers and sources
#----------------------------------------------

set(PRIVATE_SOURCES)

list(APPEND PRIVATE_SOURCES
	${NFX_DATETIME_SOURCE_DIR}/DateTime.cpp
	${NFX_DATETIME_SOURCE_DIR}/DateTimeOffset.cpp
	${NFX_DATETIME_SOURCE_DIR}/TimeSpan.cpp
)
