project('mash', 'cpp',
	default_options : ['cpp_std=c++2a'])

srcs = ['src/main.cpp', 'src/common.cpp', 'src/frontend/lexer.cpp', 
        'src/frontend/parser.cpp', 'src/builtins.cpp']

add_project_arguments(['-Wall'], language : 'cpp')
add_project_link_arguments(['-lstdc++fs'], language : 'cpp')

executable('mash', [srcs])
