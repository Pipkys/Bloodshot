#include "Texture.h"
#include "OpenGL/OpenGLHeader.h"

namespace Bloodshot
{
	FTexture::FTexture(const uint32_t InWidth,
		const uint32_t InHeight,
		const EColorChannels InChannels,
		const void* const InData)
		: Width(InWidth)
		, Height(InHeight)
		, Channels(InChannels)
	{
		glCreateTextures(GL_TEXTURE_2D, 1, &UniqueID);

		glTextureParameteri(UniqueID, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTextureParameteri(UniqueID, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTextureParameteri(UniqueID, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTextureParameteri(UniqueID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		
		switch (Channels)
		{
			case EColorChannels::R:
			{
				glTextureStorage2D(UniqueID, 1, GL_R8, Width, Height);
				glTextureSubImage2D(UniqueID, 0, 0, 0, Width, Height, GL_RED, GL_UNSIGNED_BYTE, InData);
				break;
			}
			case EColorChannels::RG:
			{
				glTextureStorage2D(UniqueID, 1, GL_RG8, Width, Height);
				glTextureSubImage2D(UniqueID, 0, 0, 0, Width, Height, GL_RG, GL_UNSIGNED_BYTE, InData);
				break;
			}
			case EColorChannels::RGB:
			{
				glTextureStorage2D(UniqueID, 1, GL_RGB8, Width, Height);
				glTextureSubImage2D(UniqueID, 0, 0, 0, Width, Height, GL_RGB, GL_UNSIGNED_BYTE, InData);
				break;
			}
			case EColorChannels::RGBA:
			{
				glTextureStorage2D(UniqueID, 1, GL_RGBA8, Width, Height);
				glTextureSubImage2D(UniqueID, 0, 0, 0, Width, Height, GL_RGBA, GL_UNSIGNED_BYTE, InData);
				break;
			}
		}

		glGenerateTextureMipmap(UniqueID);
	}

	FTexture::~FTexture()
	{
		glDeleteTextures(1, &UniqueID);
	}

	void FTexture::Bind(const ETextureUnit Unit) const
	{
		glBindTextureUnit(static_cast<uint8_t>(Unit), UniqueID);
	}

	void FTexture::Unbind(const ETextureUnit Unit) const
	{
		glBindTextureUnit(static_cast<uint8_t>(Unit), 0);
	}
}
