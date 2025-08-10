# TimeMachineLogs

## Description
A console application which allows to archive and unarchive TimeTraveller's logs from their Time Machine.

Created with C++ / Qt 6.8.3.

## How to build
Open the _CMakeLists.txt_ file in Qt Creator, configure the required kits and compile the project.

The following build settings have been tested and verified as working:

- Qt version used: 6.8.3
- Build system: cmake
- Verified to work when compiling using MinGW x64
- Tested on Windows 11 x64

Since Qt Framework is a multi-platform development too, it should be possible to compile the application
with different compilers and run it on other operating systems.

In order to deploy the application, it requires distributing the required DLLs. Run the _windeployqt_
command on the compiled executable to get the required dependencies.

## Try it out
After compiling the application run it in terminal of choice. Provide the required input arguments and wait for output.

### Help
The following command displays help regarding the application's usage:

```bash
TimeMachineLogs --help
```

### Pack mode
The following command runs the program in _PACK_ mode. It takes an input directory and an output directory as parameters:

```bash
TimeMachineLogs -m pack -i <input_directory> -o <output_directory>
```

In this mode, the program scans the provided input directory recursively, analyzes the files and produces an archive as output.

### Unpack mode
The following command runs the program in _UNPACK_ mode. It takes a path to an archive and an output directory as parameters:

```bash
TimeMachineLogs -m unpack -i <input_archive_path> -o <output_directory>
```

In this mode, the program consumes a provided archive and unpacks it to the given output directory.