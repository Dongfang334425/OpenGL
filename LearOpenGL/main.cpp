#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

using namespace::std;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

/*DF: The vertex shader is one of the shaders that are programmable by people like us.
Modern OpenGL requires that we at least set up a vertex and fragment shader if we want 
to do some rendering.
The first thing we need to do is write the vertex shader in the shader language GLSL(OpenGL
Shading Language) and then compile this shader so we can use if in our application.

GLSL:
each shader begins with a declaration of its version.
next we declare all the input vertex attributes in the vertex shader with the "in" keyword.
Right now we only care about position data so we only need a single vertex attribute.
GLSL has a vector datatype that contains 1 to 4 floats based on its postfix digit. Since each
vertex has a 3D coordinate we create a "vec3" input variable with the name aPos. We also specifically
set the location of the input variable via layout (location = 0).
*/
const char *vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"void main()\n"
"{\n"
" gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
"}\0";

const char *fragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"	FragColor=vec4(1.0f,0.5f,0.2f,1.0f);\n"
"}\n\0";

int main()
{
	

	// glfw: initialize and configure
	// ------------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	// glfw window creation
	// --------------------
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);/*DF: tell GLFW to make the context of our window the main context on the current thread.*/
	
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);/*DF: tell OpenGL the size of the rendering window so OpenGL
	knows how we want to display the data and coordinates 
	with respect to the window.*/

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))/*DF: initialize GLAD before we call any OpenGL function.*/
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}


	//build and compile our shader program
	//------------------------------------
	//vertex shader
	int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	/*DF: glShaderSource function takes the shader object to compile to as its first argument.
	The second argument specifies how many strings we're passing as source code, which is only one.
	The third parameter is the actual source code of the vertex shader and we can leave the 4th parameter
	to NULL.*/
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);

	//check for shader compile errors
	int success;
	char infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << endl;
	}
	//fragment shader
	int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);
	//check for shader compile errors
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << endl;
	}
	//link shaders
	int shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << endl;
	}
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	//set up vertex data (and buffer(s)) and configure vertex attributes
	//------------------------------------------------------------------
	float vertices1[]=
	{
		0.5f,  0.5f, 0.0f,//left
		0.5f, -0.5f, 0.0f,//right
		-0.5f,  0.5f, 0.0f, //top
	};
	float vertices2[]=
	{
		0.5f, -0.5f, 0.0f,
		-0.5f, -0.5f, 0.0f,
		-0.5f,  0.5f, 0.0f
	};
	//float vertices[] = {
	//	// first triangle
	//	 0.5f,  0.5f, 0.0f,  // top right
	//	 0.5f, -0.5f, 0.0f,  // bottom right
	//	-0.5f,  0.5f, 0.0f,  // top left 
	//	// second triangle
	//	 0.5f, -0.5f, 0.0f,  // bottom right
	//	-0.5f, -0.5f, 0.0f,  // bottom left
	//	-0.5f,  0.5f, 0.0f   // top left
	//};

	//first triangle setup
	unsigned int VBO1, VAO1;
	glGenVertexArrays(1, &VAO1);/*DF: VAO: vertex array object???create memory on the GPU where we store the vertex data, 
							   configure how OpenGL should interpret the memory and specify how to send the data to the graphics card.*/
	glGenBuffers(1, &VBO1); /*DF: we manage this memory via VBO that can store a large number of vertices in the GPU's memory.*/
	//Bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure
	//vertex attributes(s);
	glBindVertexArray(VAO1);
	glBindBuffer(GL_ARRAY_BUFFER, VBO1);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices1), vertices1, GL_STATIC_DRAW);
	/*DF: 
	GL_STREAM_DRAW: the data is set only once and used by the GPU at most a few times
	GL_STATIC_DRAW: the data is set only once and used many times
	GL_DYNAMIC_DRAW: the data is changed a lot and used many times.*/
	/*DF: As of now, we stored the vertex data within memory on the graphics card as managed by a vertex buffer object named VBO.*/
	/*DF:set the vertex attributes pointers.*/
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	//second triangle setup
	unsigned int VBO2, VAO2;
	glGenVertexArrays(1, &VAO2);
	glGenBuffers(1, &VBO2);
	glBindVertexArray(VAO2);
	glBindBuffer(GL_ARRAY_BUFFER, VBO2);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices2), vertices2, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
	// VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
	glBindVertexArray(0);


	// render loop
	// -----------
	while (!glfwWindowShouldClose(window))
	{
		// input
		// -----
		processInput(window);

		//the graphics pipeline can be divided into several steps where each step requires the output of the previous
		//as its input.
		//All of these steps are highly specialized (they have one specific function) and can easily be executed 
		//in parallel.



		// render
		// ------
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);/*DF: clear the screen's color buffer using glClear where we pass in buffer bits to 
									 specify which buffer we would like to clear.*/
									 // draw our first triangle
		glUseProgram(shaderProgram);/*DF: it's used to activate shaderProgram.*/

		//draw first triangle
		glBindVertexArray(VAO1); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
		glDrawArrays(GL_TRIANGLES, 0, 3);/*DF: First argument: the OpenGL primitive we would like to draw.
										 Second argument: the starting index of the vertex array we'd like to draw
										 Third argument: how many vertices we want to draw. 3 means 1 triangle. 6 means 2 triangles.*/

		//draw second triangle
		glBindVertexArray(VAO2);
		glDrawArrays(GL_TRIANGLES, 0, 3);

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);/*DF: swap the color buffer (a large 2D buffer that contains color values for 
								each pixel in GLFW's window) that is used to render to during this render iteration and show 
								it as output to the screen.*/
		glfwPollEvents();/*DF: check if any events are triggered like keyboard input or mouse movement events.
						 updates the window state, and calls the corresponding functions.*/
	}
	// optional: de-allocate all resources once they've outlived their purpose:
	// ------------------------------------------------------------------------
	glDeleteVertexArrays(1, &VAO1);
	glDeleteBuffers(1, &VBO1);
	glDeleteVertexArrays(1, &VAO2);
	glDeleteBuffers(1, &VBO2);

	glDeleteProgram(shaderProgram);

	// glfw: terminate, clearing all previously allocated GLFW resources.
	// ------------------------------------------------------------------
	glfwTerminate();
	return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)/*DF: if it's not pressed, glfwGetKey returns GLFW_RELEASE*/
		glfwSetWindowShouldClose(window, true);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);/*DF: the first two parameters of glViewport set the location of the lower left
									corner of the window. The third and fourth parameter set the width and height of 
									the rendering window in pixels, which we set equal to GLFW's window size.*/
}