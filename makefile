.PHONY: all mash test fmt protobufs daemon/mash_d test-go

all: mash daemon/mash_d

mash: protobufs
	cd ./interpreter/build && ninja -j1

daemon/mash_d:
	cd ./daemon && go build -buildvcs=false -o mash_d

protobufs: daemon/execStream/execStream.pb.go interpreter/src/execStream.pb.cc

daemon/execStream/execStream.pb.go: execStream.proto
	protoc -I=./ --go_out=./daemon execStream.proto

interpreter/src/execStream.pb.cc: execStream.proto
	protoc -I=./ --cpp_out=./interpreter/src execStream.proto

# Specific to gnu make
define run_go_test
	@echo "\n*** Running tests: $(1) ***"
	-@cd $(1) && go test
endef

test: mash daemon/mash_d test-go
	@echo "\n*** Running integration tests ***"
	@./scripts/test_helper.sh


test-go: protobufs
	$(call run_go_test, ./daemon/)
	$(call run_go_test, ./daemon/runtime)

fmt:
	go fmt ./daemon/*.go
	cd ./interpreter && clang-format -i --style=file \
		src/*.cpp src/*.hpp \
		src/frontend/*.cpp src/frontend/*.hpp

clean:
	-rm daemon/execStream/execStream.pb.go
	-rm interpreter/src/execStream.pb.*