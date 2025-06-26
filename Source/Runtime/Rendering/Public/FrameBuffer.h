#pragma once

#include "Core.h"

namespace Bloodshot
{
	enum class EFrameBufferTextureFormat : uint8_t
	{
		None = 0,
		RGBA8,
		RED_INTEGER,
		DEPTH24_STENCIL8,
		Depth = DEPTH24_STENCIL8
	};

	class FFrameBufferSettings final
	{
	public:
		uint32_t Width = 0;
		uint32_t Height = 0;
		uint32_t Samples = 1;
		bool SwapChainTarget = false;
		TArray<EFrameBufferTextureFormat> Attachments;
	};

	class FFrameBuffer final
	{
		friend class FRHI;

	public:
		~FFrameBuffer();

		NODISCARD FORCEINLINE const FFrameBufferSettings& GetSettings() const
		{
			return Settings;
		}

		NODISCARD FORCEINLINE uint32_t GetColorAttachmentID(const size_t Index = 0) const
		{
			BS_CHECK(ColorAttachmentIDs.IsValidIndex(Index));
			return ColorAttachmentIDs[Index];
		}

		void Bind() const;
		void Unbind() const;

		void Resize(const uint32_t Width, const uint32_t Height);

		void ClearAttachment(const EFrameBufferTextureFormat Attachment, const glm::vec4& Color);
		void ClearAttachment(const EFrameBufferTextureFormat Attachment, const uint32_t Value);
		void ClearAttachment(const size_t AttachmentIndex, const glm::vec4& Color);
		void ClearAttachment(const size_t AttachmentIndex, const uint32_t Value);

	protected:
		FFrameBuffer(const FFrameBufferSettings& InSettings);

		static constexpr inline uint32_t MaxSize = 8192;

		uint32_t UniqueID = 0;
		FFrameBufferSettings Settings;

		TArray<EFrameBufferTextureFormat> ColorAttachments;

		TArray<uint32_t> ColorAttachmentIDs;
		uint32_t DepthAttachmentIndex = 0;

		EFrameBufferTextureFormat DepthAttachment = EFrameBufferTextureFormat::None;

		void Recreate();
	};
}