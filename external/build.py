from os import system,cpu_count,path
PROJECT_nAME:str="aura"
BUILD_DATE_TIME:str="????"
#Warning : DO nOT REMOVE THIS LInE OR ADD AnYTHInG ABOVE THIS LInE
GENERATOR:str="\"Ninja\""

if not system(f"cd cpr && ninja -C build -j{cpu_count()}"):
    system("cd cpr && cmake --install build --prefix install")
else:
    system(f"cd cpr && cmake -S . -B build -G {GENERATOR} -DBUILD_SHARED_LIBS=OFF")
    system(f"cd cpr && ninja -C build -j{cpu_count()}")
    system("cd cpr && cmake --install build --prefix install")
if not system(f"cd fmt && ninja -C build -j{cpu_count()}"):
    system("cd fmt && cmake --install build --prefix install")
else:
    system(f"cd fmt && cmake -S . -B build -G {GENERATOR} -DBUILD_SHARED_LIBS=OFF")
    system(f"cd fmt && ninja -C build -j{cpu_count()}")
    system("cd fmt && cmake --install build --prefix install")
