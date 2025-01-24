from os import system
generator:str="\"Ninja\""
cpr:str="external/cpr"
def setup():
    if __name__=="__main__":
        print('Compilling External Dependencies...')
        system(f"cd {cpr} && cmake -S . -B Build/Debug -DCMAKE_BUILD_TYPE=Debug -G {generator} && cmake -S . -B Build/Release -DCMAKE_BUILD_TYPE=Release -G {generator}")
        system(f"cd {cpr}/Build/Debug && ninja")
        system(f"cd {cpr}/Build/Release && ninja")
setup()