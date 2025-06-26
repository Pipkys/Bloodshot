#pragma once

#include "Core.h"

#include "FrameBuffer.h"
#include "IndexBuffer.h"
#include "Shader.h"
#include "SourceMeshDescription.h"
#include "Texture.h"
#include "UniformBuffer.h"
#include "Vertex.h"
#include "VertexArray.h"
#include "VertexBuffer.h"
#include "VertexBufferLayout.h"

#include <chrono>

struct GLFWwindow;

namespace Bloodshot
{
	enum class ERendererType : uint8_t
	{
		OpenGL = 0,
		Vulkan,
	};

	struct FRenderResourceAllocatorsStats final
	{
		FPoolAllocatorStats VertexArrayAllocatorStats;
		FPoolAllocatorStats VertexBufferAllocatorStats;
		FPoolAllocatorStats IndexBufferAllocatorStats;
		FPoolAllocatorStats UniformBufferAllocatorStats;
		FPoolAllocatorStats FrameBufferAllocatorStats;
		FPoolAllocatorStats ShaderAllocatorStats;
		FPoolAllocatorStats TextureAllocatorStats;
	};

	class FRHI final
	{
		friend class Private::Launch::IEngineContext;
		friend class Private::Launch::FEngineEditorContext;
		friend class Private::Launch::FEngineGameContext;

	public:
		NODISCARD static FRHI& GetInstance();

		NODISCARD FORCEINLINE static uint32_t GetWindowWidth()
		{
			return GetInstance().WindowSize.x;
		}

		NODISCARD FORCEINLINE static uint32_t GetWindowHeight()
		{
			return GetInstance().WindowSize.y;
		}

		NODISCARD static GLFWwindow* GetNativeWindow();
		NODISCARD static void SetNativeWindow(GLFWwindow* const InNativeWindow);

		NODISCARD FORCEINLINE static float GetFrameTime()
		{
			return GetInstance().FrameTimeInMs;
		}

		NODISCARD static bool IsExitRequested();
		static void SetVSync(const bool bInVSyncEnabled);

		NODISCARD static int GetKeyState(const int KeyCode);
		NODISCARD static int GetMouseButtonState(const int MouseButton);
		NODISCARD static void GetMousePosition(double& OutPosX, double& OutPosY);

		static void SetMousePosition(const double PosX, const double PosY);
		static void SetMouseVisibility(const bool bVisible);

	private:
		FRHI() {}

		const char* WindowName = "Bloodshot Game Engine";
		glm::uvec2 WindowSize = glm::uvec2(1024, 768);
		bool bVSyncEnabled = false;

		std::chrono::high_resolution_clock::time_point FrameBeginTimepoint;
		float FrameTimeInMs = 0.f;

		static void StaticConstructor();
		static void StaticDestructor();

		static void SwapBuffers();
		static void PollEvents();
		static void RequestExit();

		FORCEINLINE static void BeginFrame()
		{
			GetInstance().FrameBeginTimepoint = std::chrono::high_resolution_clock::now();
		}

		FORCEINLINE static void EndFrame()
		{
			FRHI& Instance = GetInstance();
			std::chrono::duration<float> FrameTimeInSec = std::chrono::high_resolution_clock::now() - Instance.FrameBeginTimepoint;
			Instance.FrameTimeInMs = FrameTimeInSec.count() * 1000.f;
		}

	public:
		NODISCARD FORCEINLINE static ERendererType GetType()
		{
			return GetInstance().RendererType;
		}

		NODISCARD FORCEINLINE static const glm::vec4& GetBackgroundColor()
		{
			return GetInstance().BackgroundColor;
		}

		NODISCARD FORCEINLINE static void SetBackgroundColor(const glm::vec4& NewColor)
		{
			GetInstance().BackgroundColor = NewColor;
		}

		static void DrawTriangles(const FVertexArray* const InVertexArray);
		static void DrawSourceMesh(const FVertexArray* const InVertexArray, const FSourceMeshDescription& SourceMesh);

		// BSTODO: Implement ?
		//void DrawLines();

		static void ClearBackground();

	private:
		ERendererType RendererType = ERendererType::OpenGL;
		glm::vec4 BackgroundColor = glm::vec4(0.5f, 0.5f, 0.5f, 1.f);

