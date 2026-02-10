1 Abstract
We created a distributed system that trains a neural network concurrently. We used a non-trivial number of CS machines that first train a neural network on their own subsets of the training data. These machines then send their updated networks to the master computer, where the weights and biases across all networks are averaged per layer, and a new network is constructed. This network is then sent back to the worker machines to start training the next epoch. On each machine, we will additionally parallelize the part of the neural network it is running by distributing work with multithreading using pthreads. This project combines distributed systems and multithreading of a very complicated algorithm (i.e. neural networks) that we will be writing from scratch. The master uses message passing to communicate with all the workers. The workers themselves will have parallelization within to further bolster speedup. The net will function as a typical neural net and be able to be configured accordingly, with the additional configuration for distribution and parallelization.


2 Goals
Our goal was to design a neural network in scratch in both Python, and subsequently C++. It should mimic the results and overall run nature of typically neural net libraries such as Keras and TensorFlow. The goal was then to distribute the creation of the neural net over various UTCS lab machines and enable them to communicate for speedup in the computation. Furthermore, within each CS lab machine, the goal was to use pthreads to further parallelize and improve speedup.


3 Approach
3.1 Method of Parallelism For Machine Distribution and Thread Parallelization
Examining neural nets, there are two major forms of parallelization we could focus on: data parallelism and model parallelism. Model parallelism, where different parts of a neural network model are processed on different nodes, offers the advantage of handling larger models that might not fit into the memory of a single machine. However, this approach often suffers from significant communication overhead, especially when the model layers are tightly interconnected. On the other hand, data parallelism involves distributing different subsets of the training data across multiple nodes, with each node having a complete copy of the model. This method is generally more scalable and easier to implement, especially in distributed environments where synchronization across nodes is a challenge. 


Our project intended to implement both, however, when testing the program and doing the math for message passing between computers, we noticed that one implementation seemed better. 


After testing the time to send messages of various sizes between machines, we arrived at an average cost of 2 ms per message. The communication overhead from creating threads was insignificant compared to this. 


Data parallelism would focus on sending a message from worker to master once per epoch, and then sending a message from master to worker once per epoch. This meant 2 messages were sent per worker per epoch. Thus, the additional overhead could be estimated by the overall formula:


	Overhead in ms = 2 * numWorkers * numEpochs * 2


Model parallelism would focus on sending a message for each datapoint as it goes through each layer and thus nodes would be sent over and aggregated together. Each machine would handle different nodes and communicate them with each other. As data passes through each segment, the corresponding machine performs necessary computations (like forward and backward passes in training) and then passes the results to the next machine in line. This involves a lot of inter-machine communication, especially if the model has many interconnected layers. Each message is 2s and messages must be send by and to the worker for each (roundtrip). Messages must be sent twice for each layer and once for the initial and final. Finally, this is multiplied by the number of workers for a final result of:

Overhead in ms = 2 * (2 * (L − 2) + 2) * numWorkers




Given the architecture of our system and the nature of our datasets, we opted for data parallelism. This approach not only simplifies the synchronization process but also allows for more efficient utilization of our distributed resources, as each node can independently train on different data subsets and later aggregate the learnings. Additionally, and most importantly, the overhead in message passing is significant with distributed workloads, which made us favor data parallelism for increased speedup.


3.2 Starting Approach
We began by implementing a python version of a neural net to establish a baseline and compare it to an official neural net from tensorflow. This baseline was then translated to C++ for a sequential implementation. The results of this were compared to both other nets to ensure accuracy and proper set up of the net. We ran the results on a variety of small and large datasets. Small datasets were the initial test and after, once the large dataset was working, we measured and ensured message passing between computers’ overhead would not overcome any possible speedup. 


After properly setting up the net, we focused on data parallelism first. We began by examining a way to train parts of the data separately while also communicating their weights and updating so the overall model would be updated and trained accordingly. We did this by simply aggregating the weights and averaging them to develop weights for the new model. This proved data parallelism was possible, so we went about implementing it.


We first established a master machine and worker machines. They would connect and communicate between others. We created numWorkers of distinct threads to run concurrently within master and send the information to the worker machines. 


