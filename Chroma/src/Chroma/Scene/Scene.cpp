#include "chromapch.h"

#include "Scene.h"
#include "EntityRef.h"
#include <Chroma/Components/Transform.h>
#include <Chroma/Components/Tag.h>
#include <Chroma/Components/AudioSource.h>
#include <Chroma/Components/SpriteRenderer.h>
#include <Chroma/Components/BoxCollider2D.h>
#include <Chroma/Components/CircleCollider2D.h>
#include <Chroma/Systems/AudioSystem.h>
#include <Chroma/Systems/SpriteRendererSystem.h>
#include <yaml-cpp/node/node.h>
#include <yaml-cpp/node/parse.h>
#include <yaml-cpp/yaml.h>
#include <Chroma/Scene/System.h>



namespace Chroma
{
	Scene::Scene()
	{
		RegisterComponent<Tag>();
		RegisterComponent<Transform>();
		RegisterComponent<AudioSource>();
		RegisterComponent<SpriteRenderer>();
		RegisterComponent<BoxCollider2D>();
		RegisterComponent<CircleCollider2D>();

		RegisterSystem<AudioSystem>();
		RegisterSystem<SpriteRendererSystem>();
	}

	EntityRef Scene::NewEntity()
	{
		if (!m_FreeEntities.empty())
		{
			EntityIndex newIndex = m_FreeEntities.back();
			m_FreeEntities.pop_back();
			EntityID newID = CreateEntityId(newIndex, GetEntityVersion(m_Entities[newIndex]));
			m_Entities[newIndex] = newID;
			EntityRef entity = { newID, *this };
			return { m_Entities[newIndex], *this };
		}
		m_Entities.push_back(CreateEntityId(EntityIndex(m_Entities.size()), 0));
		EntityRef e = { m_Entities.back(), *this };
		std::string name = "Entity_" + std::to_string(GetEntityIndex(e.m_EntityID));
		e.AddComponent<Tag>()->EntityName = name;
		e.AddComponent<Transform>();
		return { m_Entities.back(), *this };
	}


	EntityRef Scene::NewEntityFromID(EntityID id)
	{
		m_Entities.push_back(CreateEntityId(EntityIndex(id), 0));
		EntityRef e = { m_Entities.back(), *this };
		return { m_Entities.back(), *this };
	}

	void Scene::DestroyEntity(EntityRef entity)
	{
		EntityID id = entity.m_EntityID;
		EntityID newID = CreateEntityId(EntityIndex(-1), GetEntityVersion(id) + 1);
		m_Entities[GetEntityIndex(id)] = newID;
		m_FreeEntities.push_back(GetEntityIndex(id));
		for (AbstractComponentPool* pool : m_ComponentPools)
		{
			pool->RemoveAll(id);
		}
	}

	std::string Scene::Serialize()
	{
		YAML::Emitter out;
		out << YAML::BeginMap;
		out << YAML::Key << "Scene" << YAML::Value << this->Name;
		out << YAML::Key << "Entities" << YAML::Value << YAML::BeginSeq;

		for (EntityRef e : this->View<Tag>())
		{
			out << YAML::BeginMap;
			out << YAML::Key << "Entity" << YAML::Value << e.m_EntityID;
			out << YAML::Key << "Components" << YAML::Value << YAML::BeginMap;
			for (ComponentRef c : e.GetAllComponents())
			{
				c->BeginSerialize(out);
				c->Serialize(out);
				c->EndSerialize(out);
			}
			out << YAML::EndMap;
			out << YAML::EndMap;
		}

		out << YAML::EndSeq;
		out << YAML::EndMap;

		std::string result = out.c_str();
		return result;

	}

	bool Scene::Deserialize(Scene &out, const std::string& yaml)
	{
		auto data = YAML::Load(yaml);
		if (!data["Scene"])
			return false;

		std::string sceneName = data["Scene"].as<std::string>();
		CHROMA_CORE_TRACE("Deserializing Scene '{}'", sceneName);

		out.Name = sceneName;

		auto entities = data["Entities"];
		if (entities)
		{
			for (auto entity : entities)
			{
				
				EntityID id = entity["Entity"].as<EntityID>();
				EntityRef newEntity = out.NewEntityFromID(id);

				CHROMA_CORE_TRACE("Deserialized Entity with ID = {0}", id);
				auto components = entity["Components"];
				if (components)
				{
					for (auto component : components)
					{
						std::string key = component.first.as<std::string>();
						ComponentRef<Component> newComponent = out.m_ComponentFactory[key](out, newEntity);
						newComponent->Deserialize(component.second);
					}
				}


			}
		}

		return true;



	}

	std::vector<ComponentRef<Component>> Scene::GetAllComponents(EntityRef entity)
	{
		return Scene::GetAllComponents(entity.GetID());
	}

	std::vector<ComponentRef<Component>> Scene::GetAllComponents(EntityID id)
	{
		if (this == nullptr)
			return std::vector<ComponentRef<Component>>();

		std::vector<ComponentRef<Component>> result;
		for (AbstractComponentPool* pool : m_ComponentPools)
		{
			if (pool == nullptr)
				continue;

			for (Component* c : pool->GetAbstractComponents(id))
			{
				result.push_back(ComponentRef<Component>(c, id, this));
			}
		}

		return result;
	}

	EntityRef Scene::ConvertIDToEntity(EntityID e, Scene& scene)
	{
		return EntityRef(e, scene);
	}

	EntityRef Scene::GetEntity(EntityID e)
	{
		return Scene::ConvertIDToEntity(e, *this);
	}

	void Scene::EarlyInit()
	{
		for (System *s : m_Systems)
		{
			s->EarlyInit();
		}
	}

	void Scene::Init()
	{
		for (System* s : m_Systems)
		{
			s->Init();
		}
	}

	void Scene::LateInit()
	{
		for (System* s : m_Systems)
		{
			s->LateInit();
		}
	}

	void Scene::EarlyUpdate(Time delta)
	{
		for (System* s : m_Systems)
		{
			s->EarlyUpdate(delta);
		}
	}

	void Scene::Update(Time delta)
	{
		for (System* s : m_Systems)
		{
			s->Update(delta);
		}
	}

	void Scene::LateUpdate(Time delta)
	{
		for (System* s : m_Systems)
		{
			s->LateUpdate(delta);
		}
	}

	void Scene::EarlyDraw(Time delta)
	{
		for (System* s : m_Systems)
		{
			s->EarlyDraw(delta);
		}
	}

	void Scene::Draw(Time delta)
	{
		for (System* s : m_Systems)
		{
			s->Draw(delta);
		}
	}

	void Scene::LateDraw(Time delta)
	{
		for (System* s : m_Systems)
		{
			s->LateDraw(delta);
		}
	}

}