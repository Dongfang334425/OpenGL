#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

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

//change the mixValue of the texture.
float mixValue = 0.2f;

int main()
{
	//relative path of current file.
	string path = "./Transformations/";

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
	Shader ourShader((path + "shader.vs").c_str(), (path + "shader.fs").c_str());

	


	//set up vertex data (and buffer(s)) and configure vertex attributes
	//------------------------------------------------------------------
	float vertices[]=
	{
		// positions          // colors           // texture coords
	0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // top right
	0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // bottom right
   -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // bottom left
   -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f    // top left 
	};
	unsigned int indices[] =
	{
		0,1,3,
		1,2,3
	};

	unsigned int VBO, VAO,EBO;/*EBO is a buffer, just like a vertex buffer object, that stores 
							  indices that OpenGL uses to decide what vertices to draw. This so called
							  indexed drawing.
							  */
	glGenVertexArrays(1, &VAO);//VAO: vertex array object
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	//Bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure
	//vertex attributes(s);
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// position attribute
	/*DF:  glVertexAttribPointer:
	first argument: the attribute number.
	second argument: the number of the attribute. 3 values in vertices, hence 3 here.
	third argument: the type of the attribute
	forth argument: unknown yet
	5th argument: total size of attributes: 3+3+2
	6th argument: the start point of the attribute. 0 for first, 3 for second, 6 for third in this case.
	*/
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// color attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	//texture attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));/*second argument: 2 elements.*/
	glEnableVertexAttribArray(2);
	// note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
	
	
	//load and create a texture
	unsigned int texture1, texture2;
	//---texture1---
	glGenTextures(1, &texture1);/*first argument:how many textures we want to generate and stores them in a unsigned int array
							   given as its second argument*/
	glBindTexture(GL_TEXTURE_2D, texture1);
	//set the texture wrapping parameters (on the currently bound texture object)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	//set the texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//load image, create texture and generate mipmaps
	int width, height, nrChannels;
	unsigned char *data = stbi_load((path+"container.jpg").c_str(), &width, &height, &nrChannels, 0);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		cout << "Failed to load texture" << endl;
	}
	stbi_image_free(data);
	
	//---texture2---
	glGenTextures(1, &texture2);/*first argument:how many textures we want to generate and stores them in a unsigned int array
							   given as its second argument*/
	glBindTexture(GL_TEXTURE_2D, texture2);
	//set the texture wrapping parameters (on the currently bound texture object)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	//set the texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	stbi_set_flip_vertically_on_load(true);
	data = stbi_load((path + "awesomeface.png").c_str(), &width, &height, &nrChannels, 0);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		/*DF: .png image includes an alpha (transparency) channel. It means that we now need to specify that
		the image data contains an alpha channel as well by using GL_RGBA.*/
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		cout << "Failed to load texture" << endl;
	}
	stbi_image_free(data);

	//tell opengl for each sampler to which texture unit it belongs to (only has to be done once)
	ourShader.use();//don't forget to activate/use the shader before setting uniforms!
	//either set it manually like so:
	ourShader.setInt("texture1", 0);
	ourShader.setInt("texture2", 1);


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
		
		//bind Textures on corresponding texture units
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture1);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texture2);

		ourShader.use();
		ourShader.setFloat("horizonOffset", 0.0f);//!!!:set 0.5f as the offset to reach the right side of the screen.
		ourShader.setFloat("mixValue", mixValue);
		

		//transformation
		glm::mat4 trans = glm::mat4(1.0f);
		//trans = glm::rotate(trans, glm::radians(90.0f), glm::vec3(0.0, 0.0, 1.0));//rotate 90 degrees.
		trans = glm::rotate(trans, (float)glfwGetTime(), glm::vec3(0.0, 0.0, 1.0));//rotate overtime
		trans = glm::scale(trans, glm::vec3(0.5, 0.5, 0.5));
		//first argument: get the location,
		//second argument: how many matrices we'd like to send, which is 1.
		//third argument: if we want to transpose our matrix.
		//forth argument: the actual matrix data. but GLM stores their matrices' data in a way that doesn't always match OpenGL's 
		//expectations so we first convert the data with GLM's built-in function value_ptr;
		//glUniformMatrix4fv(glGetUniformLocation(ourShader.ID, "transform"), 1, GL_FALSE, glm::value_ptr(trans));
		ourShader.setMat4("transform", trans);

		glBindVertexArray(VAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
		//glDrawArrays(GL_TRIANGLES, 0, 3);
		
		

		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);/*6 means that we want to draw 6 vertices in total
															GL_UNSIGNED_INT is the type of the indices which is of type GL_UNSIGNED_INT.
															The last argument allows us to specify an offset in the EBO.*/



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
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	ourShader.deleteShaderProgram();

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
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
	{
		mixValue += 0.001f;
		if (mixValue >= 1.0f)
			mixValue = 1.0f;
	}
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
	{
		mixValue -= 0.001f;
		if (mixValue <= 0.0f)
			mixValue = 0.0f;
	}
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