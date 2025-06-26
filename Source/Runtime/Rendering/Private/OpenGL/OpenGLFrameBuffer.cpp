#include "FrameBuffer.h"
#include "OpenGL/OpenGLHeader.h"

namespace Bloodshot
{
	namespace Private::OpenGL::FrameBuffer
	{
		NODISCARD FORCEINLINE static GLenum GetTextureTarget(const bool bMultisampled)
		{
			return bMultisampled ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
		}
		
		FORCEINLINE static void CreateTextures(const bool bMultisampled, uint32_t* const OutTextures, const size_t Count)
		{
			glCreateTextures(GetTextureTarget(bMultisampled), (GLsizei)Count, OutTextures);
		}

		FORCEINLINE static void AttachColorTexture(const uint32_t FrameBufferID,
			const uint32_t TextureID,
			const size_t SampleCount,
			const GLenum InternalFormat,
			const GLenum Format,
			const uint32_t Width,
			const uint32_t Height,
			const size_t Index)
		{
			const bool bMultisampled = SampleCount > 1;

			if (bMultisampled)
			{
				glTextureImage2DMultisampleNV(TextureID, GL_TEXTURE_2D_MULTISAMPLE, (GLsizei)SampleCount, InternalFormat, Width, Height, GL_FALSE);
			}
			else
			{
				glTextureStorage2D(TextureID, 1, InternalFormat, Width, Height);
				glTextureParameteri(TextureID, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
				glTextureParameteri(TextureID, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTextureParameteri(TextureID, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
				glTextureParameteri(TextureID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTextureParameteri(TextureID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			}

			glNamedFramebufferTexture(FrameBufferID, GL_COLOR_ATTACHMENT0 + (GLenum)Index, TextureID, 0);
		}

		FORCEINLINE static void AttachDepthTexture(const uint32_t FrameBufferID, 
			const uint32_t TextureID,
			const size_t SampleCount,
			const GLenum Format,
			const GLenum AttachmentType,
			const uint32_t Width,
			const uint32_t Height)
		{
			const bool bMultisampled = SampleCount > 1;
		
			if (bMultisampled)
			{
				glTextureImage2DMultisampleNV(TextureID, GL_TEXTURE_2D_MULTISAMPLE, (GLsizei)SampleCount, Format, Width, Height, GL_FALSE);
			}
			else
			{
				glTextureStorage2D(TextureID, 1, Format, Width, Height);
				glTextureParameteri(TextureID, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
				glTextureParameteri(TextureID, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTextureParameteri(TextureID, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
				glTextureParameteri(TextureID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTextureParameteri(TextureID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			}
		
			glNamedFramebufferTexture(FrameBufferID, AttachmentType, TextureID, 0);
		}

		NODISCARD FORCEINLINE static GLenum FrameBufferTextureFormatToOpenGLTextureFormat(const EFrameBufferTextureFormat Format)
		{
			switch (Format)
			{
				case EFrameBufferTextureFormat::RGBA8: return GL_RGBA8;
				case EFrameBufferTextureFormat::RED_INTEGER: return GL_RED_INTEGER;
			}

			BS_ASSERT(false, "FrameBufferTextureFormatToOpenGLTextureFormat: Unknown format passed, or current format cannot be converted to OpenGL texture format");
			return 0;
		}
	}

	FFrameBuffer::FFrameBuffer(const FFrameBufferSettings& InSettings)
		: Settings(InSettings)
	{
		for (const EFrameBufferTextureFormat Attachment : Settings.Attachments)
		{
			if (Attachment != EFrameBufferTextureFormat::DEPTH24_STENCIL8)
			{
				ColorAttachments.EmplaceBack(Attachment);
			}
			else
			{
				DepthAttachment = Attachment;
			}
		}

		Recreate();
	}

	FFrameBuffer::~FFrameBuffer()
	{
		glDeleteFramebuffers(1, &UniqueID);
		glDeleteTextures((GLsizei)ColorAttachmentIDs.GetSize(), ColorAttachmentIDs.GetData());
		glDeleteTextures(1, &DepthAttachmentIndex);
	}

	void FFrameBuffer::Bind() const
	{
		glBindFramebuffer(GL_FRAMEBUFFER, UniqueID);
		glViewport(0, 0, Settings.Width, Settings.Height);
	}

	void FFrameBuffer::Unbind() const
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void FFrameBuffer::Resize(const uint32_t InWidth, const uint32_t InHeight)
	{
		if (InWidth == 0 || InHeight == 0 || InWidth > MaxSize || InHeight > MaxSize)
		{
			BS_LOG(Warning, "FFrameBuffer::Resize: Trying to resize FrameBuffer to {}, {}: min size - (1, 1) max size - ({}, {})", 
				InWidth, 
				InHeight, 
				MaxSize,
				MaxSize);
			return;
		}

		Settings.Width = InWidth;
		Settings.Height = InHeight;

		Recreate();
	}

	void FFrameBuffer::ClearAttachment(const EFrameBufferTextureFormat Attachment, const glm::vec4& Color)
	{
		const size_t AttachmentIndex = ColorAttachments.Find(Attachment);
		BS_ASSERT(ColorAttachmentIDs.IsValidIndex(AttachmentIndex),
			"FFrameBuffer::ClearAttachment: AttachmentIndex was out of range of color attachments");

		glClearTexImage(ColorAttachmentIDs[AttachmentIndex],
			0,
			Private::OpenGL::FrameBuffer::FrameBufferTextureFormatToOpenGLTextureFormat(Attachment),
			GL_FLOAT_VEC4,
			&Color);
	}

	void FFrameBuffer::ClearAttachment(const EFrameBufferTextureFormat Attachment, const uint32_t Value)
	{
		const size_t AttachmentIndex = ColorAttachments.Find(Attachment);
		BS_ASSERT(ColorAttachmentIDs.IsValidIndex(AttachmentIndex),
			"FFrameBuffer::ClearAttachment: AttachmentIndex was out of range of color attachments");

		glClearTexImage(ColorAttachmentIDs[AttachmentIndex],
			0,
			Private::OpenGL::FrameBuffer::FrameBufferTextureFormatToOpenGLTextureFormat(Attachment),
			GL_INT,
			&Value);
	}

	void FFrameBuffer::ClearAttachment(const size_t AttachmentIndex, const glm::vec4& Color)
	{
		BS_ASSERT(ColorAttachmentIDs.IsValidIndex(AttachmentIndex),
			"FFrameBuffer::ClearAttachment: AttachmentIndex was out of range of color attachments");
	
		EFrameBufferTextureFormat Attachment = ColorAttachments[AttachmentIndex];
		glClearTexImage(ColorAttachmentIDs[AttachmentIndex],
			0,
			Private::OpenGL::FrameBuffer::FrameBufferTextureFormatToOpenGLTextureFormat(Attachment),
			GL_FLOAT_VEC4,
			&Color);
	}

	void FFrameBuffer::ClearAttachment(const size_t AttachmentIndex, const uint32_t Value)
	{
		BS_ASSERT(ColorAttachmentIDs.IsValidIndex(AttachmentIndex),
			"FFrameBuffer::ClearAttachment: AttachmentIndex was out of range of color attachments");

		EFrameBufferTextureFormat Attachment = ColorAttachments[AttachmentIndex];
		glClearTexImage(ColorAttachmentIDs[AttachmentIndex],
			0,
			Private::OpenGL::FrameBuffer::FrameBufferTextureFormatToOpenGLTextureFormat(Attachment),
			GL_INT,
			&Value);
	}

	void FFrameBuffer::Recreate()
	{
		if (UniqueID)
		{
			glDeleteFramebuffers(1, &UniqueID);
			glDeleteTextures((GLsizei)ColorAttachmentIDs.GetSize(), ColorAttachmentIDs.GetData());
			glDeleteTextures(1, &DepthAttachmentIndex);
			ColorAttachmentIDs.Clear();
			DepthAttachmentIndex = 0;
		}

		glCreateFramebuffers(1, &UniqueID);

		const bool bMultisampled = Settings.Samples > 1;
		const size_t ColorAttachmentCount = ColorAttachments.GetSize();

		if (ColorAttachmentCount)
		{
			ColorAttachmentIDs.Resize(ColorAttachmentCount);
			Private::OpenGL::FrameBuffer::CreateTextures(bMultisampled, 
				ColorAttachmentIDs.GetData(), 
				ColorAttachmentCount);

			for (size_t i = 0; i < ColorAttachmentCount; ++i)
			{
				switch (ColorAttachments[i])
				{
					case EFrameBufferTextureFormat::RGBA8:
					{
						Private::OpenGL::FrameBuffer::AttachColorTexture(UniqueID,
							ColorAttachmentIDs[i],
							Settings.Samples,
							GL_RGBA8,
							GL_RGBA,
							Settings.Width,
							Settings.Height,
							i);

						break;
					}
					case EFrameBufferTextureFormat::RED_INTEGER:
					{
						Private::OpenGL::FrameBuffer::AttachColorTexture(UniqueID, 
							ColorAttachmentIDs[i],
							Settings.Samples,
							GL_R32I,
							GL_RED_INTEGER,
							Settings.Width,
							Settings.Height,
							i);

						break;
					}
				}
			}
		}

		if (DepthAttachment != EFrameBufferTextureFormat::None)
		{
			Private::OpenGL::FrameBuffer::CreateTextures(bMultisampled, &DepthAttachmentIndex, 1);

			switch (DepthAttachment)
			{
				case EFrameBufferTextureFormat::DEPTH24_STENCIL8:
				{
					Private::OpenGL::FrameBuffer::AttachDepthTexture(UniqueID,
						DepthAttachmentIndex,
						Settings.Samples,
						GL_DEPTH24_STENCIL8,
						GL_DEPTH_ATTACHMENT,
						Settings.Width,
						Settings.Height);

					break;
				}
			}
		}

		const size_t AttachmentCount = ColorAttachmentIDs.GetSize();
		if (AttachmentCount > 1)
		{
			BS_ASSERT(AttachmentCount <= 4, "FFrameBuffer::Recreate: Too many color attachments, only 4 supported");
			constexpr GLenum Buffers[4] =
			{
				GL_COLOR_ATTACHMENT0,
				GL_COLOR_ATTACHMENT1,
				GL_COLOR_ATTACHMENT2,
				GL_COLOR_ATTACHMENT3,
			};

			glNamedFramebufferDrawBuffers(UniqueID, 4, Buffers);
		}
		else if (ColorAttachmentIDs.IsEmpty())
		{
			glNamedFramebufferDrawBuffer(UniqueID, 0);
		}

		BS_ASSERT(glCheckNamedFramebufferStatus(UniqueID, GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE,
			"FFrameBuffer::Recreate: FrameBuffer is incomplete");
	}
}
