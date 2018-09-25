SRC   = main.cpp imgui_impl_sdl.cpp imgui_impl_opengl3.cpp imgui.cpp imgui_demo.cpp imgui_draw.cpp imgui_widgets.cpp ./GL/gl3w.c
OBJS  = $(addsuffix .o, $(basename $(notdir $(SRC))))
LIBS  = -lGL -ldl `sdl2-config --libs`
FLAGS = -I. `sdl2-config --cflags` -Wall -Wformat

medbench: $(OBJS)
	$(CXX) -o $@ $^ $(FLAGS) $(LIBS)

%.o:%.cpp
	$(CXX) $(FLAGS) -c -o $@ $<

%.o:./GL/%.c
	$(CC) $(FLAGS) -c -o $@ $<

clean:
	rm -f medbench $(OBJS)
