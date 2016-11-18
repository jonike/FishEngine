#include "Object.hpp"
#include "Scene.hpp"
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>

namespace FishEngine
{

    Object::Object()
        : m_uuid(boost::uuids::random_generator()())
    {
        
    }
    
    Object::~Object() = default;

    void Object::Destroy(GameObjectPtr obj, const float t /*= 0.0f*/) {
        
        Scene::Destroy(obj, t);
    }


    void Object::Destroy(ComponentPtr component, float t /*= 0.0f*/)
    {
        Scene::Destroy(component, t);
    }


    void Object::Destroy(ScriptPtr script, float t /*= 0.0f*/)
    {
        Scene::Destroy(script, t);
    }


    void Object::DestroyImmediate(ScriptPtr script)
    {
        Scene::DestroyImmediate(script);
    }

    void Object::DestroyImmediate(ComponentPtr component)
    {
        Scene::DestroyImmediate(component);
    }

    void Object::DestroyImmediate(GameObjectPtr obj)
    {
        Scene::DestroyImmediate(obj);
    }

}
