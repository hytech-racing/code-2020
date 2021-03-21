add_library					( TIME SHARED TimeLib.cpp )
target_include_directories	( TIME PUBLIC ${INC}/Time )
target_link_libraries		( TIME PUBLIC CORE )