   // load video module
   SDL_Init(SDL_INIT_VIDEO);

   // enable forward compatibility
   SDL_GL_SetAttribute(
         SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
   SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
   SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);

   // create a window
   SDL_Window* window = SDL_CreateWindow(
         "OpenGL", 100, 100, 800, 600, SDL_WINDOW_OPENGL);

   // create the context
   SDL_GLContext context = SDL_GL_CreateContext(window);

   // dynamically link with graphics card functions
   glewExperimental = GL_TRUE;
   glewInit();


   float vertices[] = {
      0.0f,  0.5f, // Vertex 1 (X, Y)
      0.5f, -0.5f, // Vertex 2 (X, Y)
     -0.5f, -0.5f  // Vertex 3 (X, Y)
   };

   GLuint vbo;
   glGenBuffers(1, &vbo); // Generate 1 buffer

   // make vbo the active buffer
   glBindBuffer(GL_ARRAY_BUFFER, vbo);

   // copy the vertex data to vbo
   glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

   // compile vertex shader
   string content;
   e = tools::read_file("src/vertex_shader.glsl", content);
   if (e != NULL) {
      cout << prog_name << "::" << e;
      cout << "src/vertex_shader.glsl:\n\n" << content << endl;
      return 1;
   }
   char * vertexSource = new char[content.size()];
   for (int i = 0; i < content.size(); ++i)
      vertexSource[i] = content.c_str()[i];
   GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
   glShaderSource(vertexShader, 1, &vertexSource, NULL);
   glCompileShader(vertexShader);
   GLint status;
   glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &status);
   if (status != GL_TRUE) {
      char buffer[512];
      glGetShaderInfoLog(vertexShader, 512, NULL, buffer);
      cout << prog_name << ": vertex shader did not compile! ";
      cout << buffer << endl;
      return 1;
   }

   // compile fragment shader
   e = tools::read_file("src/fragment_shader.glsl", content);
   if (e != NULL) {
      cout << prog_name << "::" << e;
      return 1;
   }
   char * fragmentSource = new char[content.size()];
   for (int i = 0; i < content.size(); ++i)
      fragmentSource[i] = content.c_str()[i];
   GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
   glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
   glCompileShader(fragmentShader);
   glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &status);
   if (status != GL_TRUE) {
      char buffer[512];
      glGetShaderInfoLog(fragmentShader, 512, NULL, buffer);
      cout << prog_name << ": fragment shader did not compile! ";
      cout << buffer << endl;
      return 1;
   }

   // combine the shaders into a program
   GLuint shaderProgram = glCreateProgram();
   glAttachShader(shaderProgram, vertexShader);
   glAttachShader(shaderProgram, fragmentShader);
   // specify which output is written to which buffer
   glBindFragDataLocation(shaderProgram, 0, "outColor");
   glLinkProgram(shaderProgram);
   glUseProgram(shaderProgram);

   // link the vertex data and the attributes
   GLint posAttrib = glGetAttribLocation(shaderProgram, "position");
   glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 0, 0);
   glEnableVertexAttribArray(posAttrib);

   // this stores all the links between the attributes and the VBOs
   GLuint vao;
   glGenVertexArrays(1, &vao);
   glBindVertexArray(vao);

   SDL_Event windowEvent;
   while (true) {
      if (SDL_PollEvent(&windowEvent)) {
         if (windowEvent.type == SDL_QUIT) break;
         if (windowEvent.type == SDL_KEYUP &&
               windowEvent.key.keysym.sym == SDLK_ESCAPE) break;
      }

      glDrawArrays(GL_TRIANGLES, 0, 3);

      SDL_GL_SwapWindow(window);
   }

   glDeleteProgram(shaderProgram);
   glDeleteShader(fragmentShader);
   glDeleteShader(vertexShader);
   glDeleteBuffers(1, &vbo);
   glDeleteVertexArrays(1, &vao);

   SDL_GL_DeleteContext(context);
   SDL_Quit();
   return 0;
