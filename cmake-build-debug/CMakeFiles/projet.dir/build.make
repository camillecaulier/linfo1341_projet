# CMAKE generated file: DO NOT EDIT!
# Generated by "NMake Makefiles" Generator, CMake Version 3.17

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


.SUFFIXES: .hpux_make_needs_suffix_list


# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

# Suppress display of executed commands.
$(VERBOSE).SILENT:


# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE
NULL=nul
!ENDIF
SHELL = cmd.exe

# The CMake executable.
CMAKE_COMMAND = "C:\Program Files\JetBrains\CLion 2020.3.2\bin\cmake\win\bin\cmake.exe"

# The command to remove a file.
RM = "C:\Program Files\JetBrains\CLion 2020.3.2\bin\cmake\win\bin\cmake.exe" -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
<<<<<<< HEAD
CMAKE_SOURCE_DIR = "C:\Users\charl\OneDrive - UCL\Bac 3\Q2\reseau\linfo1341_projet"

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = "C:\Users\charl\OneDrive - UCL\Bac 3\Q2\reseau\linfo1341_projet\cmake-build-debug"
=======
CMAKE_SOURCE_DIR = D:\camille626\Documents\bac_3\linfo1341_reseaux\linfo1341_projet

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = D:\camille626\Documents\bac_3\linfo1341_reseaux\linfo1341_projet\cmake-build-debug
>>>>>>> cede42d90725707944736d49fbaf42992f39c1a0

# Include any dependencies generated for this target.
include CMakeFiles\projet.dir\depend.make

# Include the progress variables for this target.
include CMakeFiles\projet.dir\progress.make

# Include the compile flags for this target's objects.
include CMakeFiles\projet.dir\flags.make

<<<<<<< HEAD
CMakeFiles\projet.dir\src\log.c.obj: CMakeFiles\projet.dir\flags.make
CMakeFiles\projet.dir\src\log.c.obj: ..\src\log.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir="C:\Users\charl\OneDrive - UCL\Bac 3\Q2\reseau\linfo1341_projet\cmake-build-debug\CMakeFiles" --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/projet.dir/src/log.c.obj"
	C:\PROGRA~2\MICROS~2\2019\COMMUN~1\VC\Tools\MSVC\1428~1.293\bin\Hostx86\x86\cl.exe @<<
 /nologo $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) /FoCMakeFiles\projet.dir\src\log.c.obj /FdCMakeFiles\projet.dir\ /FS -c "C:\Users\charl\OneDrive - UCL\Bac 3\Q2\reseau\linfo1341_projet\src\log.c"
<<
=======
CMakeFiles/projet.dir/src/log.c.obj: CMakeFiles/projet.dir/flags.make
CMakeFiles/projet.dir/src/log.c.obj: CMakeFiles/projet.dir/includes_C.rsp
CMakeFiles/projet.dir/src/log.c.obj: ../src/log.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=D:\camille626\Documents\bac_3\linfo1341_reseaux\linfo1341_projet\cmake-build-debug\CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/projet.dir/src/log.c.obj"
	C:\PROGRA~1\MINGW-~1\X86_64~1.0-P\mingw64\bin\gcc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles\projet.dir\src\log.c.obj   -c D:\camille626\Documents\bac_3\linfo1341_reseaux\linfo1341_projet\src\log.c
>>>>>>> cede42d90725707944736d49fbaf42992f39c1a0

CMakeFiles\projet.dir\src\log.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/projet.dir/src/log.c.i"
<<<<<<< HEAD
	C:\PROGRA~2\MICROS~2\2019\COMMUN~1\VC\Tools\MSVC\1428~1.293\bin\Hostx86\x86\cl.exe > CMakeFiles\projet.dir\src\log.c.i @<<
 /nologo $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E "C:\Users\charl\OneDrive - UCL\Bac 3\Q2\reseau\linfo1341_projet\src\log.c"
<<
=======
	C:\PROGRA~1\MINGW-~1\X86_64~1.0-P\mingw64\bin\gcc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E D:\camille626\Documents\bac_3\linfo1341_reseaux\linfo1341_projet\src\log.c > CMakeFiles\projet.dir\src\log.c.i
>>>>>>> cede42d90725707944736d49fbaf42992f39c1a0

CMakeFiles\projet.dir\src\log.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/projet.dir/src/log.c.s"
<<<<<<< HEAD
	C:\PROGRA~2\MICROS~2\2019\COMMUN~1\VC\Tools\MSVC\1428~1.293\bin\Hostx86\x86\cl.exe @<<
 /nologo $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) /FoNUL /FAs /FaCMakeFiles\projet.dir\src\log.c.s /c "C:\Users\charl\OneDrive - UCL\Bac 3\Q2\reseau\linfo1341_projet\src\log.c"
<<

