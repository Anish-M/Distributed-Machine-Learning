master:
	g++ -g -Wall -Wextra -Wpedantic -pthread execute/master_runner.cpp -o master
worker:
	g++ -g -Wall -Wextra -Wpedantic -pthread execute/worker_runner.cpp -o worker 
clean:
	rm -f master
	rm -f worker