	public:
		NODISCARD FORCEINLINE static FVertexArray* CreateVertexArray()
		{
			void* const Memory = GetInstance().VertexArrayAllocator.Allocate();
			FVertexArray* const VertexArray = new(Memory) FVertexArray();
			return VertexArray;
		}

		NODISCARD FORCEINLINE static FVertexBuffer* CreateVertexBuffer(const TArray<FVertex>& InVertices, const FVertexBufferLayout& InLayout)
		{
			void* const Memory = GetInstance().VertexBufferAllocator.Allocate();
			const void* const Vertices = InVertices.GetData();
			const size_t VertexCount = InVertices.GetSize();
			const size_t VerticesSize = VertexCount * sizeof(FVertex);
			FVertexBuffer* const VertexBuffer = new(Memory) FVertexBuffer(Vertices, (uint32_t)VertexCount, VerticesSize, InLayout);
			return VertexBuffer;
		}

		NODISCARD FORCEINLINE static FVertexBuffer* CreateVertexBuffer(const TArray<FVertex>& InVertices, FVertexBufferLayout&& InLayout)
		{
			void* const Memory = GetInstance().VertexBufferAllocator.Allocate();
			const void* const Vertices = InVertices.GetData();
			const size_t VertexCount = InVertices.GetSize();
			const size_t VerticesSize = VertexCount * sizeof(FVertex);
			FVertexBuffer* const VertexBuffer = new(Memory) FVertexBuffer(Vertices, (uint32_t)VertexCount, VerticesSize, std::move(InLayout));
			return VertexBuffer;
		}

		NODISCARD FORCEINLINE static FIndexBuffer* CreateIndexBuffer(const TArray<uint32_t>& InIndices)
		{
			void* const Memory = GetInstance().IndexBufferAllocator.Allocate();
			const void* const Indices = InIndices.GetData();
			const size_t IndexCount = InIndices.GetSize();
			FIndexBuffer* const IndexBuffer = new(Memory) FIndexBuffer(Indices, (uint32_t)IndexCount);
			return IndexBuffer;
		}

		NODISCARD FORCEINLINE static FUniformBuffer* CreateUniformBuffer(const size_t InSize, const uint32_t InBinding)
		{
			void* const Memory = GetInstance().UniformBufferAllocator.Allocate();
			FUniformBuffer* const UniformBuffer = new(Memory) FUniformBuffer(InSize, InBinding);
			return UniformBuffer;
		}

		NODISCARD FORCEINLINE static FUniformBuffer* CreateUniformBuffer(const size_t InSize,
			const uint32_t InBinding,
			const void* const InData,
			const uint32_t InOffset = 0)
		{
			void* const Memory = GetInstance().UniformBufferAllocator.Allocate();
			FUniformBuffer* const UniformBuffer = new(Memory) FUniformBuffer(InSize, InBinding, InData, InOffset);
			return UniformBuffer;
		}

		NODISCARD FORCEINLINE static FFrameBuffer* CreateFrameBuffer(const FFrameBufferSettings& InSettings)
		{
			void* const Memory = GetInstance().FrameBufferAllocator.Allocate();
			FFrameBuffer* const FrameBuffer = new(Memory) FFrameBuffer(InSettings);
			return FrameBuffer;
		}

		FORCEINLINE static FShader* CreateShader(FStringView InName, FStringView InVertexShaderSrc, FStringView InFragmentShaderSrc)
		{
			void* const Memory = GetInstance().ShaderAllocator.Allocate();
			FShader* const Shader = new(Memory) FShader(InName, InVertexShaderSrc, InFragmentShaderSrc);
			return Shader;
		}

		static FShader* CreateShaderFromFile(FStringView InName, FStringView InVertexShaderPath, FStringView InFragmentShaderPath);

		NODISCARD static FTexture* CreateTexture(FStringView InPath, const bool bInFlipped = false);
		NODISCARD static FTexture* CreateTextureFromMemory(const void* const InData, const uint32_t InDataSize);

		FORCEINLINE static void DestroyVertexArray(FVertexArray* const InVertexArray)
		{
			if (InVertexArray)
			{
				DestroyIndexBuffer(InVertexArray->GetIndexBuffer());
				for (FVertexBuffer* const VertexBuffer : InVertexArray->GetVertexBuffers())
				{
					DestroyVertexBuffer(VertexBuffer);
				}
				InVertexArray->~FVertexArray();
				GetInstance().VertexArrayAllocator.Deallocate(InVertexArray);
			}
		}