CMakeFiles\projet.dir\src\receiver.c.obj: CMakeFiles\projet.dir\flags.make
CMakeFiles\projet.dir\src\receiver.c.obj: ..\src\receiver.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir="C:\Users\charl\OneDrive - UCL\Bac 3\Q2\reseau\linfo1341_projet\cmake-build-debug\CMakeFiles" --progress-num=$(CMAKE_PROGRESS_2) "Building C object CMakeFiles/projet.dir/src/receiver.c.obj"
	C:\PROGRA~2\MICROS~2\2019\COMMUN~1\VC\Tools\MSVC\1428~1.293\bin\Hostx86\x86\cl.exe @<<
 /nologo $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) /FoCMakeFiles\projet.dir\src\receiver.c.obj /FdCMakeFiles\projet.dir\ /FS -c "C:\Users\charl\OneDrive - UCL\Bac 3\Q2\reseau\linfo1341_projet\src\receiver.c"
<<

CMakeFiles\projet.dir\src\receiver.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/projet.dir/src/receiver.c.i"
	C:\PROGRA~2\MICROS~2\2019\COMMUN~1\VC\Tools\MSVC\1428~1.293\bin\Hostx86\x86\cl.exe > CMakeFiles\projet.dir\src\receiver.c.i @<<
 /nologo $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E "C:\Users\charl\OneDrive - UCL\Bac 3\Q2\reseau\linfo1341_projet\src\receiver.c"
<<
=======
	C:\PROGRA~1\MINGW-~1\X86_64~1.0-P\mingw64\bin\gcc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S D:\camille626\Documents\bac_3\linfo1341_reseaux\linfo1341_projet\src\log.c -o CMakeFiles\projet.dir\src\log.c.s

CMakeFiles/projet.dir/src/receiver.c.obj: CMakeFiles/projet.dir/flags.make
CMakeFiles/projet.dir/src/receiver.c.obj: CMakeFiles/projet.dir/includes_C.rsp
CMakeFiles/projet.dir/src/receiver.c.obj: ../src/receiver.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=D:\camille626\Documents\bac_3\linfo1341_reseaux\linfo1341_projet\cmake-build-debug\CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building C object CMakeFiles/projet.dir/src/receiver.c.obj"
	C:\PROGRA~1\MINGW-~1\X86_64~1.0-P\mingw64\bin\gcc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles\projet.dir\src\receiver.c.obj   -c D:\camille626\Documents\bac_3\linfo1341_reseaux\linfo1341_projet\src\receiver.c

CMakeFiles/projet.dir/src/receiver.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/projet.dir/src/receiver.c.i"
	C:\PROGRA~1\MINGW-~1\X86_64~1.0-P\mingw64\bin\gcc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E D:\camille626\Documents\bac_3\linfo1341_reseaux\linfo1341_projet\src\receiver.c > CMakeFiles\projet.dir\src\receiver.c.i
>>>>>>> cede42d90725707944736d49fbaf42992f39c1a0

CMakeFiles\projet.dir\src\receiver.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/projet.dir/src/receiver.c.s"
<<<<<<< HEAD
	C:\PROGRA~2\MICROS~2\2019\COMMUN~1\VC\Tools\MSVC\1428~1.293\bin\Hostx86\x86\cl.exe @<<
 /nologo $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) /FoNUL /FAs /FaCMakeFiles\projet.dir\src\receiver.c.s /c "C:\Users\charl\OneDrive - UCL\Bac 3\Q2\reseau\linfo1341_projet\src\receiver.c"
<<

CMakeFiles\projet.dir\src\sender.c.obj: CMakeFiles\projet.dir\flags.make
CMakeFiles\projet.dir\src\sender.c.obj: ..\src\sender.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir="C:\Users\charl\OneDrive - UCL\Bac 3\Q2\reseau\linfo1341_projet\cmake-build-debug\CMakeFiles" --progress-num=$(CMAKE_PROGRESS_3) "Building C object CMakeFiles/projet.dir/src/sender.c.obj"
	C:\PROGRA~2\MICROS~2\2019\COMMUN~1\VC\Tools\MSVC\1428~1.293\bin\Hostx86\x86\cl.exe @<<
 /nologo $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) /FoCMakeFiles\projet.dir\src\sender.c.obj /FdCMakeFiles\projet.dir\ /FS -c "C:\Users\charl\OneDrive - UCL\Bac 3\Q2\reseau\linfo1341_projet\src\sender.c"
<<

CMakeFiles\projet.dir\src\sender.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/projet.dir/src/sender.c.i"
	C:\PROGRA~2\MICROS~2\2019\COMMUN~1\VC\Tools\MSVC\1428~1.293\bin\Hostx86\x86\cl.exe > CMakeFiles\projet.dir\src\sender.c.i @<<
 /nologo $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E "C:\Users\charl\OneDrive - UCL\Bac 3\Q2\reseau\linfo1341_projet\src\sender.c"
