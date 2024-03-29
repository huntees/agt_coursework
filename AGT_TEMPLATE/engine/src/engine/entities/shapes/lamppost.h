#pragma once

namespace engine
{
	class mesh;

	/// \brief Class creating a lamppost object with a mesh of a specified size
	class lamppost
	{
	public:
		/// \brief Constructor
		lamppost();

		/// \brief Destructor
		~lamppost();

		/// \brief Getter methods
		std::vector<glm::vec3> vertices() const { return m_vertices; }
		ref<engine::mesh> mesh() const { return m_mesh; }

		static ref<lamppost> create();

	private:
		/// \brief Fields
		// vertices of the lamppost specified by a vector of positions.
		std::vector<glm::vec3> m_vertices;

		ref<engine::mesh> m_mesh;
	};
}
#pragma once
