build: mash_daemon mash

mash_daemon_exec:
	go run ./daemon

mash_daemon:
	go build ./daemon

mash:
	cd ./interpreter/build && ninja -j2
