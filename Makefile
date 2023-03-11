CXX = g++
FLAG_LIBS = -lgdi32 -lopengl32
CPPFLAGS = -std=c++11 -Wall -Wno-unused-function -g -static-libstdc++
INCLUDE = -I./include/

# directories of the project
OBJDIR = obj
LIBSDIR = src/libs
SRCDIR = src
INCLUDEDIR = include

SRCFILES = collision.cpp gameloop.cpp \
camera.cpp entity.cpp geometry.cpp \
mesh.cpp renderable.cpp shader.cpp \
matrix.cpp

# os objs escritos a serem lincados
_OBJS := $(patsubst %.cpp,%.o,$(SRCFILES)) #convert to .o
OBJS  := $(addprefix $(OBJDIR)/, $(_OBJS))	#adds obj/ na frente

# os objs das libs a serem linkados
_STATIC_OBJS := $(wildcard $(LIBSDIR)/*) #gets the files
_STATIC_OBJS := $(notdir $(_STATIC_OBJS)) #removes the dir
_STATIC_OBJS := $(patsubst %.cpp,%.o, $(_STATIC_OBJS)) #changes the cpp to o
STATIC_OBJS := $(addprefix $(OBJDIR)/, $(_STATIC_OBJS)) #adds the objdir


# $< nome do primeiro pre-requisito
# $@ é o nome da regra
# $^ é a string das dependencias, nesse caso $(OBJS)
bin/main: $(OBJS) $(STATIC_OBJS) ./lib/libglfw3.a
	$(CXX) -o $@ $^ $(CPPFLAGS) $(FLAG_LIBS) 

#regras de compilação de cada arquivo do projeto
MAIN_DEPENDS := \
	renders/shader.hpp \
	renders/mesh.hpp \
	utils/matrix.hpp \
	entities/geometry.hpp \
	controlers/collision.hpp
$(OBJDIR)/main.o : $(SRCDIR)/main.cpp $(addprefix $(SRCDIR)/, $(MAIN_DEPENDS))
	$(CXX) -c -o $@ $< $(CPPFLAGS) $(INCLUDE)

#controlers
COLLISION_DEPENDS := \
	controlers/collision.hpp \
	entities/entity.hpp
$(OBJDIR)/collision.o : $(SRCDIR)/controlers/collision.cpp $(addprefix $(SRCDIR)/, $(COLLISION_DEPENDS))
	$(CXX) -c -o $@ $< $(CPPFLAGS) $(INCLUDE)

GAMELOOP_DEPENDS := controlers/gameloop.hpp
$(OBJDIR)/gameloop.o : $(SRCDIR)/controlers/gameloop.cpp $(addprefix $(SRCDIR)/, $(GAMELOOP_DEPENDS))
	$(CXX) -c -o $@ $< $(CPPFLAGS) $(INCLUDE)

#entities
CAMERA_DEPENDS := \
	entities/camera.hpp \
	utils/matrix.hpp
$(OBJDIR)/camera.o : $(SRCDIR)/entities/camera.cpp $(addprefix $(SRCDIR)/, $(CAMERA_DEPENDS))
	$(CXX) -c -o $@ $< $(CPPFLAGS) $(INCLUDE)

ENTITY_DEPENDS := \
	entities/entity.hpp \
	entities/geometry.hpp \
	renders/renderable.hpp
$(OBJDIR)/entity.o : $(SRCDIR)/entities/entity.cpp $(addprefix $(SRCDIR)/, $(ENTITY_DEPENDS))
	$(CXX) -c -o $@ $< $(CPPFLAGS) $(INCLUDE)

GEOMETRY_DEPENDS := \
	entities/geometry.hpp \
	utils/matrix.hpp
$(OBJDIR)/geometry.o : $(SRCDIR)/entities/geometry.cpp $(addprefix $(SRCDIR)/,$(GEOMETRY_DEPENDS))	
	$(CXX) -c -o $@ $< $(CPPFLAGS) $(INCLUDE)

#renders
MESH_DEPENDS := renders/mesh.hpp
$(OBJDIR)/mesh.o : $(SRCDIR)/renders/mesh.cpp $(addprefix $(SRCDIR)/, $(MESH_DEPENDS))
	$(CXX) -c -o $@ $< $(CPPFLAGS) $(INCLUDE)

SHADER_DEPENDS := renders/shader.hpp
$(OBJDIR)/shader.o : $(SRCDIR)/renders/shader.cpp $(addprefix $(SRCDIR)/, $(SHADER_DEPENDS))
	$(CXX) -c -o $@ $< $(CPPFLAGS) $(INCLUDE)

RENDERABLE_DEPENDS := \
	renders/renderable.hpp \
	renders/mesh.hpp \
	renders/shader.hpp
$(OBJDIR)/renderable.o : $(SRCDIR)/renders/renderable.cpp $(addprefix $(SRCDIR)/, $(RENDERABLE_DEPENDS))
	$(CXX) -c -o $@ $< $(CPPFLAGS) $(INCLUDE)

#utils
MATRIX_DEPENDS := utils/matrix.hpp
$(OBJDIR)/matrix.o : $(SRCDIR)/utils/matrix.cpp $(addprefix $(SRCDIR)/, $(MATRIX_DEPENDS))
	$(CXX) -c -o $@ $< $(CPPFLAGS) $(INCLUDE)



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