<<
=======
	C:\PROGRA~1\MINGW-~1\X86_64~1.0-P\mingw64\bin\gcc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S D:\camille626\Documents\bac_3\linfo1341_reseaux\linfo1341_projet\src\receiver.c -o CMakeFiles\projet.dir\src\receiver.c.s

CMakeFiles/projet.dir/src/sender.c.obj: CMakeFiles/projet.dir/flags.make
CMakeFiles/projet.dir/src/sender.c.obj: CMakeFiles/projet.dir/includes_C.rsp
CMakeFiles/projet.dir/src/sender.c.obj: ../src/sender.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=D:\camille626\Documents\bac_3\linfo1341_reseaux\linfo1341_projet\cmake-build-debug\CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building C object CMakeFiles/projet.dir/src/sender.c.obj"
	C:\PROGRA~1\MINGW-~1\X86_64~1.0-P\mingw64\bin\gcc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles\projet.dir\src\sender.c.obj   -c D:\camille626\Documents\bac_3\linfo1341_reseaux\linfo1341_projet\src\sender.c

CMakeFiles/projet.dir/src/sender.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/projet.dir/src/sender.c.i"
	C:\PROGRA~1\MINGW-~1\X86_64~1.0-P\mingw64\bin\gcc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E D:\camille626\Documents\bac_3\linfo1341_reseaux\linfo1341_projet\src\sender.c > CMakeFiles\projet.dir\src\sender.c.i
>>>>>>> cede42d90725707944736d49fbaf42992f39c1a0

CMakeFiles\projet.dir\src\sender.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/projet.dir/src/sender.c.s"
<<<<<<< HEAD
	C:\PROGRA~2\MICROS~2\2019\COMMUN~1\VC\Tools\MSVC\1428~1.293\bin\Hostx86\x86\cl.exe @<<
 /nologo $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) /FoNUL /FAs /FaCMakeFiles\projet.dir\src\sender.c.s /c "C:\Users\charl\OneDrive - UCL\Bac 3\Q2\reseau\linfo1341_projet\src\sender.c"
<<

CMakeFiles\projet.dir\src\packet.c.obj: CMakeFiles\projet.dir\flags.make
CMakeFiles\projet.dir\src\packet.c.obj: ..\src\packet.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir="C:\Users\charl\OneDrive - UCL\Bac 3\Q2\reseau\linfo1341_projet\cmake-build-debug\CMakeFiles" --progress-num=$(CMAKE_PROGRESS_4) "Building C object CMakeFiles/projet.dir/src/packet.c.obj"
	C:\PROGRA~2\MICROS~2\2019\COMMUN~1\VC\Tools\MSVC\1428~1.293\bin\Hostx86\x86\cl.exe @<<
 /nologo $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) /FoCMakeFiles\projet.dir\src\packet.c.obj /FdCMakeFiles\projet.dir\ /FS -c "C:\Users\charl\OneDrive - UCL\Bac 3\Q2\reseau\linfo1341_projet\src\packet.c"
<<

CMakeFiles\projet.dir\src\packet.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/projet.dir/src/packet.c.i"
	C:\PROGRA~2\MICROS~2\2019\COMMUN~1\VC\Tools\MSVC\1428~1.293\bin\Hostx86\x86\cl.exe > CMakeFiles\projet.dir\src\packet.c.i @<<
 /nologo $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E "C:\Users\charl\OneDrive - UCL\Bac 3\Q2\reseau\linfo1341_projet\src\packet.c"
<<

CMakeFiles\projet.dir\src\packet.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/projet.dir/src/packet.c.s"
	C:\PROGRA~2\MICROS~2\2019\COMMUN~1\VC\Tools\MSVC\1428~1.293\bin\Hostx86\x86\cl.exe @<<
 /nologo $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) /FoNUL /FAs /FaCMakeFiles\projet.dir\src\packet.c.s /c "C:\Users\charl\OneDrive - UCL\Bac 3\Q2\reseau\linfo1341_projet\src\packet.c"
<<
=======
	C:\PROGRA~1\MINGW-~1\X86_64~1.0-P\mingw64\bin\gcc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S D:\camille626\Documents\bac_3\linfo1341_reseaux\linfo1341_projet\src\sender.c -o CMakeFiles\projet.dir\src\sender.c.s
>>>>>>> cede42d90725707944736d49fbaf42992f39c1a0

# Object files for target projet
projet_OBJECTS = \
"CMakeFiles\projet.dir\src\log.c.obj" \
"CMakeFiles\projet.dir\src\receiver.c.obj" \
"CMakeFiles\projet.dir\src\sender.c.obj" \
"CMakeFiles\projet.dir\src\packet.c.obj"

