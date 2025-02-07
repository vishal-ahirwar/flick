from os import system,cpu_count,path
PROJECT_NAME:str="aura"
BUILD_DATE_TIME:str="????"
#Warning : DO NOT REMOVE THIS LINE OR ADD ANYTHING ABOVE THIS LINE
GENERATOR:str="\"Ninja\""

if not system(f"cd cpr && Ninja -C build -j{cpu_count()}"):
    system("cd cpr && cmake --install build --prefix install")
else:
    system(f"cd cpr && cmake -S . -B build -G {GENERATOR} -DBUILD_SHARED_LIBS=OFF")
    system(f"cd cpr && Ninja -C build -j{cpu_count()}")
    system("cd cpr && cmake --install build --prefix install")
if not system(f"cd fmt && Ninja -C build -j{cpu_count()}"):
    system("cd fmt && cmake --install build --prefix install")
else:
    system(f"cd fmt && cmake -S . -B build -G {GENERATOR} -DBUILD_SHARED_LIBS=OFF")
    system(f"cd fmt && Ninja -C build -j{cpu_count()}")
    system("cd fmt && cmake --install build --prefix install")