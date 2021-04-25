#pragma once

#include <yaml-cpp/yaml.h>

#include "Saffron/Base.h"
#include "Saffron/Math/SaffronMath.h"

///////////////////////////////////////////////////////////////////////////
/// YAML - Template Specialization
///////////////////////////////////////////////////////////////////////////

namespace YAML
{
template <>
struct convert<Se::Vector2>
{
	static Node encode(const Se::Vector2& rhs)
	{
		Node node;
		node.push_back(rhs.x);
		node.push_back(rhs.y);
		return node;
	}

	static bool decode(const Node& node, Se::Vector2& rhs)
	{
		if (!node.IsSequence() || node.size() != 2) return false;

		rhs.x = node[0].as<float>();
		rhs.y = node[1].as<float>();
		return true;
	}
};

template <>
struct convert<Se::Vector3>
{
	static Node encode(const Se::Vector3& rhs)
	{
		Node node;
		node.push_back(rhs.x);
		node.push_back(rhs.y);
		node.push_back(rhs.z);
		return node;
	}

	static bool decode(const Node& node, Se::Vector3& rhs)
	{
		if (!node.IsSequence() || node.size() != 3) return false;

		rhs.x = node[0].as<float>();
		rhs.y = node[1].as<float>();
		rhs.z = node[2].as<float>();
		return true;
	}
};

template <>
struct convert<Se::Vector4>
{
	static Node encode(const Se::Vector4& rhs)
	{
		Node node;
		node.push_back(rhs.x);
		node.push_back(rhs.y);
		node.push_back(rhs.z);
		node.push_back(rhs.w);
		return node;
	}

	static bool decode(const Node& node, Se::Vector4& rhs)
	{
		if (!node.IsSequence() || node.size() != 4) return false;

		rhs.x = node[0].as<float>();
		rhs.y = node[1].as<float>();
		rhs.z = node[2].as<float>();
		rhs.w = node[3].as<float>();
		return true;
	}
};

template <>
struct convert<Se::Quaternion>
{
	static Node encode(const Se::Quaternion& rhs)
	{
		Node node;
		node.push_back(rhs.w);
		node.push_back(rhs.x);
		node.push_back(rhs.y);
		node.push_back(rhs.z);
		return node;
	}

	static bool decode(const Node& node, Se::Quaternion& rhs)
	{
		if (!node.IsSequence() || node.size() != 4) return false;

		rhs.w = node[0].as<float>();
		rhs.x = node[1].as<float>();
		rhs.y = node[2].as<float>();
		rhs.z = node[3].as<float>();
		return true;
	}
};

template <>
struct convert<Se::Matrix4>
{
	static Node encode(const Se::Matrix4& rhs)
	{
		Node node;
		const auto* valuePtr = value_ptr(rhs);
		for (int i = 0; i < 16; i++)
		{
			node.push_back(*(valuePtr + i));
		}
		return node;
	}

	static bool decode(const Node& node, Se::Matrix4& rhs)
	{
		if (!node.IsSequence() || node.size() != 16) return false;

		float matrixData[16];
		for (int i = 0; i < 16; i++)
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

YAML::Emitter& operator<<(YAML::Emitter& out, const Vector2& v)
{
	out << YAML::Flow;
	out << YAML::BeginSeq << v.x << v.y << YAML::EndSeq;
	return out;
}

YAML::Emitter& operator<<(YAML::Emitter& out, const Vector3& v)
{
	out << YAML::Flow;
	out << YAML::BeginSeq << v.x << v.y << v.z << YAML::EndSeq;
	return out;
}

YAML::Emitter& operator<<(YAML::Emitter& out, const Vector4& v)
{
	out << YAML::Flow;
	out << YAML::BeginSeq << v.x << v.y << v.z << v.w << YAML::EndSeq;
	return out;
}

YAML::Emitter& operator<<(YAML::Emitter& out, const Quaternion& v)
{
	out << YAML::Flow;
	out << YAML::BeginSeq << v.w << v.x << v.y << v.z << YAML::EndSeq;
	return out;
}

YAML::Emitter& operator<<(YAML::Emitter& out, const Matrix4& m)
{
	out << YAML::Flow;
	const auto* valuePtr = value_ptr(m);
	out << YAML::BeginSeq;
	for (int i = 0; i < 16; i++)
	{
		out << *(valuePtr + i);
	}
	out << YAML::EndSeq;
	return out;
}
}
