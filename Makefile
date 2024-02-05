SHELL := /bin/env bash # to use here-strings

int_N = 4

# Number of times to call the program when timing
int_timingIterations = 200

cflags_debug = -Wall -Wextra -pedantic -fanalyzer -g -Og -fsanitize=address
# NDEBUG désactive les assertions
cflags_release = -DNDEBUG -Wall -Wextra -pedantic -O0

files_sources=$(wildcard *.c)
files_headers=$(wildcard *.h)

dir_bin = bin
dir_profile = profile
str_exeName = RESOLUTION-2

str_gridName = $(filter-out $@,$(MAKECMDGOALS))
file_grid = "../grilles/MaxiGrille$(str_gridName).sud"

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

# Exécution simple
run: $(file_exe_release)
	$(file_exe_release) <<< $(file_grid)

# Exécution en mode débougage
debug: $(file_exe_debug)
	$(file_exe_debug) <<< $(file_grid)

# Exécution et vérification de la grille de sortie
check: $(file_exe_release)
	$(file_exe_release) <<< $(file_grid) | tail -n 23 | head -n 21 | scripts/check.py $(int_N)

# Exécution un certain nombre de fois et moyenne des temps obtenus
time: $(file_exe_release)
	scripts/time.bash $(file_exe_release) $(file_grid) $(int_timingIterations)

# Profilage des fonctions avec gprof
gprof: $(dir_bin) $(files_sources) $(files_headers)
	gcc $(cflags_release) -pg $(files_sources) -o $(file_exe_gprof)
	scripts/gprof.bash $(file_exe_gprof) $(file_grid) $(str_gridName) $(dir_profile)
	
# Profilage ligne par ligne avec gcov
gcov: $(files_sources) $(files_headers)
	gcc $(cflags_release) --coverage -dumpbase '' $(files_sources) -o $(file_exe_gcov)
	scripts/gcov.bash $(file_exe_gcov) $(file_grid) $(str_gridName) $(dir_profile)
	rm $(file_exe_gcov)

# Nettoyage
clean:
	rm -r $(dir_bin)

%: # Catch-all so last argument does not cause error
	@: