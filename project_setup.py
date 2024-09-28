import os
import sys
import subprocess

if __name__ == "__main__":
    subprocess.run(("./ext/premake/premake5-win.exe", "vs2022"))