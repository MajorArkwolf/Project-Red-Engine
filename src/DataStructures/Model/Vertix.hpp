#pragma once
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <array>
#include <glm/gtc/quaternion.hpp>
#include <vector>

namespace Blue {
    /// Used for the terrain model generation.
	struct Vertex {
		glm::vec3 position = {};
		glm::vec2 texCoords = {};
		glm::vec3 normals = {};
	};

	struct Perlin {
		float height = 0;
		glm::vec3 color = {};
	};
    /// Used to transfer information from terrain to react.
    struct HeightRange {
        float min = 0.0f;
        float max = 0.0f;
        float range = 0.0f;
    };

	struct SimpleMesh {
	    std::vector<glm::vec3> vertex = {};
	    std::vector<unsigned int> indices = {};
	};
}