The master will create a net with random initialization of the weights and biases. Its activation functions, layers, and weights would be passed to the workers for them to set up a replica of the nets. Finally, we passed in a set of integers relating to the indexes of the data that each machine is responsible for. In each machine, we further parallelized the data using pthreads, splitting the training and test evenly between the threads. These threads will be managed by a barrier that ensures that the net is updated after a chunk of the data makes it through. 


3.3 Message Passing
Socket programming was a complicated task we had to tackle when communicating between different messages. When sending long strings via socket programming, the message would automatically be broken up and read in. To handle this case, we had to set multiple states that tracked the start and end of each message. When we received the final end piece of a message, we then pieced the methods together. 


The master would have to first bind a socket to port 8000. However, this port could be unavailable so it would loop through from 8000 to 8005 finding the first available port. The worker machines then connect to the socket and the master machine waits for n_clients machines to connect. This is dynamically set so any workers can join the master and the master will accept their connection. 


To read in the messages continuously was another challenge that we faced, but our solution for this problem was multithreading. The master started a continuous thread to read from each of the n_clients. The thread would run an infinite loop that continuously read incoming messages while the main thread would handle then sending of the messages. For each of the workers, we launched a reading thread to continuously read messages. Every time a message is received, it invokes a message handler that synchronously handles messages and returns execution to the thread.


3.4 Message Failure & Fault Tolerance
The network is unreliable resource that can drop messages. After running the neural net various times, we noticed varying drops. The reasons ended up being that messages were dropped and with large amounts of workers / epochs, we ended up encountering many drops that halted our program execution.


We fixed this by implementing fault tolerance for both messages from the master to the worker and the worker to the master. If the master did not receive a message from a worker (i.e. a result from the worker when it was done), it would notify the worker periodically based on a timeout timer that we set. The worker, if it was still working on a current epoch, would do nothing and let the master wait. Otherwise, it would resend its neural net to the master. 


If the worker did not receive the message, the worker would notify the master and the master, upon reading this, would resend the new neural net to any and all workers needing a new message.


This enabled us to run our neural network consistently without worrying about message dropping, enabling for a higher level of parallelism and higher speedup.


4 Hardware Used
Google Collab was used for the Keras baseline. UTCS Lab Machines were used for the C++ sequential, distributed, and parallelized implementations. The pedagogical machines were used to train large datasets for their larger memory for the program to take advantage of.


5 Issues
We faced many issues over the bulk of the project. Prominent issues ranged from sending messages between machines, aggregating neural nets together, and creating a functioning and learning neural network. 

One of the primary issues was being unable to pass the entire message through the socket. The socket would take one large string that we were sending and break it into smaller individual messages. We decide to rather than restructure the messages, instead read each element of the message separately and look for it exactly. This allowed us to go around the socket’s irregularity in parsing the large string representing the neural network.

Another issue was passing in the neural network itself. This involved creating a large string with the weights, layer, activation function, biases, and more. This would be for all layers. This was used so each worker started with the same “random” neural net determined by the master. The workers would then also send their neural nets back to master after an epoch for the master to aggregate and send back to the workers for the next epoch.

7 How To Run
To run our code, there must be n worker machines and a designated master machine. SSH or log into all the desired machines. In worker_runner.cpp’s main() method, the machine hostname should be set to the machine in which you are running master. It should be “[machine name].cs.utexas.edu”. A sample name in the correct format is there. Afterwards, in master_runner.cpp, n_clients should be changed to the desired number of clients for this run. 


A file is already loaded in and set up to run. If another file is desired, pick a filename in the designated format and location. Set n_samples, n_features, and n_classes in the readInFile() method in worker_runner and main() method in master_runner.cpp.


Finally, if desired, change the epoches master_runner and worker_runner in each of their respective main methods. A default number of epochs is set so this is not necessary if you do not want to set it.


The master machine should do “make clean && make master && master”. After running this, the port number should be noted and ensure that it lines up with the port set in worker_runner. If not, either change the port number before running (in main() in the file worker_runner.cpp), or recompile till the desired port is found.


