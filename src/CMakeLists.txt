add_executable(aart
	Main.cpp
	Options.cpp
	BitmapImage.cpp
	FreeTyper.cpp
	CharImage.cpp
	app.rc
	app.ico)

target_link_libraries(aart ${FREETYPE_LIBRARIES} ${Argtable_LIBRARY} ${PNG_LIBRARIES})