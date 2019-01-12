# Makefile generated with gcmake v2.1.5 see https://github.com/Theomat/c-toolchain/blob/master/scripts/gcmake

OBJ =  ./out/objects/main.o ./out/objects/arp.o ./out/objects/ethernet.o ./out/objects/net_interface.o ./out/objects/tuntap_interface.o ./out/objects/utils.o ./out/objects/hashmap.o
CC = gcc
ARGS = -Wall -Werror -std=c99 
LD_ARGS = 

DEBUG_LD_ARGS = 
DEBUG_ARGS = -g3

TEST_ARGS = -fprofile-arcs -ftest-coverage
TEST_LD_ARGS = -fprofile-arcs -ftest-coverage

LCOV_ARGS = --rc lcov_branch_coverage=1 --rc lcov_excl_br_line=LCOV_EXCL_BR_LINE\|assert
GENHTML_ARGS = --branch-coverage --legend

project :
	@export cc_args='$(ARGS)';export ld_args='$(LD_ARGS)'; make executable

debug :
	@export cc_args='$(DEBUG_ARGS)';export ld_args='$(DEBUG_LD_ARGS)'; make executable

profiling :
	@export cc_args='$(ARGS) -pg';export ld_args='$(LD_ARGS) -pg'; make executable

executable: $(OBJ)
	@$(CC) $$ld_args -o project $(OBJ)
test: 
	@
report_cov:
	@lcov $(LCOV_ARGS) -c -d ./out/objects/ -o ./out/tmp_coverage_report.info; mv ./out/tmp_coverage_report.info ./out/coverage_report.info; mkdir -p coverage_report;genhtml $(GENHTML_ARGS) ./out/coverage_report.info -o ./coverage_report/; echo Report generated: ./coverage_report/index.html

reset_cov:
	@lcov -z $(LCOV_ARGS) -d ./out/objects/

.PHONY : clean project debug check  report_cov


./out/objects/main.o: src/main.c src/arp.h src/ethernet.h src/net_interface.h \
 src/tuntap_interface.h src/utils.h ./out/objects/
	@$(CC) $$cc_args -c src/main.c -o ./out/objects/main.o
./out/objects/arp.o: src/arp.c src/collections/hashmap.h src/ethernet.h \
 src/net_interface.h src/arp.h ./out/objects/
	@$(CC) $$cc_args -c src/arp.c -o ./out/objects/arp.o
./out/objects/ethernet.o: src/ethernet.c src/ethernet.h ./out/objects/
	@$(CC) $$cc_args -c src/ethernet.c -o ./out/objects/ethernet.o
./out/objects/net_interface.o: src/net_interface.c src/ethernet.h src/net_interface.h ./out/objects/
	@$(CC) $$cc_args -c src/net_interface.c -o ./out/objects/net_interface.o
./out/objects/tuntap_interface.o: src/tuntap_interface.c src/tuntap_interface.h \
 src/utils.h ./out/objects/
	@$(CC) $$cc_args -c src/tuntap_interface.c -o ./out/objects/tuntap_interface.o
./out/objects/utils.o: src/utils.c src/utils.h ./out/objects/
	@$(CC) $$cc_args -c src/utils.c -o ./out/objects/utils.o
./out/objects/hashmap.o: src/collections/hashmap.c src/collections/hashmap.h ./out/objects/
	@$(CC) $$cc_args -c src/collections/hashmap.c -o ./out/objects/hashmap.o


remake:
	@output=$$(ls [m,M]akefile); /home/theo/dev/projects/c-toolchain/scripts/gcmake -m src/main.c -o project $(LD_ARGS) > $$output

./out/:
	@mkdir -p ./out/
./out/objects/: ./out/
	@mkdir -p ./out/objects/
./out/tests/: ./out/
	@mkdir -p ./out/tests/
clean :
	@rm -r ./out/;rm project