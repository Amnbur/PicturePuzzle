# 自用 makefile，编译需要另外编写
# write another makefile if possible

export CPLUS_INCLUDE_PATH=D:\programs\code\SDL2-2.28.5\i686-w64-mingw32\include\SDL2
export LD_LIBRARY_PATH=C:\Windows\System32
export LIBRARY_PATH="D:\programs\code\SDL2-2.28.5\x86_64-w64-mingw32\lib"

dlls = SDL2_image.dll SDL2.dll
AbandonedDLL = SDL2_rotozoom.dll SDL2_gfxPrimitives.dll
libs = -lSDL2 -lSDL2main -lSDL2_image
withLib = -L$(LIBRARY_PATH) $(libs)

PicturePuzzle:
	g++ -m64 $(dlls) PicturePuzzle.cpp -g2 -o PicturePuzzle
Pstatic:
	g++ -m64 $(withLib) PicturePuzzle.cpp -g2 -o .\static\PicturePuzzle
	# error got
test:
	g++ -m64 $(dlls) test.cpp -g2 -o test
list:
	echo "PicturePuzzle"
	echo "Pstatic"
	echo "test"
	echo "list"
	echo "PPrelease"
	echo "clean"
PPrelease:
	md release
	g++ -m64 $(dlls) PicturePuzzle.cpp -o .\release\PicturePuzzle
	GetUsualDLL.cmd
	md .\release\src
	xcopy /s /e src .\release\src
	copy *.dll .\release
	copy vic.cmd .\release
	copy VictoryInfo.txt .\release
	copy StartGame.cmd .\release
	copy ReadMe.Md .\release
	copy README.txt .\release
clean:
	rd /s /q release