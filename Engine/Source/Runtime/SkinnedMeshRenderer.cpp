#include "SkinnedMeshRenderer.hpp"

#include <cassert>

#include "GameObject.hpp"
#include "Debug.hpp"
#include "Scene.hpp"
#include "Mesh.hpp"
#include "Light.hpp"
#include "Animator.hpp"
#include "Pipeline.hpp"
#include "Camera.hpp"
#include "Gizmos.hpp"
#include "Shader.hpp"
#include "Graphics.hpp"

namespace FishEngine
{
    SkinnedMeshRenderer::
        SkinnedMeshRenderer(MaterialPtr material)
        : Renderer(material)
    {

    }


    Bounds SkinnedMeshRenderer::
        localBounds() const {
        return m_sharedMesh->bounds();
    }


    void RecursivelyGetTransformation(
        const TransformPtr&                   transform,
        const std::map<std::string, int>&   nameToIndex,
        std::vector<Matrix4x4>&             outMatrixPalette)
    {
        const auto& name = transform->name();
        const auto& it = nameToIndex.find(name);
        if (it != nameToIndex.end()) {
            const auto boneIndex = it->second;
            outMatrixPalette[boneIndex] = transform->localToWorldMatrixFast();
            outMatrixPalette[boneIndex] = transform->localToWorldMatrix();
        }
        for (auto& child : transform->children()) {
            RecursivelyGetTransformation(child.lock(), nameToIndex, outMatrixPalette);
        }
    }

    void SkinnedMeshRenderer::UpdateMatrixPalette() const
    {
        m_matrixPalette.resize(m_avatar->m_boneToIndex.size());
        RecursivelyGetTransformation(m_rootBone.lock(), m_avatar->m_boneToIndex, m_matrixPalette);
        const auto& invGlobalTransform = gameObject()->transform()->worldToLocalMatrix();
        const auto& bindposes = m_sharedMesh->bindposes();
        for (uint32_t i = 0; i < m_matrixPalette.size(); ++i)
        {
            auto& m = m_matrixPalette[i];
            m = invGlobalTransform * m * bindposes[i];
            // macOS bug
            m = m.transpose();
#if FISHENGINE_PLATFORM_APPLE
#endif
        }
    }
    
    void SkinnedMeshRenderer::Update()
    {
        UpdateMatrixPalette();
    }


    void SkinnedMeshRenderer::Render() const
    {
        auto model = transform()->localToWorldMatrix();
        Pipeline::UpdatePerDrawUniforms(model);

        //std::map<std::string, TexturePtr> textures;
        //auto& lights = Light::lights();
        //if (lights.size() > 0)
        //{
        //    auto& l = lights.front();
        //    if (l->transform() != nullptr)
        //    {
        //        textures["ShadowMap"] = l->m_shadowMap;
        //    }
        //}

        // hack
        // TODO: remove this block
        //bool skinned = m_avatar != nullptr;
        //if (skinned && m_matrixPalette.size() == 0)
        //{
            UpdateMatrixPalette();
        //}
        
        Pipeline::UpdateBonesUniforms(m_matrixPalette);

        for (auto& m : m_materials)
        {
            m->EnableKeyword(ShaderKeyword::SkinnedAnimation);
#if 0
            auto shader = m->shader();
            assert(shader != nullptr);
            shader->Use();
            shader->PreRender();
            m->BindTextures(textures);
            m->BindProperties();
            shader->CheckStatus();
            m_sharedMesh->Render();
            shader->PostRender();
#else
            Graphics::DrawMesh(m_sharedMesh, m);
#endif
            m->DisableKeyword(ShaderKeyword::SkinnedAnimation);
        }
    }

    void SkinnedMeshRenderer::OnDrawGizmosSelected()
    {
        Gizmos::setColor(Color::white);
        Gizmos::setMatrix(transform()->localToWorldMatrix());
        Bounds b = localBounds();
        Gizmos::DrawWireCube(b.center(), b.size());
    }
}
