cmake_minimum_required(VERSION 3.1)
project(TMP)
set(TEST_PROJECT_NAME ${PROJECT_NAME}_TESTS)

enable_language(CXX)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD 17)

#extra flags
if ("${CMAKE_CXX_COMPILER_ID}" MATCHES "GNU" OR "${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
	#general warnings
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror -g")
	#special warnings for gcc
	if ("${CMAKE_CXX_COMPILER_ID}" MATCHES "GNU")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unknown-pragmas -Wodr -Wshadow -Wredundant-decls -Wcast-align -Wmissing-declarations -Wmissing-include-dirs -Wredundant-decls -Wformat=2 -Wmissing-format-attribute -Wformat-nonliteral -Wlogical-op -Warray-bounds=2 -Wimplicit-fallthrough=5 -Walloc-zero -Wduplicated-branches -Wduplicated-cond -Wplacement-new=2 -Wrestrict -Wold-style-cast -Wzero-as-null-pointer-constant -Wformat=2 -Wformat-overflow=2 -Wformat-truncation=2 -Wformat-signedness -Wsuggest-attribute=noreturn -Wsuggest-attribute=format")
	endif()
	#special warnings for clang
	if ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Weverything -Wno-c++98-compat -Wno-c++98-compat-pedantic -Wno-newline-eof -Wno-padded -Wno-sign-conversion -Wno-exit-time-destructors -Wno-global-constructors -Wno-weak-vtables -Wno-c99-extensions -Wno-shorten-64-to-32 -Wno-range-loop-analysis -Wno-switch-enum -Wno-old-style-cast -Wodr -Wshadow -Wredundant-decls -Wcast-align -Wmissing-declarations -Wmissing-include-dirs -Wredundant-decls -Wformat=2 -Wmissing-format-attribute -Wformat-nonliteral -Warray-bounds -Wimplicit-fallthrough -Wduplicate-enum -Wold-style-cast -Wformat=2")
		if (DEFINED ENV{TRAVIS})
			set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unreachable-code") #temporarily disable unreachable warning for clang-6.0.1
		endif()
	endif()
	#disable annoying warnings
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-parameter -Wno-missing-braces")
	#stop clang from complaining about unused flags. TODO: figure out why those flags are considered unused.
	if ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-command-line-argument")
	endif()
	#extra debug flags
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fno-omit-frame-pointer -ggdb -O0")
	#environment flags
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} $ENV{CXX_FLAGS_DEBUG}")
	set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} $ENV{CXX_LFLAGS_DEBUG}")
	#release flags
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} $ENV{CXX_FLAGS_RELEASE} -DNDEBUG")
	set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} $ENV{CXX_LFLAGS_RELEASE}")
	#link time optimization for gcc only because of gold linker issues
	if ("${CMAKE_CXX_COMPILER_ID}" MATCHES "GNU")
		set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -flto")
		set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} -flto")
	endif()
endif()

# Test files
set(TMP_TEST_SRC
	tests/tests.cpp
	tests/test_callable.cpp
	tests/test_type_list.cpp
)

# Other files one would want to edit that are not directly part of the TMP build but we want to appear in projects
set(MISC_FILES
	#.clang-format
	#.gitignore
	#.travis.yml
	CMakeLists.txt
	#README.md
)
add_custom_target(MISC_FILES
	COMMENT "This target only exists to list files in IDEs"
	SOURCES ${MISC_FILES} ${UI_FILES}
)

#add test executable
add_executable(${TEST_PROJECT_NAME} ${TMP_TEST_SRC})

# Link executable
set(LIBS_TO_LINK)
target_link_libraries(${TEST_PROJECT_NAME} ${LIBS_TO_LINK})
