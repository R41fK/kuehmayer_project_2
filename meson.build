# uncomment as necessary
project('Remote Method Invocation System', 'cpp',
        license : ['Boost'],
        version : '1.0.0',
        default_options : ['cpp_std=c++17', 'warning_level=3', 'werror=true'])

thread_dep = dependency('threads')

# begin asio: https://think-async.com/Asio/
add_global_arguments('-I' + get_option('asio_include_dir'), language : 'cpp')
add_global_arguments('-DASIO_STANDALONE', language : 'cpp')
add_global_arguments('-DASIO_NO_DEPRECATED', language : 'cpp')
# end asio

# CLI11: https://github.com/CLIUtils/CLI11
add_global_arguments('-I' + get_option('cli11_include_dir'), language : 'cpp')

# spdlog: https://github.com/gabime/spdlog
add_global_arguments('-I' + get_option('spdlog_include_dir'), language : 'cpp')
add_global_arguments('-DSPDLOG_FMT_EXTERNAL=ON', language: 'cpp')

# json: https://github.com/nlohmann/json
add_global_arguments('-I' + get_option('json_include_dir'), language : 'cpp')

# add additional include global arguments appropriately!

# doctest: https://github.com/onqtam/doctest
add_global_arguments('-I' + get_option('doctest_include_dir'), language : 'cpp')

# cpp-peglib: https://github.com/yhirose/cpp-peglib
add_global_arguments('-I' + get_option('peglib_include_dir'), language : 'cpp')

# fmt: https://github.com/fmtlib/fmt
add_global_arguments('-I' + get_option('fmt_include_dir'), language : 'cpp')
add_global_arguments('-DFMT_HEADER_ONLY', language : 'cpp')

# magic_enum: https://github.com/Neargye/magic_enum
add_global_arguments('-I' + get_option('magic_enum_include_dir'), language : 'cpp')

# rang: https://github.com/agauniyal/rang
add_global_arguments('-I' + get_option('rang_include_dir'), language : 'cpp')

# toml++: https://github.com/marzer/tomlplusplus/
add_global_arguments('-I' + get_option('toml_include_dir'), language : 'cpp')



# # begin protobuf: https://developers.google.com/protocol-buffers/
# # protobuf: must be installed independently and has to be found...
protoc = find_program('protoc', required : true)
protobuf_dep = dependency('protobuf', required : true)
grpccpp_dep = dependency('grpc++', required : true)
grpc_plug = find_program('grpc_cpp_plugin', required : true)

gen_proto = generator(protoc, \
  output    : ['@BASENAME@.pb.cc', '@BASENAME@.pb.h'],
  arguments : ['--proto_path=@CURRENT_SOURCE_DIR@/src/messages/', '--cpp_out=@BUILD_DIR@', '@PLAINNAME@']
)

# change proto file accordingly
proto = gen_proto.process( 'src/messages/message.proto'
                   , 'src/messages/car.proto'
                   , 'src/messages/calculator.proto'
                   )
# end protobuf

gen_grpc = generator(protoc, \
  output    : ['@BASENAME@.pb.cc', '@BASENAME@.pb.h', '@BASENAME@.grpc.pb.cc', '@BASENAME@.grpc.pb.h'],
  arguments : ['--proto_path=@CURRENT_SOURCE_DIR@/src/messages/', '--grpc_out=@BUILD_DIR@', '--cpp_out=@BUILD_DIR@', '@PLAINNAME@', 
              '--plugin=protoc-gen-grpc=' + grpc_plug.path()]
)


grpc = gen_grpc.process('src/messages/grpc_message.proto')



#subdir('SUBDIR1')



inc_dir = include_directories('include')


client_src = ['src/client/c_main.cpp', 'src/pystring.cpp', 'src/config.cpp', 
              'src/car_calculator.cpp', 'src/car.cpp', 'src/car_builder.cpp',
              'src/client/repl_constructor.cpp', 'src/client/repl_functions.cpp',
              'src/message_utility.cpp', 'src/client/client_grpc.cpp']

server_src = ['src/server/s_main.cpp', 'src/pystring.cpp', 'src/config.cpp', 
              'src/car_calculator.cpp', 'src/car.cpp', 'src/car_builder.cpp', 
              'src/server/object_storage.cpp', 'src/message_utility.cpp',
              'src/server/server_grpc.cpp']

unit_test_src = ['unit_tests/main_tests.cpp', 'unit_tests/car_tests.cpp', 'src/car.cpp', 
                 'src/car_builder.cpp', 'unit_tests/car_calculator_test.cpp', 
                 'src/car_calculator.cpp', 'src/client/repl_constructor.cpp', 
                 'src/client/repl_functions.cpp', 'unit_tests/repl_tests.cpp', 
                 'src/config.cpp', 'src/message_utility.cpp', 'src/pystring.cpp', 
                 'unit_tests/message_utility_test.cpp', 'src/client/client_grpc.cpp']

int_test_src = ['integration_tests/main_int_tests.cpp', 'integration_tests/repl_int_tests.cpp',
                'src/car.cpp', 'src/car_builder.cpp', 'src/car_calculator.cpp', 
                'src/client/repl_constructor.cpp', 'src/client/repl_functions.cpp',
                'src/config.cpp', 'src/message_utility.cpp', 'src/pystring.cpp',
                'src/client/client_grpc.cpp']

#protobuf: src = ['src/main.cpp', generated]

executable('client', proto, grpc,
           sources : client_src,
           include_directories : inc_dir,
           dependencies : [thread_dep, protobuf_dep, grpccpp_dep],
           cpp_args : '-Wno-psabi'
          )

executable('server', proto, grpc,
           sources : server_src,
           include_directories : inc_dir,
           dependencies : [thread_dep, protobuf_dep, grpccpp_dep],
           cpp_args : '-Wno-psabi'
          )

executable('unit_tests', proto, grpc,
           sources : unit_test_src,
           include_directories : inc_dir,
           dependencies : [thread_dep, protobuf_dep, grpccpp_dep],
           cpp_args : ['-DTESTS', '-Wno-psabi']
          )

executable('integration_tests', proto, grpc,
           sources : int_test_src,
           include_directories : inc_dir,
           dependencies : [thread_dep, protobuf_dep, grpccpp_dep],
           cpp_args : ['-DTESTS', '-Wno-psabi']
          )