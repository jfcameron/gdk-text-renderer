// © 2020 Joseph Cameron - All Rights Reserved

#ifndef GDK_TEXT_RENDER_H
#define GDK_TEXT_RENDER_H

#include <gdk/graphics_context.h>
#include <gdk/scene.h>
#include <gdk/entity.h>
#include <gdk/text_map.h>

namespace gdk
{
	/// \brief base renderer
	///
	/// TODO: should be a kind of entity
	/// possibly entity should be split into an interface? not sure.
	/// this needs to be done so that the user can freely attach the text render to whateverscene etc. 
	/// hide it etc. without code dupes
	//
	// this is clarifying my earlier naval gazing about unicode rendering.
	// expand this to support multiple models, expand this to map to many textures
	// in this way, chinese characters can be spread across a number of textures (to fit platform texture size limitations)
	//
	// shader program with maximum # of textures bound,
	// write texture index into vertex data
	//
	// Width rules? support for carriage returns?
	// width rule in local space. yes thats a good one
	//
	// multi directional support, so render down, left, etc.
	//
	// add character index to vertex data for cool effects
	//
	// text direction, new line direction.
	//
	// individual character spacing (kerning) with a default value of {1} (full width)
	//
	// line spacing (space between lines)
	//
	// Text anchoring
	//
	// add character index vertex data
	//
	// Anchoring: side & corner anchoring
	//
	// localization? global write mode and language write mode, if local does not have text, throw
	//
	class text_renderer
	{
	public:
		using vertex_attribute_type = float;
		using vertex_attribute_array_type = std::vector<vertex_attribute_type>;

		using entity_ptr_type = std::shared_ptr<gdk::entity>;
		using material_ptr_type = std::shared_ptr<gdk::material>;
		using model_ptr_type = std::shared_ptr<gdk::model>;

		enum class alignment
		{
			left_edge,
			right_edge,
			upper_edge,
			lower_edge,

			left_upper_corner,
			left_lower_corner,
			right_upper_corner,
			right_lower_corner,
			
			center,
		};

	private:
		gdk::graphics::context::context_shared_ptr_type pContext;

	protected:
		entity_ptr_type m_Entity;
		material_ptr_type m_Material;
		model_ptr_type m_Model;

		text_map m_TextMap;

		const text_renderer::alignment m_Alignment;

		// replace this stuff with a 4x4
		Vector3<float> m_Position;
		Vector3<float> m_Scale = gdk::Vector3<float>::One;
		Quaternion<float> m_Rotation;

		//! builds quad data to hold a single character, appends to back of param buffers
		// TODO: consider refactoring, static of text_renderer.cpp's translation unit
		void build_character_quad(vertex_attribute_array_type& posData,
			vertex_attribute_array_type& uvData,
			const wchar_t character,
			gdk::IntVector2<size_t> aCharacterOffsetInCells,
			const size_t aLongestLineLength,
			const size_t aLineCount,
			const text_renderer::alignment aAlignment);

		//! builds model data for the string
		void build_string_model(vertex_data_view::UsageHint hint, const std::wstring &aText);

	public:
		/// This is duplication of nearly the entire graphics::entity interface. Some serious thought needs to be had
		// about providing some sort of "drawables" interface (more restrictive than the entity interface, from which entity inherits and
		/// scenes operate on instead of the entity interface).
		/// this would let graphics::scenes work directly with higher level abstractions such as this text renderer. no need for duping

		void hide();

		void show();

		bool isHidden() const;

		void set_model_matrix(const graphics_vector3_type& aWorldPos,
			const graphics_quaternion_type& aRotation,
			const graphics_vector3_type& aScale = graphics_vector3_type::One);

		//void set_model_matrix(const graphics_mat4x4_type& a);
		///


		//! can be added to multiple scenes
		void add_to_scene(gdk::graphics::context::scene_shared_ptr_type pScene);

		//! removes the text renderer's entities from the scene
		/// if the scene does not contain them, no error will occur
		void remove_from_scene(gdk::graphics::context::scene_shared_ptr_type pScene);

	protected:
		text_renderer(gdk::graphics::context::context_shared_ptr_type pContext, 
			gdk::text_map aTextMap,
			const text_renderer::alignment aAlignment,
			std::shared_ptr<material> aMaterial = nullptr);

	public:
		virtual ~text_renderer() = default;
	};
}

#endif

