default:
	@make -s build

all:
	@make -s test

build: configure
	@cmake --build ./out --config Debug --target all -- -j 10

configure:
	@export CC=/usr/bin/clang 
	@export CXX=/usr/bin/clang++
	@mkdir out -p && cd out && cmake ..

test:
	@make -s build
	@cd out && ctest -C out

clean:
	@rm out -rf

.PHONY: buidl configure test clean
