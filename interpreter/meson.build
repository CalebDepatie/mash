project('mash', 'cpp',
	default_options : ['cpp_std=c++2a'])

srcs = ['src/main.cpp', 'src/common.cpp', 'src/frontend/lexer.cpp',
        'src/frontend/parser.cpp', 'src/frontend/ast.cpp', 'src/middleend.cpp',
        'src/execStream.pb.cc']

add_project_arguments(['-Wall'], language : 'cpp')
add_project_link_arguments(['-lstdc++fs'], language : 'cpp')

depends = []

depends += dependency('protobuf')
depends += dependency('threads')

executable('mash', srcs, dependencies: depends)
