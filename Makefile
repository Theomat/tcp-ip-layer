# Makefile generated with gcmake v2.1.5 see https://github.com/Theomat/c-toolchain/blob/master/scripts/gcmake

OBJ =  ./out/objects/main.o ./out/objects/ethernet.o ./out/objects/net_interface.o ./out/objects/arp.o ./out/objects/ip.o ./out/objects/tun_device.o ./out/objects/log.o ./out/objects/protocols.o ./out/objects/hashmap.o ./out/objects/icmp.o ./out/objects/tcp.o ./out/objects/udp.o ./out/objects/runtime_utils.o
CC = gcc
ARGS = -Wall -Werror -std=c99 
LD_ARGS = -pthread

DEBUG_LD_ARGS = -pthread
DEBUG_ARGS = -g3

TEST_ARGS = -fprofile-arcs -ftest-coverage
TEST_LD_ARGS = -fprofile-arcs -ftest-coverage

LCOV_ARGS = --rc lcov_branch_coverage=1 --rc lcov_excl_br_line=LCOV_EXCL_BR_LINE\|assert
GENHTML_ARGS = --branch-coverage --legend

stack :
	@export cc_args='$(ARGS)';export ld_args='$(LD_ARGS)'; make executable

debug :
	@export cc_args='$(DEBUG_ARGS)';export ld_args='$(DEBUG_LD_ARGS)'; make executable

profiling :
	@export cc_args='$(ARGS) -pg';export ld_args='$(LD_ARGS) -pg'; make executable

executable: $(OBJ)
	@$(CC) $$ld_args -o stack $(OBJ)
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

.PHONY : clean stack debug check  test_hashmap report_cov


./out/objects/main.o: src/main.c src/./protocols/ethernet.h src/net_interface.h \
 src/protocols/arp.h src/protocols/ethernet.h src/protocols/ip.h \
 src/tun_device.h src/utils/log.h ./out/objects/
	@$(CC) $$cc_args -c src/main.c -o ./out/objects/main.o
./out/objects/ethernet.o: src/./protocols/ethernet.c src/./protocols/ethernet.h \
 src/./protocols/../utils/log.h ./out/objects/
	@$(CC) $$cc_args -c src/./protocols/ethernet.c -o ./out/objects/ethernet.o
./out/objects/net_interface.o: src/net_interface.c src/net_interface.h \
 src/./protocols/ethernet.h src/./utils/protocols.h src/tun_device.h \
 src/utils/log.h ./out/objects/
	@$(CC) $$cc_args -c src/net_interface.c -o ./out/objects/net_interface.o
./out/objects/arp.o: src/protocols/arp.c src/protocols/arp.h src/protocols/ethernet.h \
 src/protocols/../collections/hashmap.h src/protocols/../net_interface.h \
 src/protocols/../utils/log.h src/protocols/../utils/protocols.h ./out/objects/
	@$(CC) $$cc_args -c src/protocols/arp.c -o ./out/objects/arp.o
./out/objects/ip.o: src/protocols/ip.c src/protocols/ip.h src/protocols/ethernet.h \
 src/protocols/../net_interface.h src/protocols/../utils/log.h \
 src/protocols/../utils/protocols.h src/protocols/arp.h \
 src/protocols/icmp.h src/protocols/tcp.h src/protocols/udp.h ./out/objects/
	@$(CC) $$cc_args -c src/protocols/ip.c -o ./out/objects/ip.o
./out/objects/tun_device.o: src/tun_device.c src/tun_device.h src/utils/log.h \
 src/utils/runtime_utils.h ./out/objects/
	@$(CC) $$cc_args -c src/tun_device.c -o ./out/objects/tun_device.o
./out/objects/log.o: src/utils/log.c src/utils/log.h ./out/objects/
	@$(CC) $$cc_args -c src/utils/log.c -o ./out/objects/log.o
./out/objects/protocols.o: src/./utils/protocols.c src/./utils/protocols.h ./out/objects/
	@$(CC) $$cc_args -c src/./utils/protocols.c -o ./out/objects/protocols.o
./out/objects/hashmap.o: src/protocols/../collections/hashmap.c \
 src/protocols/../collections/hashmap.h ./out/objects/
	@$(CC) $$cc_args -c src/protocols/../collections/hashmap.c -o ./out/objects/hashmap.o
./out/objects/icmp.o: src/protocols/icmp.c src/protocols/icmp.h src/protocols/ip.h \
 src/protocols/ethernet.h src/protocols/../net_interface.h \
 src/protocols/../utils/log.h src/protocols/../utils/protocols.h ./out/objects/
	@$(CC) $$cc_args -c src/protocols/icmp.c -o ./out/objects/icmp.o
./out/objects/tcp.o: src/protocols/tcp.c src/protocols/tcp.h src/protocols/ip.h \
 src/protocols/ethernet.h src/protocols/../net_interface.h \
 src/protocols/../utils/log.h src/protocols/../utils/protocols.h ./out/objects/
	@$(CC) $$cc_args -c src/protocols/tcp.c -o ./out/objects/tcp.o
./out/objects/udp.o: src/protocols/udp.c src/protocols/../net_interface.h \
 src/protocols/../utils/log.h src/protocols/../utils/protocols.h \
 src/protocols/ip.h src/protocols/ethernet.h src/protocols/udp.h ./out/objects/
	@$(CC) $$cc_args -c src/protocols/udp.c -o ./out/objects/udp.o
./out/objects/runtime_utils.o: src/utils/runtime_utils.c src/utils/runtime_utils.h ./out/objects/
	@$(CC) $$cc_args -c src/utils/runtime_utils.c -o ./out/objects/runtime_utils.o
./out/objects/test_hashmap.o: tests/test_hashmap.c tests/../src/collections/hashmap.h \
 tests/test.h ./out/objects/
	@$(CC) $$cc_args -c tests/test_hashmap.c -o ./out/objects/test_hashmap.o


remake:
	@output=$$(ls [m,M]akefile); /home/theo/dev/projects/c-toolchain/scripts/gcmake -m src/main.c -t tests/ -o stack $(LD_ARGS) > $$output

./out/:
	@mkdir -p ./out/
./out/objects/: ./out/
	@mkdir -p ./out/objects/
./out/tests/: ./out/
	@mkdir -p ./out/tests/
clean :
	@rm -r ./out/;rm stack
