#pragma once


#include "Chroma/Scene/ECS.h"
#include "Chroma/Core/Log.h"
#include <string>
#include "Chroma/Utilities/Yaml.h"
#include "yaml-cpp/node/convert.h"
#include "yaml-cpp/node/node.h"
#include "yaml-cpp/node/parse.h"

namespace Polychrome
{
	class Inspector;
}

namespace Chroma
{

	template <class T>
	class ComponentPool;

	struct Component
	{
		void BeginSerialize(YAML::Emitter& out);
		virtual void Serialize(YAML::Emitter& out) {};
		void EndSerialize(YAML::Emitter& out);

		virtual void Deserialize(YAML::Node& node) {};

		bool operator == (const Component& other)
		{
			return this->GetUniqueID() == other.GetUniqueID();
		}

		const virtual std::string Name() const = 0;

		virtual void DrawImGui() {};

		const virtual bool AllowMultiple() const { return true; };

		const unsigned int GetUniqueID() const
		{
			return comparison_id;
		}

	private:

		const virtual bool IsTag() const { return false; }
		const virtual bool IsTransform() const { return false; }

		unsigned int comparison_id = 0;


		void SetComparisonID()
		{
			comparison_id = component_counter;
			component_counter++;
		}


		static unsigned int component_counter;

		template <class T>
		friend class ComponentPool;
		friend class Polychrome::Inspector;
		friend class Tag;
		friend class Transform;
	};
}

