#include "RHI.h"
#include "OpenGL/OpenGLHeader.h"
#include "StbImageHeader.h"

namespace Bloodshot
{
	namespace Private::Rendering::RenderHardware
	{
		static inline GLFWwindow* Window = nullptr;
	}

	void FRHI::StaticConstructor()
	{
		BS_LOG(Debug, "Creating FRHI...");

		const int GLFWInitResult = glfwInit();
		BS_ASSERT(GLFWInitResult, "FRHI::StaticConstructor: Failed to init GLFW");

		GLFWwindow*& Window = Private::Rendering::RenderHardware::Window;
		FRHI& Instance = GetInstance();
		glm::uvec2& WindowSize = Instance.WindowSize;
		const glm::vec4& BackgroundColor = Instance.BackgroundColor;

		Window = glfwCreateWindow(WindowSize.x, WindowSize.y, Instance.WindowName, nullptr, nullptr);
		BS_ASSERT(Window, "FRHI::StaticConstructor: Failed to create GLFW Window");

		glfwSetWindowUserPointer(Window, &WindowSize);
		glfwSetWindowSizeCallback(Window, [](GLFWwindow* const Window, const int Width, const int Height)
		{
			*reinterpret_cast<glm::uvec2*>(glfwGetWindowUserPointer(Window)) = {Width, Height};
			glViewport(0, 0, Width, Height);
		});

		glfwMakeContextCurrent(Window);
		glfwSwapInterval(Instance.bVSyncEnabled);

		const int GladLoadGLResult = gladLoadGL();
		BS_ASSERT(GladLoadGLResult, "FRHI::StaticConstructor: Failed to load GLAD");
		BS_LOG(Info, "OpenGL {}.{} has been loaded", GLVersion.major, GLVersion.minor);

		glClearColor((GLfloat)BackgroundColor.r, (GLfloat)BackgroundColor.g, (GLfloat)BackgroundColor.b, (GLfloat)BackgroundColor.a);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);

		glEnable(GL_CULL_FACE);
		glCullFace(GL_FRONT);
		glFrontFace(GL_CW);

