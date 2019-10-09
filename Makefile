FLAGS=-lncurses -lpthread -std=c++11

all: model

model: model_mainloop.cpp oo_model.cpp oo_model.hpp
	g++ -ogame model_mainloop.cpp oo_model.cpp $(FLAGS)
