#pragma once

#include <yaml-cpp/yaml.h>

#include "Saffron/Base.h"
#include "Saffron/Core/Math/SaffronMath.h"

///////////////////////////////////////////////////////////////////////////
/// YAML - Template Specialization
///////////////////////////////////////////////////////////////////////////

namespace YAML {

template<>
struct convert<Se::Vector2f>
{
	static Node encode(const Se::Vector2f &rhs)
	{
		Node node;
		node.push_back(rhs.x);
		node.push_back(rhs.y);
		return node;
	}

	static bool decode(const Node &node, Se::Vector2f &rhs)
	{
		if ( !node.IsSequence() || node.size() != 2 )
			return false;

		rhs.x = node[0].as<float>();
		rhs.y = node[1].as<float>();
		return true;
	}
};

template<>
struct convert<Se::Vector3f>
{
	static Node encode(const Se::Vector3f &rhs)
	{
		Node node;
		node.push_back(rhs.x);
		node.push_back(rhs.y);
		node.push_back(rhs.z);
		return node;
	}

	static bool decode(const Node &node, Se::Vector3f &rhs)
	{
		if ( !node.IsSequence() || node.size() != 3 )
			return false;

		rhs.x = node[0].as<float>();
		rhs.y = node[1].as<float>();
		rhs.z = node[2].as<float>();
		return true;
	}
};

template<>
struct convert<Se::Vector4f>
{
	static Node encode(const Se::Vector4f &rhs)
	{
		Node node;
		node.push_back(rhs.x);
		node.push_back(rhs.y);
		node.push_back(rhs.z);
		node.push_back(rhs.w);
		return node;
	}

	static bool decode(const Node &node, Se::Vector4f &rhs)
	{
		if ( !node.IsSequence() || node.size() != 4 )
			return false;

		rhs.x = node[0].as<float>();
		rhs.y = node[1].as<float>();
		rhs.z = node[2].as<float>();
		rhs.w = node[3].as<float>();
		return true;
	}
};

template<>
struct convert<Se::Quaternion>
{
	static Node encode(const Se::Quaternion &rhs)
	{
		Node node;
		node.push_back(rhs.w);
		node.push_back(rhs.x);
		node.push_back(rhs.y);
		node.push_back(rhs.z);
		return node;
	}

	static bool decode(const Node &node, Se::Quaternion &rhs)
	{
		if ( !node.IsSequence() || node.size() != 4 )
			return false;

		rhs.w = node[0].as<float>();
		rhs.x = node[1].as<float>();
		rhs.y = node[2].as<float>();
		rhs.z = node[3].as<float>();
		return true;
	}
};

template<>
struct convert<Se::Matrix4f>
{
	static Node encode(const Se::Matrix4f &rhs)
	{
		Node node;
		const auto *valuePtr = glm::value_ptr(rhs);
		for ( int i = 0; i < 16; i++ )
		{
			node.push_back(*(valuePtr + i));
		}
		return node;
	}

	static bool decode(const Node &node, Se::Matrix4f &rhs)
	{
		if ( !node.IsSequence() || node.size() != 16 )
			return false;

		float matrixData[16];
		for ( int i = 0; i < 16; i++ )
		{
			matrixData[i] = node[i].as<float>();
		}
		rhs = glm::make_mat4(matrixData);

		return true;
	}
};
}


namespace Se
{

///////////////////////////////////////////////////////////////////////////
/// YAML - Operator overloading
///////////////////////////////////////////////////////////////////////////

YAML::Emitter &operator<<(YAML::Emitter &out, const Vector2f &v)
{
	out << YAML::Flow;
	out << YAML::BeginSeq << v.x << v.y << YAML::EndSeq;
	return out;
}

YAML::Emitter &operator<<(YAML::Emitter &out, const Vector3f &v)
{
	out << YAML::Flow;
	out << YAML::BeginSeq << v.x << v.y << v.z << YAML::EndSeq;
	return out;
}

YAML::Emitter &operator<<(YAML::Emitter &out, const Vector4f &v)
{
	out << YAML::Flow;
	out << YAML::BeginSeq << v.x << v.y << v.z << v.w << YAML::EndSeq;
	return out;
}

YAML::Emitter &operator<<(YAML::Emitter &out, const Quaternion &v)
{
	out << YAML::Flow;
	out << YAML::BeginSeq << v.w << v.x << v.y << v.z << YAML::EndSeq;
	return out;
}

YAML::Emitter &operator<<(YAML::Emitter &out, const Matrix4f &m)
{
	out << YAML::Flow;
	const auto *valuePtr = glm::value_ptr(m);
	out << YAML::BeginSeq;
	for ( int i = 0; i < 16; i++ )
	{
		out << *(valuePtr + i);
	}
	out << YAML::EndSeq;
	return out;
}
}