## Raylib Starter Kit
This repository is to raylib starter kit to help get up and running quickly after installing MSVC, clang or gcc, and emcc.

### Development Setup
Brief summary of the list of dependencies and steps needed in order to build the project.

Project uses following libraries:
- [raylib][raylib]
  - [download windows installer][raylib-dl]
  - [wiki][raylib-wiki]
  - [cheatsheet][raylib-cheatsheet]
  
### General setup
- Windows CMD
  - Several CMD shortcut icons that execute different .bat script can be saved to your desktop with the following added to the target field in the cmd properties (i.e., right-click properties on CMD shortcut) -> %windir%\system32\cmd.exe /k "C:\Users\{username}\shell-win32.bat".
  - A copy of that shell-*.bat file will need to exist in the location specificed.
  
- Game Source Code / Libraries
  - Source code is stored in game folder under the src folder.
  - Libaries are contained in the game folder, src, and the libs folder - please do not move these around.
  - Game assets should be stored in the assets folder, and the build scripts should copy them over to the build location.

#### Win32
- Compiler: [MSVC][msvc] (visual studio / c and c++ installation only)
- Env Script: Shell-win32.bat
  - environment is contained with cmd instance running another compiler could mess with set envs.
- Build Script: build-win32.bat

#### Linux/Mac
- TODO

#### Web
- SDK: emcscriptensdk
  - [download sdk][emscripten-sdk]
- Compiler: emcc

##### emcc - emscripten setup
- Install SDK and run the shell-emcc.bat file provided by this repo in a cmd instance (NOTE: c:\emsdk install location is assumed, if not installed there change in script")

[emscripten-sdk]:http://kripken.github.io/emscripten-site/
[msvc]: https://www.visualstudio.com/vs/community/
[raylib]: https://github.com/raysan5/raylib
[raylib-cheatsheet]:http://www.raylib.com/cheatsheet/cheatsheet.html
[raylib-dl]:https://raysan5.itch.io/raylib/download/eyJleHBpcmVzIjoxNTEyMTk5MzA4LCJpZCI6ODUzMzF9.LqUq4shrk%2b6p%2faTV7y7CumPj%2b5w%3d 
[raylib-wiki]:https://github.com/raysan5/raylib/wiki 
