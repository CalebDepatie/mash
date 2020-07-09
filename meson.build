project('mash', 'cpp',
	default_options : ['cpp_std=c++17'])

pch = 'src/pch.h'

srcs = ['src/main.cpp']

add_project_arguments(['-w'], language : 'cpp')
add_project_link_arguments(['-lstdc++fs'], language : 'cpp')

executable('mash', [srcs],
          cpp_pch : pch)
