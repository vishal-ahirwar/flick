from os import system,cpu_count,path
PROJECT_NAME:str="@projectName"#Warning : DO NOT REMOVE THIS LINE OR ADD ANYTHING ABOVE THIS LINE
BUILD_DATE_TIME:str="@builddatetime"
GENERATOR:str="\"Ninja\""
conan_file_exist:bool=path.exists("conanfile.txt")
if __name__=="__main__":
    if not conan_file_exist:
        print('Compilling Project...')
        if not system(f"ninja -C build/Debug -j{cpu_count()}") and not system(f"ninja -C build/Release -j{cpu_count()}"):
            exit(0)
        if not system(f"cmake -S . -B build/Debug -DCMAKE_BUILD_TYPE=Debug -G {GENERATOR} && cmake -S . -B build/Release -DCMAKE_BUILD_TYPE=Release -G {GENERATOR}"):
            system(f"ninja -C build/Debug -j{cpu_count()}") and not system(f"ninja -C build/Release -j{cpu_count()}")

    else:
        print('Compilling Project...')
        if not system(f"ninja -C build/Debug -j{cpu_count()}") and not system(f"ninja -C build/Release -j{cpu_count()}"):
            exit(0)
        if not system("conan install . --build=missing --settings=build_type=Debug"):
            if not system(f"cmake --preset conan-debug -G {GENERATOR}"):
                if not system("conan install . --build=missing --settings=build_type=Release"):
                    if not system(f"cmake --preset conan-release -G {GENERATOR}"):
                        system(f"ninja -C build/Debug -j{cpu_count()}") and not system(f"ninja -C build/Release -j{cpu_count()}")
                        print("Done")
        else:
            print("failed!")