# External object files for target projet
projet_EXTERNAL_OBJECTS =

<<<<<<< HEAD
projet.exe: CMakeFiles\projet.dir\src\log.c.obj
projet.exe: CMakeFiles\projet.dir\src\receiver.c.obj
projet.exe: CMakeFiles\projet.dir\src\sender.c.obj
projet.exe: CMakeFiles\projet.dir\src\packet.c.obj
projet.exe: CMakeFiles\projet.dir\build.make
projet.exe: CMakeFiles\projet.dir\objects1.rsp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir="C:\Users\charl\OneDrive - UCL\Bac 3\Q2\reseau\linfo1341_projet\cmake-build-debug\CMakeFiles" --progress-num=$(CMAKE_PROGRESS_5) "Linking C executable projet.exe"
	"C:\Program Files\JetBrains\CLion 2020.3.2\bin\cmake\win\bin\cmake.exe" -E vs_link_exe --intdir=CMakeFiles\projet.dir --rc=C:\PROGRA~2\WI3CF2~1\10\bin\100183~1.0\x86\rc.exe --mt=C:\PROGRA~2\WI3CF2~1\10\bin\100183~1.0\x86\mt.exe --manifests  -- C:\PROGRA~2\MICROS~2\2019\COMMUN~1\VC\Tools\MSVC\1428~1.293\bin\Hostx86\x86\link.exe /nologo @CMakeFiles\projet.dir\objects1.rsp @<<
 /out:projet.exe /implib:projet.lib /pdb:"C:\Users\charl\OneDrive - UCL\Bac 3\Q2\reseau\linfo1341_projet\cmake-build-debug\projet.pdb" /version:0.0  /machine:X86 /debug /INCREMENTAL /subsystem:console  kernel32.lib user32.lib gdi32.lib winspool.lib shell32.lib ole32.lib oleaut32.lib uuid.lib comdlg32.lib advapi32.lib 
<<
=======
projet.exe: CMakeFiles/projet.dir/src/log.c.obj
projet.exe: CMakeFiles/projet.dir/src/receiver.c.obj
projet.exe: CMakeFiles/projet.dir/src/sender.c.obj
projet.exe: CMakeFiles/projet.dir/build.make
projet.exe: CMakeFiles/projet.dir/linklibs.rsp
projet.exe: CMakeFiles/projet.dir/objects1.rsp
projet.exe: CMakeFiles/projet.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=D:\camille626\Documents\bac_3\linfo1341_reseaux\linfo1341_projet\cmake-build-debug\CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Linking C executable projet.exe"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles\projet.dir\link.txt --verbose=$(VERBOSE)
>>>>>>> cede42d90725707944736d49fbaf42992f39c1a0

# Rule to build all files generated by this target.
CMakeFiles\projet.dir\build: projet.exe

.PHONY : CMakeFiles\projet.dir\build

CMakeFiles\projet.dir\clean:
	$(CMAKE_COMMAND) -P CMakeFiles\projet.dir\cmake_clean.cmake
.PHONY : CMakeFiles\projet.dir\clean

<<<<<<< HEAD
CMakeFiles\projet.dir\depend:
	$(CMAKE_COMMAND) -E cmake_depends "NMake Makefiles" "C:\Users\charl\OneDrive - UCL\Bac 3\Q2\reseau\linfo1341_projet" "C:\Users\charl\OneDrive - UCL\Bac 3\Q2\reseau\linfo1341_projet" "C:\Users\charl\OneDrive - UCL\Bac 3\Q2\reseau\linfo1341_projet\cmake-build-debug" "C:\Users\charl\OneDrive - UCL\Bac 3\Q2\reseau\linfo1341_projet\cmake-build-debug" "C:\Users\charl\OneDrive - UCL\Bac 3\Q2\reseau\linfo1341_projet\cmake-build-debug\CMakeFiles\projet.dir\DependInfo.cmake" --color=$(COLOR)
.PHONY : CMakeFiles\projet.dir\depend
=======
CMakeFiles/projet.dir/depend:
	$(CMAKE_COMMAND) -E cmake_depends "MinGW Makefiles" D:\camille626\Documents\bac_3\linfo1341_reseaux\linfo1341_projet D:\camille626\Documents\bac_3\linfo1341_reseaux\linfo1341_projet D:\camille626\Documents\bac_3\linfo1341_reseaux\linfo1341_projet\cmake-build-debug D:\camille626\Documents\bac_3\linfo1341_reseaux\linfo1341_projet\cmake-build-debug D:\camille626\Documents\bac_3\linfo1341_reseaux\linfo1341_projet\cmake-build-debug\CMakeFiles\projet.dir\DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/projet.dir/depend
>>>>>>> cede42d90725707944736d49fbaf42992f39c1a0

