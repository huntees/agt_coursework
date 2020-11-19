#pragma once

namespace engine
{
	class mesh;

	/// \brief Class creating a hexagon object with a mesh of a specified size
	class hexagon
	{
	public:
		/// \brief Constructor
		hexagon();

		/// \brief Destructor
		~hexagon();

		/// \brief Getter methods
		std::vector<glm::vec3> vertices() const { return m_vertices; }
		ref<engine::mesh> mesh() const { return m_mesh; }

		static ref<hexagon> create();

	private:
		/// \brief Fields
		// vertices of the hexagon specified by a vector of positions.
		std::vector<glm::vec3> m_vertices;

		ref<engine::mesh> m_mesh;
	};
}
#pragma once
