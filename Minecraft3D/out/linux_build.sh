g++ -g \
../src/Minecraft.cpp ../src/World.cpp ../src/Menu.cpp ../src/Loader.cpp ../src/Wad.cpp ../src/Gameplay.cpp ../src/Utility.cpp \
../src/Llama3D/Llama_3D.cpp ../src/Llama3D/Llama_Math.cpp ../src/Llama3D/Llama_Graphics.cpp \
../src/Monkey2D/Monkey_2D.cpp ../src/Monkey2D/Monkey_Input.cpp \
-DUSE_SDL -lSDL2 -o LMine3D

