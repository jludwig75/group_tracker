debug:
	mkdir -p Debug
	cd Debug && cmake -DCMAKE_BUILD_TYPE=Debug .. && make

release:
	mkdir -p Release
	cd Release && cmake -DCMAKE_BUILD_TYPE=Release .. && make

clean:
	rm -rf Debug
	rm -rf Release

all: debug release