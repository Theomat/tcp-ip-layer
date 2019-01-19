# Makefile generated with gcmake v2.1.5 see https://github.com/Theomat/c-toolchain/blob/master/scripts/gcmake

OBJ =  ./out/objects/main.o ./out/objects/ethernet.o ./out/objects/net_interface.o ./out/objects/arp.o ./out/objects/ip.o ./out/objects/tuntap_interface.o ./out/objects/print_utils.o ./out/objects/hashmap.o ./out/objects/checksum.o ./out/objects/icmp.o ./out/objects/runtime_utils.o
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
./out/tests/test_hashmap :  ./out/objects/test_hashmap.o ./out/objects/hashmap.o ./out/tests/
	@$(CC) $(TEST_LD_ARGS) -o ./out/tests/test_hashmap  ./out/objects/test_hashmap.o ./out/objects/hashmap.o
test_hashmap: 
	@export cc_args='$(TEST_ARGS)'; make ./out/tests/test_hashmap;./out/tests/test_hashmap
test:  test_hashmap
	@
report_cov:
	@lcov $(LCOV_ARGS) -c -d ./out/objects/ -o ./out/tmp_coverage_report.info; lcov $(LCOV_ARGS) -r ./out/tmp_coverage_report.info \*tests/\* -o ./out/coverage_report.info > /dev/null; mkdir -p coverage_report;genhtml $(GENHTML_ARGS) ./out/coverage_report.info -o ./coverage_report/; echo Report generated: ./coverage_report/index.html

reset_cov:
	@lcov -z $(LCOV_ARGS) -d ./out/objects/

.PHONY : clean project debug check  test_hashmap report_cov


./out/objects/main.o: src/main.c src/ethernet.h src/net_interface.h src/utils/log.h \
 src/protocols/arp.h src/protocols/ip.h src/tuntap_interface.h ./out/objects/
	@$(CC) $$cc_args -c src/main.c -o ./out/objects/main.o
./out/objects/ethernet.o: src/ethernet.c src/utils/print_utils.h src/ethernet.h ./out/objects/
	@$(CC) $$cc_args -c src/ethernet.c -o ./out/objects/ethernet.o
./out/objects/net_interface.o: src/net_interface.c src/ethernet.h \
 src/tuntap_interface.h src/utils/log.h src/net_interface.h ./out/objects/
	@$(CC) $$cc_args -c src/net_interface.c -o ./out/objects/net_interface.o
./out/objects/arp.o: src/protocols/arp.c src/protocols/../collections/hashmap.h \
 src/protocols/../ethernet.h src/protocols/../net_interface.h \
 src/protocols/../utils/log.h src/protocols/../utils/print_utils.h \
 src/protocols/arp.h ./out/objects/
	@$(CC) $$cc_args -c src/protocols/arp.c -o ./out/objects/arp.o
./out/objects/ip.o: src/protocols/ip.c src/protocols/../ethernet.h \
 src/protocols/../net_interface.h src/protocols/ip.h \
 src/protocols/../utils/checksum.h src/protocols/../utils/log.h \
 src/protocols/../utils/print_utils.h src/protocols/arp.h \
 src/protocols/icmp.h ./out/objects/
	@$(CC) $$cc_args -c src/protocols/ip.c -o ./out/objects/ip.o
./out/objects/tuntap_interface.o: src/tuntap_interface.c src/utils/log.h \
 src/utils/runtime_utils.h src/tuntap_interface.h ./out/objects/
	@$(CC) $$cc_args -c src/tuntap_interface.c -o ./out/objects/tuntap_interface.o
./out/objects/print_utils.o: src/utils/print_utils.c src/utils/print_utils.h ./out/objects/
	@$(CC) $$cc_args -c src/utils/print_utils.c -o ./out/objects/print_utils.o
./out/objects/hashmap.o: src/protocols/../collections/hashmap.c \
 src/protocols/../collections/hashmap.h ./out/objects/
	@$(CC) $$cc_args -c src/protocols/../collections/hashmap.c -o ./out/objects/hashmap.o
./out/objects/checksum.o: src/protocols/../utils/checksum.c ./out/objects/
	@$(CC) $$cc_args -c src/protocols/../utils/checksum.c -o ./out/objects/checksum.o
./out/objects/icmp.o: src/protocols/icmp.c src/protocols/../ethernet.h \
 src/protocols/../net_interface.h src/protocols/../utils/checksum.h \
 src/protocols/../utils/log.h src/protocols/ip.h src/protocols/icmp.h ./out/objects/
	@$(CC) $$cc_args -c src/protocols/icmp.c -o ./out/objects/icmp.o
./out/objects/runtime_utils.o: src/utils/runtime_utils.c src/utils/runtime_utils.h ./out/objects/
	@$(CC) $$cc_args -c src/utils/runtime_utils.c -o ./out/objects/runtime_utils.o
./out/objects/test_hashmap.o: tests/test_hashmap.c tests/../src/collections/hashmap.h \
 tests/test.h ./out/objects/
	@$(CC) $$cc_args -c tests/test_hashmap.c -o ./out/objects/test_hashmap.o


remake:
	@output=$$(ls [m,M]akefile); /home/theo/dev/projects/c-toolchain/scripts/gcmake -m src/main.c -t tests/ -o project $(LD_ARGS) > $$output

./out/:
	@mkdir -p ./out/
./out/objects/: ./out/
	@mkdir -p ./out/objects/
./out/tests/: ./out/
	@mkdir -p ./out/tests/
clean :
	@rm -r ./out/;rm project
