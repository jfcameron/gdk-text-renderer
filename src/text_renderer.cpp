#include<gdk/text_renderer.h>

using namespace gdk;

static constexpr char TAG[] = "gdk::text_renderer";

text_renderer::text_renderer(gdk::graphics::context::context_shared_ptr_type aContext,
	gdk::text_map aTextMap,
	const text_renderer::alignment aAlignment,
	std::shared_ptr<material> aMaterial)
	: pContext(aContext)
	, m_TextMap(aTextMap)
	, m_Alignment(aAlignment)
	, m_Material([aMaterial, aContext, aTextMap]()
	{
		std::shared_ptr<material> p;

		if (!aMaterial)
		{
			p = std::shared_ptr<material>(std::move(aContext->make_material(aContext->get_alpha_cutoff_shader())));
			p->setTexture("_Texture", aTextMap.getTexture());
			p->setVector2("_UVScale", { 1, 1 });
			p->setVector2("_UVOffset", { 0, 0 });
		}
		else p = aMaterial;

		return p;
	}())
{}

static inline Vector2<float> alignment_to_offset_vector(text_renderer::alignment aAlignment, 
	size_t aLongestLineLength, size_t aLineCount)
{
	const auto width(static_cast<float>(aLongestLineLength));
	const auto half_width(width / 2.f + 0.5f);
	
	const auto height(static_cast<float>(aLineCount));
	const auto half_height(static_cast<float>(aLineCount) / 2);

	switch (aAlignment)
	{
		case text_renderer::alignment::left_edge: return         { 0, 0.5f - half_height };
		case text_renderer::alignment::left_lower_corner: return { 0, 0.0f - height };
		case text_renderer::alignment::left_upper_corner: return { 0, 1.0f };

		case text_renderer::alignment::right_edge: return         { -width, 0.5f - half_height };
		case text_renderer::alignment::right_lower_corner: return { -width, 0.0f - height };
		case text_renderer::alignment::right_upper_corner: return { -width, 1.0f };

		case text_renderer::alignment::center: return	  { 0.5f - half_width, 0.5f - half_height };
		case text_renderer::alignment::upper_edge: return { 0.5f - half_width, 1.0f };//2.0f - half_height };
		case text_renderer::alignment::lower_edge: return { 0.5f - half_width, 0.0f - height };

		default: throw std::runtime_error(std::string(TAG) + ": unhandled text alignment");
	}
}

void text_renderer::build_character_quad(vertex_attribute_array_type& rPosData,
	vertex_attribute_array_type& rUVData,
	const wchar_t character,
	gdk::IntVector2<size_t> aCharacterOffsetInCells,
	const size_t aLongestLineLength,
	const size_t aLineCount,
	const text_renderer::alignment aAlignment)
{
	vertex_attribute_array_type posData({
		1, 1, 0,
		0, 1, 0,
		0, 0, 0,
		1, 1, 0,
		0, 0, 0,
		1, 0, 0
	});

	auto rasterCoord = m_TextMap.get_raster_coordinate(character);

	auto textureSizeInCells = m_TextMap.getTextureSizeInCells();

	gdk::Vector2<float> character_offset = aCharacterOffsetInCells.toVector2();

	for (size_t i(0), s(posData.size()); i < s; i += 3)
	{
		Vector2<float> alignment_offset = 
			alignment_to_offset_vector(aAlignment, aLongestLineLength, aLineCount);

		posData[i + 0] += alignment_offset.x;
		posData[i + 1] -= alignment_offset.y;
		posData[i + 2] += 0;

		posData[i + 0] += character_offset.x;
		posData[i + 1] -= character_offset.y;
		posData[i + 2] += 0;
	}

	rPosData.insert(rPosData.end(), posData.begin(), posData.end());

	gdk::Vector2<float> cellSize(1.f / textureSizeInCells.x, 1.f / textureSizeInCells.y);

	const float sampleBias = 0.001f;

	float bottom_xl = rasterCoord.x * cellSize.x + sampleBias;
	float bottom_yl = rasterCoord.y * cellSize.y + sampleBias;
	float bottom_xh = rasterCoord.x * cellSize.x + cellSize.x - sampleBias;
	float bottom_yh = rasterCoord.y * cellSize.y + cellSize.y - sampleBias;

	vertex_attribute_array_type uvData({
		bottom_xh, bottom_yl,
		bottom_xl, bottom_yl,
		bottom_xl, bottom_yh,
		bottom_xh, bottom_yl,
		bottom_xl, bottom_yh,
		bottom_xh, bottom_yh,
	});

	rUVData.insert(rUVData.end(), uvData.begin(), uvData.end());
}

void text_renderer::add_to_scene(gdk::graphics::context::scene_shared_ptr_type pScene)
{
	pScene->add_entity(m_Entity);
}

void text_renderer::remove_from_scene(gdk::graphics::context::scene_shared_ptr_type pScene)
{
	pScene->remove_entity(m_Entity);
}

void text_renderer::build_string_model(vertex_data_view::UsageHint hint, 
	const std::wstring& aText)
{
	text_map aTextMap = m_TextMap;

	text_renderer::vertex_attribute_array_type posData;
	text_renderer::vertex_attribute_array_type uvData;

	size_t longestLineLength(0), lineCount(0);

	for (int i(0), s(aText.size()), current_line_length(0); i < s; ++i)
	{
		if (aText[i] == '\n' || aText[i] == '\r')
		{
			current_line_length = 0;

			lineCount++;
		}
		else
		{
			current_line_length++;

			if (longestLineLength < current_line_length)
			{
				longestLineLength = current_line_length;
			}
		}
	}

	for (int input_index(0), input_size(aText.size()), current_line_char_counter(0), line_counter(0); 
		input_index < input_size; ++input_index)
	{
		auto current_character = aText[input_index];

		if (current_character == L'\n' || current_character == L'\r')
		{
			current_line_char_counter = 0;
			++line_counter;
		}
		else
		{
			build_character_quad(posData, uvData, current_character,
					{ current_line_char_counter, line_counter }, longestLineLength, lineCount, m_Alignment);

			++current_line_char_counter;
		}
	}

	vertex_data_view data = { hint,
	{
		{
			"a_Position",
			{
				&posData.front(),
				posData.size(),
				3
			}
		},
		{
			"a_UV",
			{
				&uvData.front(),
				uvData.size(),
				2
			}
		}
	}};

	if (!m_Model)
	{
		m_Model = std::shared_ptr<model>(std::move(
			pContext->make_model(data)));
	}
	else
	{
		m_Model->update_vertex_data(data);
	}

	if (!m_Entity)
	{
		m_Entity = pContext->make_entity(m_Model, m_Material);

		m_Entity->set_model_matrix(m_Position, m_Rotation, m_Scale);
	}
}

void text_renderer::set_model_matrix(const graphics_vector3_type& aWorldPos,
	const graphics_quaternion_type& aRotation,
	const graphics_vector3_type& aScale)
{
	m_Position = aWorldPos;
	m_Rotation = aRotation;
	m_Scale = aScale;

	m_Entity->set_model_matrix(m_Position, m_Rotation, m_Scale);
}

void text_renderer::hide()
{
	m_Entity->hide();
}

void text_renderer::show()
{
	m_Entity->show();
}

bool text_renderer::isHidden() const
{
	return m_Entity->isHidden();
}
