#!/usr/local/bin/python3
import os
import sys
import argparse
import subprocess

def run_formatter(start, extensions = [".cpp", ".hpp", ".h"], style = "google"):

    to_visit = [start]
    to_format = [] # C++ files to format using clang-format

    print(f"ℹ️ Info: running clang-format --style={style} on '*{'|*'.join(extensions)}' files...")

    while len(to_visit) > 0:
        path = to_visit.pop()
        for filename in os.listdir(path):
            filename = path + "/" + filename
            if os.path.isfile(filename):
                if filename.endswith(tuple(extensions)):
                    to_format.append(filename)
            elif os.path.isdir(filename):
                to_visit.append(filename)
    
    for filename in to_format:
        print(f"🔄 Formatting: {filename}...")
        subprocess.run(["clang-format", "--style", style, "-i", filename])

    return

def main():
    
    arg_parser = argparse.ArgumentParser(description="Format C++ code inside the directories recursively.")
    arg_parser.add_argument("--path", type=str, help="Folder containing C++ code")
    arg_parser.add_argument("--style", type=str, help="Formatting style used by clang-format", default="google")

    args = arg_parser.parse_args()

    input_path = args.path
    style = args.style

    if not os.path.isdir(input_path):
        print(f"The specified path does not exist or the path isn't a folder!")
        sys.exit()

    run_formatter(input_path, style=style)    

    return

if __name__ == "__main__":
    main()