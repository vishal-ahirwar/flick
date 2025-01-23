from setup import setup,system,generator
if __name__=="__main__":
    print('Compilling Project...')
    system(f"cmake -S . -B Build/Debug -DCMAKE_BUILD_TYPE=Debug -G {generator} && cmake -S . -B Build/Release -DCMAKE_BUILD_TYPE=Release -G {generator}")
    system(f"cd Build/Debug && ninja")
    system(f"cd Build/Release && ninja")