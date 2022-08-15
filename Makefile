CXX=/opt/gcc-11.2/bin/g++-11.2
CXXFLAGS=-Wall -Wextra -Wconversion -Werror -O2 -std=gnu++20
LDFLAGS=-pthread -lboost_system -lboost_program_options -Wl,-rpath -Wl,/opt/gcc-11.2/lib64

.PHONY: clean

all: robots-server robots-client

# SERVER

robots-server: robots_server.o server.o server_game.o server_game_helpers.o server_receiver.o server_event_executors.o connection.o s_message.o s_event.o s_serializer.o
	$(CXX) $(CXXFLAGS) -o robots-server robots_server.o server.o server_game.o server_game_helpers.o server_receiver.o server_event_executors.o connection.o s_message.o s_event.o s_serializer.o $(LDFLAGS)

robots_server.o: ./server/robots_server.cpp ./server/server.hpp
	$(CXX) $(CXXFLAGS) -c ./server/robots_server.cpp

server_game.o: ./server/server_game.cpp ./server/server.hpp
	$(CXX) $(CXXFLAGS) -c ./server/server_game.cpp

server_game_helpers.o: ./server/server_game_helpers.cpp ./server/server.hpp
	$(CXX) $(CXXFLAGS) -c ./server/server_game_helpers.cpp

server_receiver.o: ./server/server_receiver.cpp ./server/server.hpp
	$(CXX) $(CXXFLAGS) -c ./server/server_receiver.cpp

server_event_executors.o: ./server/server_event_executors.cpp ./server/server.hpp
	$(CXX) $(CXXFLAGS) -c ./server/server_event_executors.cpp

server.o: ./server/server.cpp ./server/server.hpp
	$(CXX) $(CXXFLAGS) -c ./server/server.cpp

connection.o: ./server/connection.cpp ./server/connection.hpp
	$(CXX) $(CXXFLAGS) -c ./server/connection.cpp

s_message.o: ./server/message.cpp ./server/message.hpp
	$(CXX) $(CXXFLAGS) -c ./server/message.cpp -o s_message.o

s_event.o: ./server/event.cpp ./server/event.hpp
	$(CXX) $(CXXFLAGS) -c ./server/event.cpp -o s_event.o

s_serializer.o: ./server/serializer.cpp ./server/serializer.hpp ./server/types.hpp
	$(CXX) $(CXXFLAGS) -c ./server/serializer.cpp -o s_serializer.o
	
# CLIENT

robots-client: robots_client.o client.o c_message.o c_event.o receiver.o deserializer.o c_serializer.o
	$(CXX) $(CXXFLAGS) -o robots-client robots_client.o client.o c_message.o c_event.o receiver.o deserializer.o c_serializer.o $(LDFLAGS)

robots_client.o: ./client/robots_client.cpp ./client/client.hpp
	$(CXX) $(CXXFLAGS) -c ./client/robots_client.cpp

client.o: ./client/client.cpp ./client/client.hpp
	$(CXX) $(CXXFLAGS) -c ./client/client.cpp

c_message.o: ./client/message.cpp ./client/message.hpp
	$(CXX) $(CXXFLAGS) -c ./client/message.cpp -o c_message.o

c_event.o: ./client/event.cpp ./client/event.hpp
	$(CXX) $(CXXFLAGS) -c ./client/event.cpp -o c_event.o

receiver.o: ./client/receiver.cpp ./client/receiver.hpp
	$(CXX) $(CXXFLAGS) -c ./client/receiver.cpp

deserializer.o: ./client/deserializer.cpp ./client/deserializer.hpp
	$(CXX) $(CXXFLAGS) -c ./client/deserializer.cpp

c_serializer.o: ./client/serializer.cpp ./client/serializer.hpp ./client/types.hpp
	$(CXX) $(CXXFLAGS) -c ./client/serializer.cpp -o c_serializer.o

clean:
	rm -f *.o robots-server robots-client