		FORCEINLINE static void DestroyVertexBuffer(FVertexBuffer* const InVertexBuffer)
		{
			if (InVertexBuffer)
			{
				InVertexBuffer->~FVertexBuffer();
				GetInstance().VertexBufferAllocator.Deallocate(InVertexBuffer);
			}
		}

		FORCEINLINE static void DestroyIndexBuffer(FIndexBuffer* const InIndexBuffer)
		{
			if (InIndexBuffer)
			{
				InIndexBuffer->~FIndexBuffer();
				GetInstance().IndexBufferAllocator.Deallocate(InIndexBuffer);
			}
		}

		FORCEINLINE static void DestroyUniformBuffer(FUniformBuffer* const InUniformBuffer)
		{
			if (InUniformBuffer)
			{
				InUniformBuffer->~FUniformBuffer();
				GetInstance().UniformBufferAllocator.Deallocate(InUniformBuffer);
			}
		}

		FORCEINLINE static void DestroyFrameBuffer(FFrameBuffer* const InFrameBuffer)
		{
			if (InFrameBuffer)
			{
				InFrameBuffer->~FFrameBuffer();
				GetInstance().FrameBufferAllocator.Deallocate(InFrameBuffer);
			}
		}

		FORCEINLINE static void DestroyShader(FShader* const InShader)
		{
			if (InShader)
			{
				InShader->~FShader();
				GetInstance().ShaderAllocator.Deallocate(InShader);
			}
		}

		FORCEINLINE static void DestroyTexture(FTexture* const InTexture)
		{
			if (InTexture)
			{
				InTexture->~FTexture();
				GetInstance().TextureAllocator.Deallocate(InTexture);
			}
		}

	private:
		template<typename ResourceType>
		using TRenderResourceAllocator = TPoolAllocator<512>::ForElementType<ResourceType>;

		TRenderResourceAllocator<FVertexArray> VertexArrayAllocator;
		TRenderResourceAllocator<FVertexBuffer> VertexBufferAllocator;
		TRenderResourceAllocator<FIndexBuffer> IndexBufferAllocator;
		TRenderResourceAllocator<FUniformBuffer> UniformBufferAllocator;
		TRenderResourceAllocator<FFrameBuffer> FrameBufferAllocator;
		TRenderResourceAllocator<FShader> ShaderAllocator;
		TRenderResourceAllocator<FTexture> TextureAllocator;

	public:
		NODISCARD FORCEINLINE static const FPoolAllocatorStats& GetVertexArrayAllocatorStats()
		{
			return GetInstance().VertexArrayAllocator.GetStats();
		}

		NODISCARD FORCEINLINE static const FPoolAllocatorStats& GetVertexBufferAllocatorStats()
		{
			return GetInstance().VertexBufferAllocator.GetStats();
		}

		NODISCARD FORCEINLINE static const FPoolAllocatorStats& GetIndexBufferAllocatorStats()
		{
			return GetInstance().IndexBufferAllocator.GetStats();
		}

		NODISCARD FORCEINLINE static const FPoolAllocatorStats& GetUniformBufferAllocatorStats()
		{
			return GetInstance().UniformBufferAllocator.GetStats();
		}

		NODISCARD FORCEINLINE static const FPoolAllocatorStats& GetFrameBufferAllocatorStats()
		{
			return GetInstance().FrameBufferAllocator.GetStats();
		}

		NODISCARD FORCEINLINE static const FPoolAllocatorStats& GetShaderAllocatorStats()
		{
			return GetInstance().ShaderAllocator.GetStats();
		}

		NODISCARD FORCEINLINE static const FPoolAllocatorStats& GetTextureAllocatorStats()
		{
			return GetInstance().TextureAllocator.GetStats();
		}

		NODISCARD FORCEINLINE static FRenderResourceAllocatorsStats GetRenderResourceAllocatorsStats()
		{
			FRHI& Instance = GetInstance();
			FRenderResourceAllocatorsStats Stats =
			{
				Instance.VertexArrayAllocator.GetStats(),
				Instance.VertexBufferAllocator.GetStats(),
				Instance.IndexBufferAllocator.GetStats(),
				Instance.UniformBufferAllocator.GetStats(),
				Instance.FrameBufferAllocator.GetStats(),
				Instance.ShaderAllocator.GetStats(),
				Instance.TextureAllocator.GetStats(),
			};
			return Stats;
		}
	};
}
