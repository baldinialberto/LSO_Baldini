cmake_minimum_required (VERSION 3.8)

project (AB_SO)

set(PROJECT_SOURCE_DIR "src")
set(PROJECT_INCLUDE_DIR "include")

add_executable (AB_SO
	"${PROJECT_SOURCE_DIR}/main.c"
	"${PROJECT_SOURCE_DIR}/strings.c"
)

# TODO: Add tests and install targets if needed.
