
all: simgame playgame review train createNN

test.o: test.cpp montecarlo.h
	g++ -g -Wall -pthread test.cpp -o test.o

test: test.o
	./test.o

simgame: simgame.cpp NN/neuralnet.o montecarlo.h Engine/engine.h
	g++ -g -Wall -O3 -pthread $< NN/neuralnet.o -o $@

playgame: playgame.cpp NN/neuralnet.o montecarlo.h Engine/engine.h
	g++ -g -Wall -O3 -pthread $< NN/neuralnet.o -o $@

review: review.cpp Engine/engine.h
	g++ -g -Wall -O3 $< -o $@

train: train.cpp NN/neuralnet.o
	g++ -g -Wall -O3 $^ -o $@

createNN: createNN.cpp NN/neuralnet.o
	g++ -g -Wall -O3 $^ -o $@

NN/neuralnet.o:
	$(MAKE) -C NN/

clean:
	rm -f *.o test simgame playgame review train createNN
