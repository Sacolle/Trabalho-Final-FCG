CXX = g++
FLAG_LIBS = -lgdi32 -lopengl32
CPPFLAGS = -std=c++11 -Wall -Wno-unused-function -g -static-libstdc++
INCLUDE = -I./include/

OBJDIR = src/obj
LIBSDIR = src/libs
SRCDIR = src
INCLUDEDIR = include

SRC = $(wildcard $(SRCDIR)/*.cpp)

_OBJS := $(notdir $(SRC))
_OBJS := $(patsubst %.cpp,%.o,$(_OBJS))    #convert to .o
OBJS  := $(addprefix $(OBJDIR)/, $(_OBJS))	#adds obj/ na frente

_STATIC_OBJS := $(wildcard $(LIBSDIR)/*)
_STATIC_OBJS := $(basename $(_STATIC_OBJS))
_STATIC_OBJS := $(notdir $(_STATIC_OBJS))
_STATIC_OBJS := $(addsuffix .o, $(_STATIC_OBJS))
STATIC_OBJS := $(addprefix $(OBJDIR)/, $(_STATIC_OBJS))


# $< nome do primeiro pre-requisito
# $@ é o nome da regra
# $^ é a string das dependencias, nesse caso $(OBJS)
bin/main: $(OBJS) $(STATIC_OBJS) ./lib/libglfw3.a
	$(CXX) -o $@ $^ $(CPPFLAGS) $(FLAG_LIBS) 

$(OBJDIR)/main.o : $(SRCDIR)/main.cpp $(wildcard ./include/*.hpp) SRC
	$(CXX) -c -o $@ $< $(CPPFLAGS) $(INCLUDE)

$(OBJDIR)/entities.o : $(addprefix $(SRCDIR)/, entities.cpp matrix.cpp shader.cpp collision.cpp) $(addprefix $(INCLUDEDIR)/,entities.hpp matrix.hpp shader.hpp collision.hpp)
	$(CXX) -c -o $@ $< $(CPPFLAGS) $(INCLUDE)

$(OBJDIR)/matrix.o : $(SRCDIR)/matrix.cpp $(addprefix $(INCLUDEDIR)/,matrix.hpp)
	$(CXX) -c -o $@ $< $(CPPFLAGS) $(INCLUDE)

$(OBJDIR)/shader.o : $(SRCDIR)/shader.cpp $(addprefix $(INCLUDEDIR)/,shader.hpp)
	$(CXX) -c -o $@ $< $(CPPFLAGS) $(INCLUDE)

$(OBJDIR)/mesh.o : $(SRCDIR)/mesh.cpp $(addprefix $(INCLUDEDIR)/,mesh.hpp)
	$(CXX) -c -o $@ $< $(CPPFLAGS) $(INCLUDE)

$(OBJDIR)/collision.o : $(SRCDIR)/collision.cpp $(SRCDIR)/entities.cpp $(addprefix $(INCLUDEDIR)/,collision.hpp entities.hpp)
	$(CXX) -c -o $@ $< $(CPPFLAGS) $(INCLUDE)

#builds the src files versão generica e unreliable
#$(OBJDIR)/%.o: $(SRCDIR)/%.cpp $(INCLUDEDIR)/%.hpp
#	$(CXX) -c -o $@ $< $(INCLUDE) $(CPPFLAGS) 

#builds the libs
#builds imgui 
$(OBJDIR)/%.o: $(LIBSDIR)/%.cpp
	$(CXX) -c -o $@ $^ -I./include/imgui $(INCLUDE)
#builds glad.c
$(OBJDIR)/glad.o: $(LIBSDIR)/glad.c
	$(CXX) -c -o $@ $^ $(INCLUDE)

.PHONY: clean run
clean:
	rm -f src/obj/*.o
run: ./bin/main
	./bin/main src/shaders/model_vertex.glsl src/shaders/model_fragment.glsl