		glEnable(GL_REPEAT);
	}

	void FRHI::StaticDestructor()
	{
		BS_LOG(Debug, "Destroying FRHI...");
		glfwTerminate();
	}

	FRHI& FRHI::GetInstance()
	{
		static FRHI Instance;
		return Instance;
	}

	GLFWwindow* FRHI::GetNativeWindow()
	{
		return Private::Rendering::RenderHardware::Window;
	}

	void FRHI::SetNativeWindow(GLFWwindow* const InNativeWindow)
	{
		GLFWwindow*& Window = Private::Rendering::RenderHardware::Window;
		Window = InNativeWindow;
		glfwMakeContextCurrent(Window);
	}

	bool FRHI::IsExitRequested()
	{
		return glfwWindowShouldClose(Private::Rendering::RenderHardware::Window);
	}

	void FRHI::SetVSync(const bool bInVSyncEnabled)
	{
		GetInstance().bVSyncEnabled = bInVSyncEnabled;
		glfwSwapInterval(bInVSyncEnabled);
	}

	int FRHI::GetKeyState(const int KeyCode)
	{
		return glfwGetKey(Private::Rendering::RenderHardware::Window, KeyCode);
	}

	int FRHI::GetMouseButtonState(const int MouseButton)
	{
		return glfwGetMouseButton(Private::Rendering::RenderHardware::Window, MouseButton);
	}

	void FRHI::GetMousePosition(double& OutPosX, double& OutPosY)
	{
		glfwGetCursorPos(Private::Rendering::RenderHardware::Window, &OutPosX, &OutPosY);
	}

	void FRHI::SetMousePosition(const double PosX, const double PosY)
	{
		glfwSetCursorPos(Private::Rendering::RenderHardware::Window, PosX, PosY);
	}

	void FRHI::SetMouseVisibility(const bool bVisible)
	{
		glfwSetInputMode(Private::Rendering::RenderHardware::Window, 
			GLFW_CURSOR, 
			bVisible ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_HIDDEN);
	}

	void FRHI::SwapBuffers()
	{
		glfwSwapBuffers(Private::Rendering::RenderHardware::Window);
	}

	void FRHI::PollEvents()
	{
		glfwPollEvents();
	}

	void FRHI::RequestExit()
	{
		glfwSetWindowShouldClose(Private::Rendering::RenderHardware::Window, GL_TRUE);
	}

	void FRHI::DrawTriangles(const FVertexArray* const InVertexArray)
	{
		InVertexArray->Bind();
		if (const FIndexBuffer* const IndexBuffer = InVertexArray->GetIndexBuffer())
		{
			glDrawElements(GL_TRIANGLES, (GLsizei)IndexBuffer->GetIndexCount(), GL_UNSIGNED_INT, nullptr);
		}
		else
		{
			glDrawArrays(GL_TRIANGLES, 0, InVertexArray->GetVertexCount());
		}
		InVertexArray->Unbind();
	}

	void FRHI::DrawSourceMesh(const FVertexArray* const InVertexArray, const FSourceMeshDescription& SourceMesh)
	{
		InVertexArray->Bind();
		glDrawElementsBaseVertex(GL_TRIANGLES,
			SourceMesh.IndexCount,
			GL_UNSIGNED_INT,
			(const void*)(sizeof(SourceMesh.StartIndex) * SourceMesh.StartIndex),
			SourceMesh.StartVertex);
		InVertexArray->Unbind();
	}

	void FRHI::ClearBackground()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	FShader* FRHI::CreateShaderFromFile(FStringView InName, FStringView InVertexShaderPath, FStringView InFragmentShaderPath)
	{
		// BSTODO: Reimplement

		std::ifstream InputStream;

		InputStream.open(InVertexShaderPath.GetData());
		BS_CHECK(InputStream.is_open());
		std::stringstream VertexShaderSource;
		VertexShaderSource << InputStream.rdbuf();
		const FString& VSC = VertexShaderSource.str().c_str();
		InputStream.close();

		InputStream.open(InFragmentShaderPath.GetData());
		BS_CHECK(InputStream.is_open());
		std::stringstream FragmentShaderSource;
		FragmentShaderSource << InputStream.rdbuf();
		const FString& FSC = FragmentShaderSource.str().c_str();
		InputStream.close();

		return CreateShader(InName, VSC.GetData(), FSC.GetData());
	}

	FTexture* FRHI::CreateTexture(FStringView InPath, const bool bInFlipped)
	{
		stbi_set_flip_vertically_on_load(bInFlipped);

		int Width;
		int Height;
		int Channels;

		void* const Data = stbi_load(InPath.GetData(),
			&Width,
			&Height,
			&Channels,
			STBI_rgb_alpha);

		if (!Data)
		{
			BS_LOG(Error, "Failed to load texture: {}", InPath.IsEmpty() ? "???" : InPath);
			return nullptr;
		}

		void* const Memory = GetInstance().TextureAllocator.Allocate();
		FTexture* const Texture = new(Memory) FTexture((uint32_t)Width,
			(uint32_t)Height,
			static_cast<EColorChannels>(Channels),
			Data);

		stbi_image_free(Data);
		stbi_set_flip_vertically_on_load(false);

		return Texture;
	}

	FTexture* FRHI::CreateTextureFromMemory(const void* const InData, const uint32_t InDataSize)
	{
		int Width;
		int Height;
		int Channels;

		void* const Data = stbi_load_from_memory((const stbi_uc*)InData,
			(int)InDataSize,
			&Width,
			&Height,
			&Channels,
			0);

		if (!Data)
		{
			BS_LOG(Error, "Failed to load texture from memory: {}, {}", InData, InDataSize);
			return nullptr;
		}

		void* const Memory = GetInstance().TextureAllocator.Allocate();
		FTexture* const Texture = new(Memory) FTexture((uint32_t)Width,
			(uint32_t)Height,
			static_cast<EColorChannels>(Channels),
			Data);

		stbi_image_free(Data);

		return Texture;
	}
}
