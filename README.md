# kuehmayer_project_2


## Parametr
### -l,--log-to-file 
Activates logging to a file

### -d,--log-level-debug 
Changes to Loglevel from Info to Debug

### --log-file <Filepath> 
Specifies the location and name of the logfile

### -p,--port <positive integer>
Sets the ports. The given port and th port one higher will be used.

### Client: -s,--server-ip <Hostname or IP address> 
Sets the IP address for the client to connect to the server.

### -j,--config-file-json <Filepath>
Configurate the programm with a given JSON-file

### -t,--config-file-toml <Filepath> 
Configurate the programm with a given TOML-file

### Client: --start-server 
Starts the server with the client.

### Server: -e,--enable-shutdown 
Allows the server to be shutdown by a client.

### Server: -a, --allow-print
Allows the server to print to the consol.

## Libraries


[asio]( https://think-async.com/Asio/)

[CLI11](https://github.com/CLIUtils/CLI11)

[doctest](https://github.com/onqtam/doctest)

[fmt](https://github.com/fmtlib/fmt)

[grpc](https://grpc.io/)

[json](https://github.com/nlohmann/json)

[magic_enum](https://github.com/Neargye/magic_enum)

[peglib](https://github.com/yhirose/cpp-peglib)

[protobuf](https://developers.google.com/protocol-buffers/)

[rang](https://github.com/agauniyal/rang)

[spdlog](https://github.com/gabime/spdlog)

[toml++](https://github.com/marzer/tomlplusplus/)