# rcube

A Rubik's cube visualizer written in C with Raylib.

## Cloning, Compiling and Running

To clone the repository, run:
```console
> git clone --recurse-submodules https://github.com/rhdcc/rcube.git
```
### Windows

To build `rcube` with the `cl` compiler, run `cl_build.bat` inside a Visual Studio 2019/2022 Developper Command Prompt.\
Learn more about that [here](https://learn.microsoft.com/en-us/visualstudio/ide/reference/command-prompt-powershell?view=vs-2022).

To build `rcube` with the `clang` compiler, run `clang_build.bat` inside any Windows Command Prompt.\

After compilation, simply run the `rcube` executable file inside the created `bin` directory. For example, run:
```console
> bin\rcube.exe
```
## Dependencies
 - [Raylib](https://www.raylib.com/)