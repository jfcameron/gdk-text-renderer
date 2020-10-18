#include<gdk/dynamic_text_renderer.h>

using namespace gdk;

void dynamic_text_renderer::update_text(const std::wstring& aText)
{
	if (aText == m_Text) return;

	build_string_model(vertex_data_view::UsageHint::Dynamic, aText);

	m_Text = aText;
}

dynamic_text_renderer::dynamic_text_renderer(gdk::graphics::context::context_shared_ptr_type pContext,
	gdk::text_map aTextMap,
	const text_renderer::alignment aAlignment,
	std::wstring aText)
	: text_renderer(pContext, aTextMap, aAlignment)
	, m_Text(aText)
{
	build_string_model(vertex_data_view::UsageHint::Dynamic, aText);
}
