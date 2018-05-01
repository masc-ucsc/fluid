
all: run

straight_test:
	verilator +incdir+rtl --Wall --cc --trace ./rtl/straight_test.v rtl/fflop.v --exe tests/straight_test_tb.cpp -CFLAGS "-DTRACE=1 -DVL_DEBUG=1"
	make -C obj_dir/ -f Vstraight_test.mk Vstraight_test

run1: straight_test
	./obj_dir/Vstraight_test

join_test:
	verilator +incdir+rtl --Wall --cc --trace ./rtl/join_test.v rtl/fflop.v --exe tests/join_test_tb.cpp -CFLAGS "-DTRACE=1 -DVL_DEBUG=1"
	make -C obj_dir/ -f Vjoin_test.mk Vjoin_test

run2: join_test
	./obj_dir/Vjoin_test

fork_test:
	verilator +incdir+rtl --Wall --cc --trace ./rtl/fork_test.v rtl/fflop.v --exe tests/fork_test_tb.cpp -CFLAGS "-DTRACE=1 -DVL_DEBUG=1"
	make -C obj_dir/ -f Vfork_test.mk Vfork_test

run3: fork_test
	./obj_dir/Vfork_test

run: run1 run2

clean:
	rm -rf obj_dir output.vcd a.out

