# Number of times to call the program when timing
CC = gcc-12

int_timingIterations = 200

#todo: -fprofile-use

clfags_lib = -lm
cflags = -Wall -Wextra -fmacro-prefix-map=$(dir_src)=.
cflags_debug = $(cflags) -g -Og -fsanitize=address -fsanitize=signed-integer-overflow -fsanitize=leak
cflags_release = $(cflags) -O0 -DNDEBUG # NDEBUG disables assertions

dir_bin = bin
dir_profile = profile
dir_src = src
str_exeName = sudone

# argument: $(grid)
# example: make debug n=4 grid="N4/1"
file_grid = 'sample_grids/$(grid).sud'
str_gridName = $(grid)

files_sources=$(wildcard $(dir_src)/*.c)
files_headers=$(wildcard $(dir_src)/*.h)

file_exe_release = $(dir_bin)/release_$(str_exeName)
file_exe_debug = $(dir_bin)/debug_$(str_exeName)
file_exe_gprof = $(dir_bin)/gprof_$(str_exeName)
file_exe_gcov = $(dir_src)/gcov_$(str_exeName)

$(dir_bin):
	mkdir -p $(dir_bin)

$(file_exe_debug): $(dir_bin) $(files_sources) $(files_headers)
	$(CC) $(cflags_debug) $(files_sources) -o $(file_exe_debug) $(clfags_lib)
	
$(file_exe_release): $(dir_bin) $(files_sources) $(files_headers)
	$(CC) $(cflags_release) $(files_sources) -o $(file_exe_release)	$(clfags_lib)

# Simple run
run: $(file_exe_release)
	$(file_exe_release) $(n) -s < $(file_grid)

# Debug run
debug: $(file_exe_debug)
	$(file_exe_debug) $(n) -s < $(file_grid)

# Grid testing run
test: $(file_exe_release)
	scripts/test.bash $(file_exe_release) $(file_grid)

# Benchmarking run
time: $(file_exe_release)
	scripts/time.bash $(file_exe_release) $(file_grid) $(int_timingIterations)

# gprof function profiling run
gprof: $(dir_bin) $(files_sources) $(files_headers)
	$(CC) $(cflags_release) -pg $(files_sources) -o $(file_exe_gprof) $(clfags_lib)
	scripts/gprof.bash $(file_exe_gprof) $(file_grid) $(str_gridName) $(dir_profile)

# gcov line-by-line profiling run
gcov: $(files_sources) $(files_headers)
	$(CC) $(cflags_release) --coverage -dumpbase '' $(files_sources) -o $(file_exe_gcov) $(clfags_lib)
	scripts/gcov.bash $(file_exe_gcov) $(file_grid) $(str_gridName) $(dir_profile)
	rm $(file_exe_gcov)

# Cleanup
clean:
	rm -r $(dir_bin)