#pragma once

#include "Saffron/Base.h"
#include "Saffron/Graphics/Model.h"

namespace Se
{
struct NameComponent
{
	NameComponent() = default;

	explicit NameComponent(std::string name) :
		Name(name)
	{
	}

	std::string Name;
};

struct IdComponent
{
	IdComponent() = default;

	explicit IdComponent(Uuid id) :
		Id(id)
	{
	}

	Uuid Id;
};

struct ModelComponent
{
	ModelComponent() = default;

	explicit ModelComponent(std::shared_ptr<Model> model) :
		Model(model)
	{
	}

	std::shared_ptr<Model> Model;
};

struct TransformComponent
{
	TransformComponent() :
		Translation(0, 0, 0),
		Scale(1, 1, 1),
		Rotation(0, 0, 0)
	{
	}

	explicit TransformComponent(const Vector3& Translation, const Vector3& Scale, const Vector3& Rotation) :
		Translation(Translation),
		Scale(Scale),
		Rotation(Rotation)
	{
	}

	auto Transform() const -> Matrix
	{
		return Matrix::CreateScale(Scale) * Matrix::CreateFromQuaternion(
			Quaternion::CreateFromYawPitchRoll(Rotation.x, Rotation.y, Rotation.z)
		) * Matrix::CreateTranslation(Translation);
	}

	Vector3 Translation, Scale, Rotation;
};
}
