# Number of times to call the program when timing
int_timingIterations = 200

cflags_debug = -Wall -Wextra -pedantic -fanalyzer -g -Og -fsanitize=address -fsanitize=signed-integer-overflow
# NDEBUG disabled assertions
cflags_release = -DNDEBUG -Wall -Wextra -pedantic -O0

files_sources=$(wildcard *.c)
files_headers=$(wildcard *.h)

dir_bin = ../bin
dir_profile = ../profile
str_exeName = sudokusolve

str_gridName = $(filter-out $@,$(MAKECMDGOALS))
file_grid = "../sample_grids/$(str_gridName).sud"

file_exe_release = $(dir_bin)/release_$(str_exeName)
file_exe_debug = $(dir_bin)/debug_$(str_exeName)
file_exe_gprof = $(dir_bin)/gprof_$(str_exeName)
file_exe_gcov = gcov_$(str_exeName)

$(dir_bin):
	mkdir -p $(dir_bin)

$(file_exe_debug): $(dir_bin) $(files_sources) $(files_headers)
	gcc $(cflags_debug) $(files_sources) -o $(file_exe_debug)
	
$(file_exe_release): $(dir_bin) $(files_sources) $(files_headers)
	gcc $(cflags_release) $(files_sources) -o $(file_exe_release)	

# Simple run
run: $(file_exe_release)
	$(file_exe_release) < $(file_grid)

# Debug run
debug: $(file_exe_debug)
	$(file_exe_debug) < $(file_grid)

# Grid testing run
test: $(file_exe_release)
	scripts/test.bash $(file_exe_release) $(file_grid)

# Benchmarking run
time: $(file_exe_release)
	scripts/time.bash $(file_exe_release) $(file_grid) $(int_timingIterations)

# gprof function profiling run
gprof: $(dir_bin) $(files_sources) $(files_headers)
	gcc $(cflags_release) -pg $(files_sources) -o $(file_exe_gprof)
	scripts/gprof.bash $(file_exe_gprof) $(file_grid) $(str_gridName) $(dir_profile)
	
# gcov line-by-line profiling run
gcov: $(files_sources) $(files_headers)
	gcc $(cflags_release) --coverage -dumpbase '' $(files_sources) -o $(file_exe_gcov)
	scripts/gcov.bash $(file_exe_gcov) $(file_grid) $(str_gridName) $(dir_profile)
	rm $(file_exe_gcov)

# Cleanup
clean:
	rm -r $(dir_bin)

%: # Catch-all so last argument does not cause error
	@: