#include "SaffronPCH.h"

#include "json/json.h"

#include "Saffron/Serialize/SceneSerializer.h"
#include "Saffron/Scene/Entity.h"
#include "Saffron/Scene/EntityComponents.h"
#include "Saffron/Scene/SceneRegistry.h"

using namespace nlohmann;

Se::Scene* context = nullptr;
const Se::Scene* constContext = nullptr;

template <>
struct nlohmann::adl_serializer<Se::Vector3>
{
	static void to_json(json& j, const Se::Vector3& value)
	{
		j = {value.x, value.y, value.z};
	}

	static void from_json(const json& j, Se::Vector3& value)
	{
		value.x = j[0];
		value.y = j[1];
		value.z = j[2];
	}
};

template <>
struct nlohmann::adl_serializer<Se::Uuid>
{
	static void to_json(json& j, const Se::Uuid& value)
	{
		j = static_cast<Se::ulong>(value);
	}

	static void from_json(const json& j, Se::Uuid& value)
	{
		value = j.get<Se::ulong>();
	}
};

template <>
struct nlohmann::adl_serializer<std::filesystem::path>
{
	static void to_json(json& j, const std::filesystem::path& value)
	{
		j = value.string();
	}

	static void from_json(const json& j, std::filesystem::path& value)
	{
		value = j.get<std::string>();
	}
};

template <>
struct nlohmann::adl_serializer<Se::Entity>
{
	static void to_json(json& j, const Se::Entity& value)
	{
		// Core, always have
		auto idComp = value.Get<Se::IdComponent>();
		auto nameComp = value.Get<Se::NameComponent>();
		auto transformComp = value.Get<Se::TransformComponent>();

		j = {
			{"id", idComp.Id}, {"name", nameComp.Name}, {"translation", transformComp.Translation},
			{"rotation", transformComp.Rotation}, {"scale", transformComp.Scale}
		};

		// Extra, need to check
		const auto& modelComponent = value.Get<Se::ModelComponent>();

		j.emplace("model", modelComponent.Model->Path());
	}

	static void from_json(const json& j, Se::Entity& value)
	{
		// Core, always have
		auto id = j.at("id").get<Se::Uuid>();
		value = context->CreateEntity(id);

		auto& nameComp = value.Add<Se::NameComponent>();
		auto& transformComp = value.Add<Se::TransformComponent>();

		j.at("name").get_to(nameComp.Name);
		j.at("translation").get_to(transformComp.Translation);
		j.at("rotation").get_to(transformComp.Rotation);
		j.at("scale").get_to(transformComp.Scale);

		// Extra, need to check
		auto& modelComponent = value.Add<Se::ModelComponent>();

		modelComponent.Model = Se::Model::Create(j.at("model").get<std::filesystem::path>());
	}
};


namespace Se
{
const std::filesystem::path SceneSerializer::FromBuildPath("../Engine/");
const std::filesystem::path SceneSerializer::BasePath("Assets/Scenes/");

void SceneSerializer::WriteToFile(const Scene& scene, const std::filesystem::path& output)
{
	constContext = &scene;

	json jsonScene;

	json entities;
	scene.Registry().each(
		[&scene, &entities](entt::entity entt)
		{
			Entity entity{static_cast<ulong>(entt), &const_cast<Scene&>(scene)};
			entities.emplace_back(entity);
		}
	);

	constContext = nullptr;

	jsonScene.emplace("entities", entities);

	std::filesystem::path fullpath = FromBuildPath;
	fullpath += BasePath;
	fullpath += output;

	std::ofstream os(fullpath);
	os << jsonScene;
}

auto SceneSerializer::ReadFromFile(const std::filesystem::path& path) -> std::shared_ptr<Scene>
{
	std::filesystem::path fullpath = BasePath;
	fullpath += path;

	const auto json = json::parse(std::ifstream(fullpath.c_str()));

	auto scene = SceneRegistry::CreateScene();

	context = scene.get();

	const auto& entites = json["entities"];
	for (auto& jsonEntity : entites)
	{
		auto entity = jsonEntity.get<Entity>();
		Log::Info("Deserialized Entity {0}", entity.Get<NameComponent>().Name);
	}

	context = nullptr;

	return scene;
}
}