The worker machines should each do “make clean && make worker && worker”. The worker should set the host to the machine (ex: “hydra.cs.utexas.edu”) that is running master_runner.cpp. The number of clients should be set in the master file as n_clients. 


After all workers are initialized and have successfully connected (can be viewed in master terminal), the program will begin to run. The output for the training and timing can be seen in the terminals of all workers. The final timing is viewable in the master terminal.


Baseline Measurement
Our network architecture was to use a Sequential neural network with keras Tensorflow. The baseline model running this architecture on Google Colab took 271 seconds, with an average time per epoch was 27.1 s.
















Results


Data Parallelism Tests
Test 1 Details
  n_samples = 10000;
  n_features = 60;
  n_classes = 2;
  n_clients = 2;
  epochs = 20;



1 Client (Sequential)
2 Clients (Parallel)
4 Clients
(Parallel)
6 Clients (Parallel)
Total Training Time
192 s 
93 s
67 s
33 s
Average Time Per Epoch
9.6 s
4.8 s
3.35 s
1.65 s


A clear speed up was noticed, to a very high level considering the 1/x pattern emerging in the data. This means the program is almost entirely parallelizable, which lines up with our expectations as the epochs are the bulk of the time, with minimal overhead for sending, receiving, and parsing the various neural nets between epochs.

Test 2 Details
  n_samples = 50000;
  n_features = 60;
  n_classes = 2;
  epochs = 20;



1 Client
(Sequential)
4 Clients
(Parallel)
6 Clients (Parallel)
12 Clients (Parallel)
Total Training Time
1,733 s
425 s
283 s
196 s
Average Time Per Epoch
86.65 s


21.25 s
14.15 s
9.8 s


The speedup continues to hold with a larger amount of data. As the number of clients grows, there is a proportional speedup. The speedup ends up decreasing slightly over time, due to larger overhead, yet it still remains positive and high indicating a high level of parallelizability.














Model Parallelism Tests
  n_samples = 100;
  n_features = 2000;
  n_classes = 2;
  n_clients = 1;
  epochs = 20;



1 thread
(Sequential)
4 threads
(Parallel)
8 threads
(Parallel)
32 threads
(Parallel)
64 threads
(Parallel)
Total Training Time
409.188s
425.4s 
484s
534.108s
556.46s
Average Time Per Epoch
20.4594s
21.2737s
24.20s
26.7054s
27.823s


The model parallelism was done by creating threads for the forward propagation operations in each thread. The work that each thread did was calculated proportionally to the input size (i.e. the previous network’s layer). The thread then completed its operation and joined when completed. The overall results are not very promising. They show that the overhead overpowered all the speedup. The total time ended up growing by a small rate as the number of threads increased. All of these tests were done on one machine, meaning message passing had minimal effect on the actual dataset.

Overall Analysis & Conclusion
The distribution and data parallelism was very successful. We tried to model parallelism by multithreading the tasks within a single layer with threads. Unfortunately, model parallelism did not prove to be successful. Any speedup gained by parallelizing work was lost in the increased overhead that was used. This is likely the result of the high level of communication. 
Nonetheless, the project is an overall success with a clear and very significant speedup using data parallelization, which is especially significant for large datasets. These are the datasets most common for neural networks. The large speedup with data parallelism will be particularly valuable for datasets with a large number of records. There is a drawback in terms of memory available on the machine, as messages passed are very large, however, the tradeoff between memory and time is usually worth it with neural networks due to their long runtimes and need for consistent iteration.

The reason data parallelism works is because the amount of samples per epoch each computer does is reduced. Each worker takes 1/n number of samples to run an epoch on. Each of these 1/n work segments is then calculated in parallel as they are being run on different machines. The master then averages these epochs which adds less overhead than if there was a single computer iterating through all the samples every epoch.

Another major success was fault tolerance being implemented to cover message drops from workers to master and master to workers. This was a major issue in the first working implementation, with us unable to run an entire dataset the majority of the time due to dropped messages. While it did add some overhead, the results show it is insignificant compared to the speedup, and serves to make the system more robust and viable.
