#pragma once

namespace engine
{
	class mesh;

	/// \brief Class creating a bench object with a mesh of a specified size
	class bench
	{
	public:
		/// \brief Constructor
		bench();

		/// \brief Destructor
		~bench();

		/// \brief Getter methods
		std::vector<glm::vec3> vertices() const { return m_vertices; }
		ref<engine::mesh> mesh() const { return m_mesh; }

		static ref<bench> create();

	private:
		/// \brief Fields
		// vertices of the bench specified by a vector of positions.
		std::vector<glm::vec3> m_vertices;

		ref<engine::mesh> m_mesh;
	};
}
#pragma once
