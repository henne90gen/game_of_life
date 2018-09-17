default:
	@make -s build

build: configure
	@cmake --build ./out --config Debug --target all -- -j 10

configure:
	@export CC=/usr/bin/clang 
	@export CXX=/usr/bin/clang++
	@mkdir out -p && cd out && cmake ..

clean:
	@rm out -rf
