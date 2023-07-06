.PHONY: all mash test fmt protobufs

all: mash

mash: protobufs
	cd ./interpreter/build && ninja -j2

protobufs: daemon/execStream/execStream.pb.go interpreter/src/execStream.pb.cc

daemon/execStream/execStream.pb.go: execStream.proto
	protoc -I=./ --go_out=./daemon execStream.proto

interpreter/src/execStream.pb.cc: execStream.proto
	protoc -I=./ --cpp_out=./interpreter/src execStream.proto

test: protobufs
	cd ./daemon && \
	go test

fmt:
	go fmt ./daemon/*.go
	cd ./interpreter && clang-format -i --style=file \
		src/*.cpp src/*.hpp \
		src/frontend/*.cpp src/frontend/*.hpp
