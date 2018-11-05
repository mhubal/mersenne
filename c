g++ -o2 -mtune=native -march=native -m64 -o ../bin/mersenne_elim mersenne_elim.cpp mersenne_elim_core.cpp mersenne_elim_v1.cpp mersenne_elim_v1.s -pthread
#objdump -M intel -S -d ../bin/mersenne_elim > out.dump
#g++ -o2 -mtune=native -march=native -m64 -o out mersenne_elim.cpp mersenne_elim_core.cpp mersenne_elim_v1.cpp mersenne_elim_v1.s
#g++ -S -o out.asm out
