main:
	g++ -g -Wall -Wextra -Wpedantic -pthread main.cpp -o main
clean:
	rm -f main
	rm -f worker
worker:
	g++ ../worker.cpp -o worker -lpthread
