#pragma once

#include <cereal/access.hpp>
#include <cereal/cereal.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <meta.hpp/headers/meta.hpp/meta_all.hpp>

using namespace std::literals;

// The global scope for getting all registered values
static meta_hpp::scope global_reflection_scope = meta_hpp::static_scope_("global_reflection_scope");

// FOR REFLECTION - When called, the developer defines auto_register_function_ to specify what should be reflected
#define CONCAT_INNER(a, b) a##b
#define CONCAT(a, b) CONCAT_INNER(a, b)

#define REGISTER_AT_RUNTIME                                                        	\
static void auto_register_function_();                                              \
namespace {                                                                         \
	struct auto_register_trigger {													\
		auto_register_trigger() {                                                   \
			auto_register_function_();                                              \
		}                                                                           \
	};                                                                              \
}                                                                                   \
static const auto_register_trigger CONCAT(auto_register_, __LINE__);                \
static void auto_register_function_()


static void auto_register_function_();
#define REGISTRATION_FRIEND friend void ::auto_register_function_();
/*						
#define RTTR_REGISTRATION_FRIEND friend void ::rttr_auto_register_reflection_function_();          \
			template<typename Ctor_Type, typename Policy, typename Accessor, typename Arg_Indexer> \
			friend struct rttr::detail::constructor_invoker;

#define REGISTRATION_FRIEND friend void ::auto_register_function_();                    \
	template < constructor_policy_family Policy, class_kind Class, typename... Args >	\
	friend uvalue meta_hpp::detail::raw_constructor_create(type_registry& registry, std::span<const uarg> args)
*/


// register all user defined serialization
namespace cereal
{
	// glm vectors serialization
	template<class Archive> void serialize(Archive& ar, glm::vec2& v) { ar(cereal::make_nvp("x", v.x), cereal::make_nvp("y", v.y)); }
	template<class Archive> void serialize(Archive& ar, glm::vec3& v) { ar(cereal::make_nvp("x", v.x), cereal::make_nvp("y", v.y), cereal::make_nvp("z", v.z)); }
	template<class Archive> void serialize(Archive& ar, glm::vec4& v) { ar(cereal::make_nvp("x", v.x), cereal::make_nvp("y", v.y), cereal::make_nvp("z", v.z), cereal::make_nvp("w", v.w)); }
	template<class Archive> void serialize(Archive& ar, glm::ivec2& v) { ar(cereal::make_nvp("x", v.x), cereal::make_nvp("y", v.y)); }
	template<class Archive> void serialize(Archive& ar, glm::ivec3& v) { ar(cereal::make_nvp("x", v.x), cereal::make_nvp("y", v.y), cereal::make_nvp("z", v.z)); }
	template<class Archive> void serialize(Archive& ar, glm::ivec4& v) { ar(cereal::make_nvp("x", v.x), cereal::make_nvp("y", v.y), cereal::make_nvp("z", v.z), cereal::make_nvp("w", v.w)); }
	template<class Archive> void serialize(Archive& ar, glm::uvec2& v) { ar(cereal::make_nvp("x", v.x), cereal::make_nvp("y", v.y)); }
	template<class Archive> void serialize(Archive& ar, glm::uvec3& v) { ar(cereal::make_nvp("x", v.x), cereal::make_nvp("y", v.y), cereal::make_nvp("z", v.z)); }
	template<class Archive> void serialize(Archive& ar, glm::uvec4& v) { ar(cereal::make_nvp("x", v.x), cereal::make_nvp("y", v.y), cereal::make_nvp("z", v.z), cereal::make_nvp("w", v.w)); }
	template<class Archive> void serialize(Archive& ar, glm::dvec2& v) { ar(cereal::make_nvp("x", v.x), cereal::make_nvp("y", v.y)); }
	template<class Archive> void serialize(Archive& ar, glm::dvec3& v) { ar(cereal::make_nvp("x", v.x), cereal::make_nvp("y", v.y), cereal::make_nvp("z", v.z)); }
	template<class Archive> void serialize(Archive& ar, glm::dvec4& v) { ar(cereal::make_nvp("x", v.x), cereal::make_nvp("y", v.y), cereal::make_nvp("z", v.z), cereal::make_nvp("w", v.w)); }

	// glm matrices serialization
	template<class Archive> void serialize(Archive& ar, glm::mat2& m) { ar(cereal::make_nvp("col_0", m[0]), cereal::make_nvp("col_1", m[1])); }
	template<class Archive> void serialize(Archive& ar, glm::dmat2& m) { ar(cereal::make_nvp("col_0", m[0]), cereal::make_nvp("col_1", m[1])); }
	template<class Archive> void serialize(Archive& ar, glm::mat3& m) { ar(cereal::make_nvp("col_0", m[0]), cereal::make_nvp("col_1", m[1]), cereal::make_nvp("col_2", m[2])); }
	template<class Archive> void serialize(Archive& ar, glm::mat4& m) { ar(cereal::make_nvp("col_0", m[0]), cereal::make_nvp("col_1", m[1]), cereal::make_nvp("col_2", m[2]), cereal::make_nvp("col_3", m[3])); }
	template<class Archive> void serialize(Archive& ar, glm::dmat4& m) { ar(cereal::make_nvp("col_0", m[0]), cereal::make_nvp("col_1", m[1]), cereal::make_nvp("col_2", m[2]), cereal::make_nvp("col_3", m[3])); }

	template<class Archive> void serialize(Archive& ar, glm::quat& q) { ar(cereal::make_nvp("x", q.x), cereal::make_nvp("y", q.y), cereal::make_nvp("z", q.z), cereal::make_nvp("w", q.w)); }
	template<class Archive> void serialize(Archive& ar, glm::dquat& q) { ar(cereal::make_nvp("x", q.x), cereal::make_nvp("y", q.y), cereal::make_nvp("z", q.z), cereal::make_nvp("w", q.w)); }
}
