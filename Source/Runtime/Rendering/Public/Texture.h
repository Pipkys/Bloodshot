#pragma once

#include "Core.h"

namespace Bloodshot
{
	enum class EColorChannels : uint8_t
	{
		R = 1,
		RG,
		RGB,
		RGBA
	};

	enum class ETextureUnit : uint8_t
	{
		BaseColor = 0,
		Diffuse = 1,
		NormalMap = 2,
		Roughness = 3,
		Specular = 4,
		Metallic = 5,
		HeightMap = 6,
	};

	class FTexture final
	{
		friend class FRHI;

	public:
		~FTexture();

		NODISCARD FORCEINLINE uint32_t GetUniqueID() const
		{
			return UniqueID;
		}

		void Bind(const ETextureUnit Unit) const;
		void Unbind(const ETextureUnit Unit) const;

	protected:
		FTexture(const uint32_t InWidth,
			const uint32_t InHeight,
			const EColorChannels InChannels,
			const void* const InData);

		uint32_t UniqueID;
		uint32_t Width;
		uint32_t Height;
		EColorChannels Channels;
	};
}