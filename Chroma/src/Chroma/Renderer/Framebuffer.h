#pragma once

#include "Chroma/Core/Core.h"

namespace Chroma
{
	struct FramebufferInfo
	{
		uint32_t Width, Height;
		uint32_t Samples = 1;

		// Rendering to the swap chain
		bool SwapChainTarget = false;
	};

	class Framebuffer
	{
	public:
		virtual ~Framebuffer() = default;
		static Ref<Framebuffer> Create(const FramebufferInfo& spec);

		virtual void Resize() = 0;
		virtual void Resize(uint32_t width, uint32_t height) = 0;
		virtual void Bind() = 0;
		virtual void Unbind() = 0;

		virtual uint32_t GetColorAttachmentRendererID() const = 0;
		virtual const FramebufferInfo& GetSpecification() const = 0;

		
	};
}


