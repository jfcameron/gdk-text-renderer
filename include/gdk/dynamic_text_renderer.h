// � 2020 Joseph Cameron - All Rights Reserved

#ifndef GDK_DYNAMIC_TEXT_RENDER_H
#define GDK_DYNAMIC_TEXT_RENDER_H

#include <gdk/text_renderer.h>

namespace gdk
{
	/// \brief base text renderer
	class dynamic_text_renderer final : public text_renderer
	{
		/// \brief text buffer, used to prevent unnecessary vertex data reconstruction
		std::wstring m_Text;

	public:
		void update_text(const std::wstring &string);

		dynamic_text_renderer(gdk::graphics::context::context_shared_ptr_type pContext,
			gdk::text_map aTextMap,
			const text_renderer::alignment aAlignment,
			std::wstring aText = L" ");

		~dynamic_text_renderer() = default;
	};
}

#endif
