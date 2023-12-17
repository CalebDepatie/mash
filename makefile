.PHONY: all mash test fmt protobufs daemon/mash_d

all: mash daemon/mash_d

mash: protobufs
	cd ./interpreter/build-pi && ninja -j1

daemon/mash_d:
	cd ./daemon && go build -o mash_d

protobufs: daemon/execStream/execStream.pb.go interpreter/src/execStream.pb.cc

daemon/execStream/execStream.pb.go: execStream.proto
	protoc -I=./ --go_out=./daemon execStream.proto

interpreter/src/execStream.pb.cc: execStream.proto
	protoc -I=./ --cpp_out=./interpreter/src execStream.proto

# Specific to gnu make
# define run_test
# 	@echo "\n*** Running tests: $(1) ***"
# 	-cd $(1) && go test
# endef

test: protobufs
	@echo "\n*** Running tests: ./daemon/ ***"
	-cd ./daemon/ && go test

	@echo "\n\n*** Running tests: ./daemon/runtime ***"
	-cd ./daemon/runtime && go test

fmt:
	go fmt ./daemon/*.go
	cd ./interpreter && clang-format -i --style=file \
		src/*.cpp src/*.hpp \
		src/frontend/*.cpp src/frontend/*.hpp
