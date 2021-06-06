//
// Created by Ricardo on 4/21/2021.
//

#include <template/template_game.h>
#include <template/ecs/systems/camera_control_system.h>
#include <eagle/application.h>
#include <eagle/window.h>
#include <eagle/random.h>

#include <beagle/ecs/systems/transform_system.h>
#include <beagle/ecs/systems/camera_system.h>
#include <beagle/ecs/systems/render_system.h>
#include <beagle/ecs/systems/mesh_system.h>
#include <beagle/ecs/components/light.h>
#include <beagle/ecs/components/skybox.h>
#include <beagle/ecs/systems/skybox_system.h>

TemplateGame::TemplateGame() {
    EG_LOG_CREATE("template");

    eagle::Random::init();
}

void TemplateGame::calculate_tangent_space(std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices) {

    for (size_t i = 0; i < indices.size(); i += 3){

        uint32_t idx1 = indices[i];
        uint32_t idx2 = indices[i + 1];
        uint32_t idx3 = indices[i + 2];


        glm::vec3 pos1 = vertices[idx1].position;
        glm::vec3 pos2 = vertices[idx2].position;
        glm::vec3 pos3 = vertices[idx3].position;

        glm::vec2 uv1 = vertices[idx1].texCoord;
        glm::vec2 uv2 = vertices[idx2].texCoord;
        glm::vec2 uv3 = vertices[idx3].texCoord;

        glm::vec3 edge1 = pos2 - pos1;
        glm::vec3 edge2 = pos3 - pos1;
        glm::vec2 deltaUV1 = uv2 - uv1;
        glm::vec2 deltaUV2 = uv3 - uv1;

        float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

        glm::vec3 tangent = {
                f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x),
                f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y),
                f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z)
        };

        glm::vec3 bitangent = {
                f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x),
                f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y),
                f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z)
        };

        vertices[idx1].tangent = tangent;
        vertices[idx2].tangent = tangent;
        vertices[idx3].tangent = tangent;

        vertices[idx1].bitangent = bitangent;
        vertices[idx2].bitangent = bitangent;
        vertices[idx3].bitangent = bitangent;
    }
}


void TemplateGame::init(beagle::Engine* engine) {
    EG_TRACE("template", "init called");

    auto& window = eagle::Application::instance().window();
    auto context = window.rendering_context();

    auto pi = 3.141592;

    const size_t resolution = 20;
    std::vector<Vertex> sphereVertices;

    for (uint32_t slice = 0; slice <= resolution; slice++) {
        auto theta = slice *  pi / resolution;
        auto sinTheta = sinf(theta);
        auto cosTheta = cosf(theta);

        for (uint32_t stack = 0; stack <= resolution; stack++) {
            auto phi = stack * 2 * pi / resolution;
            auto sinPhi = sinf(phi);
            auto cosPhi = cosf(phi);
            auto x = (float)(cosPhi * sinTheta);
            auto y = (float)(cosTheta);
            auto z = (float)(sinPhi * sinTheta);
            auto s = 1.0f - ((float)stack / (float)resolution);
            auto t = 1.0f - ((float)slice / (float)resolution);
            auto position = glm::vec3(x, y, z);
            auto normal = glm::normalize(position);
            sphereVertices.emplace_back(Vertex{position, glm::vec2(s, t), normal});
        }
    }

    std::vector<uint32_t> sphereIndices;
    for (int z = 0; z <= resolution; z++) {
        for (int x = 0; x <= resolution; x++) {
            uint32_t zero = x + z * resolution;
            uint32_t one = (x + 1) + z * resolution;
            uint32_t two = x + (z + 1) * resolution;
            uint32_t three = (x + 1) + (z + 1) * resolution;

            sphereIndices.push_back(zero);
            sphereIndices.push_back(one);
            sphereIndices.push_back(three);

            sphereIndices.push_back(zero);
            sphereIndices.push_back(three);
            sphereIndices.push_back(two);
        }
    }

    calculate_tangent_space(sphereVertices, sphereIndices);
    auto sphereMesh = engine->assets().mesh_pool().insert(sphereVertices, sphereIndices);

    struct Plane {
        std::vector<Vertex> vertices = {
                Vertex{glm::vec3(-1.0f, 0.0f, -1.0f), glm::vec2(0.0f, 1.0f), glm::vec3(0.0f, 1.0f, 0.0f)},
                Vertex{glm::vec3(-1.0f, 0.0f, 1.0f), glm::vec2(0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f)},
                Vertex{glm::vec3(1.0f, 0.0f, 1.0f), glm::vec2(1.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f)},
                Vertex{glm::vec3(1.0f, 0.0f, -1.0f), glm::vec2(1.0f, 1.0f), glm::vec3(0.0f, 1.0f, 0.0f)}
        };

        std::vector<uint32_t> indices = {
                0, 1, 2, 0, 2, 3, //down
        };
    } plane;

    calculate_tangent_space(plane.vertices, plane.indices);

    auto planeMesh = engine->assets().mesh_pool().insert(plane.vertices, plane.indices);

    struct Cube {

        std::vector<Vertex> vertices = {
                //up
                Vertex{glm::vec3(-1.0f, -1.0f, -1.0f), glm::vec2(0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)},
                Vertex{glm::vec3(-1.0f, -1.0f, 1.0f), glm::vec2(0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)},
                Vertex{glm::vec3(1.0f, -1.0f, 1.0f), glm::vec2(1.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)},
                Vertex{glm::vec3(1.0f, -1.0f, -1.0f), glm::vec2(1.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)},

                //down
                Vertex{glm::vec3(-1.0f, 1.0f, -1.0f), glm::vec2(0.0f, 1.0f), glm::vec3(0.0f, 1.0f, 0.0f)},
                Vertex{glm::vec3(-1.0f, 1.0f, 1.0f), glm::vec2(0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f)},
                Vertex{glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(1.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f)},
                Vertex{glm::vec3(1.0f, 1.0f, -1.0f), glm::vec2(1.0f, 1.0f), glm::vec3(0.0f, 1.0f, 0.0f)},

                //left
                Vertex{glm::vec3(-1.0f, -1.0f, -1.0f), glm::vec2(0.0f, 1.0f), glm::vec3(1.0f, 0.0f, 0.0f)},
                Vertex{glm::vec3(-1.0f, -1.0f, 1.0f), glm::vec2(0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f)},
                Vertex{glm::vec3(-1.0f, 1.0f, 1.0f), glm::vec2(1.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f)},
                Vertex{glm::vec3(-1.0f, 1.0f, -1.0f), glm::vec2(1.0f, 1.0f), glm::vec3(1.0f, 0.0f, 0.0f)},

                //right
                Vertex{glm::vec3(1.0f, -1.0f, -1.0f), glm::vec2(0.0f, 1.0f), glm::vec3(1.0f, 0.0f, 0.0f)},
                Vertex{glm::vec3(1.0f, -1.0f, 1.0f), glm::vec2(0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f)},
                Vertex{glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(1.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f)},
                Vertex{glm::vec3(1.0f, 1.0f, -1.0f), glm::vec2(1.0f, 1.0f), glm::vec3(1.0f, 0.0f, 0.0f)},

                //front
                Vertex{glm::vec3(-1.0f, -1.0f, 1.0f), glm::vec2(0.0f, 1.0f), glm::vec3(0.0f, 0.0f, 1.0f)},
                Vertex{glm::vec3(-1.0f, 1.0f, 1.0f), glm::vec2(0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)},
                Vertex{glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)},
                Vertex{glm::vec3(1.0f, -1.0f, 1.0f), glm::vec2(1.0f, 1.0f), glm::vec3(0.0f, 0.0f, 1.0f)},

                //back
                Vertex{glm::vec3(-1.0f, -1.0f, -1.0f), glm::vec2(0.0f, 1.0f), glm::vec3(0.0f, 0.0f, -1.0f)},
                Vertex{glm::vec3(-1.0f, 1.0f, -1.0f), glm::vec2(0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)},
                Vertex{glm::vec3(1.0f, 1.0f, -1.0f), glm::vec2(1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)},
                Vertex{glm::vec3(1.0f, -1.0f, -1.0f), glm::vec2(1.0f, 1.0f), glm::vec3(0.0f, 0.0f, -1.0f)}
        };
        std::vector<uint32_t> indices = {
                0, 3, 2, 0, 2, 1, //up
                4, 5, 6, 4, 6, 7, //down
                8, 9, 10, 8, 10, 11, //left
                12, 14, 13, 12, 15, 14, //right
                16, 18, 17, 16, 19, 18, //front
                20, 21, 22, 20, 22, 23, //down
        };
    } cube;

    calculate_tangent_space(cube.vertices, cube.indices);

    auto cubeMesh = engine->assets().mesh_pool().insert(cube.vertices, cube.indices);

    struct Pyramid {
        std::vector<Vertex> vertices {
                Vertex{glm::vec3(-1.0f, -1.0f, 1.0f), glm::vec2(0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)},
                Vertex{glm::vec3(1.0f, -1.0f, 1.0f), glm::vec2(1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)},
                Vertex{glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.5f, 1.0f), glm::vec3(0.0f, 0.0f, 1.0f)},

                Vertex{glm::vec3(1.0f, -1.0f, 1.0f), glm::vec2(0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f)},
                Vertex{glm::vec3(0.0f, -1.0f, -1.0f), glm::vec2(1.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f)},
                Vertex{glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.5f, 1.0f), glm::vec3(1.0f, 0.0f, 0.0f)},

                Vertex{glm::vec3(0.0f, -1.0f, -1.0f), glm::vec2(0.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f)},
                Vertex{glm::vec3(-1.0f, -1.0f, 1.0f), glm::vec2(1.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f)},
                Vertex{glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.5f, 1.0f), glm::vec3(-1.0f, 0.0f, 0.0f)},

                Vertex{glm::vec3(-1.0f, -1.0f, 1.0f), glm::vec2(0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)},
                Vertex{glm::vec3(0.0f, -1.0f, -1.0f), glm::vec2(1.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)},
                Vertex{glm::vec3(1.0f, -1.0f, 1.0f), glm::vec2(0.5f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)},
        };
        std::vector<uint32_t> indices = {
                0, 1, 2,
                3, 4, 5,
                6, 7, 8,
                9, 10, 11
        };
    } pyramid;

    calculate_tangent_space(pyramid.vertices, pyramid.indices);

    auto pyramidMesh = engine->assets().mesh_pool().insert(pyramid.vertices, pyramid.indices);

    engine->assets().mesh_pool().upload();


    eagle::ShaderCreateInfo shaderCreateInfo = {
            context->main_render_pass(),
            {
                    {eagle::ShaderStage::VERTEX, "shaders/mesh.vert.spv"},
                    {eagle::ShaderStage::FRAGMENT, "shaders/pbr/pbr_mesh.frag.spv"},
            }
    };
    //position
    shaderCreateInfo.vertexLayout.add(0, eagle::Format::R32G32B32_SFLOAT);

    //texcoord
    shaderCreateInfo.vertexLayout.add(0, eagle::Format::R32G32_SFLOAT);

    //normal
    shaderCreateInfo.vertexLayout.add(0, eagle::Format::R32G32B32_SFLOAT);

    //tangent
    shaderCreateInfo.vertexLayout.add(0, eagle::Format::R32G32B32_SFLOAT);

    //bitangent
    shaderCreateInfo.vertexLayout.add(0, eagle::Format::R32G32B32_SFLOAT);

    //transform matrix
    //build a mat4 using 4 vec4
    shaderCreateInfo.vertexLayout.add(1, eagle::Format::R32G32B32A32_SFLOAT);
    shaderCreateInfo.vertexLayout.add(1, eagle::Format::R32G32B32A32_SFLOAT);
    shaderCreateInfo.vertexLayout.add(1, eagle::Format::R32G32B32A32_SFLOAT);
    shaderCreateInfo.vertexLayout.add(1, eagle::Format::R32G32B32A32_SFLOAT);

    //inverse transform
    shaderCreateInfo.vertexLayout.add(1, eagle::Format::R32G32B32A32_SFLOAT);
    shaderCreateInfo.vertexLayout.add(1, eagle::Format::R32G32B32A32_SFLOAT);
    shaderCreateInfo.vertexLayout.add(1, eagle::Format::R32G32B32A32_SFLOAT);
    shaderCreateInfo.vertexLayout.add(1, eagle::Format::R32G32B32A32_SFLOAT);

    shaderCreateInfo.vertexLayout[1].inputRate = eagle::VertexInputRate::INSTANCE;
    shaderCreateInfo.depthTesting = true;
    shaderCreateInfo.blendEnable = true;
    auto shader = engine->assets().shader_pool().insert(shaderCreateInfo, "pbr");


    eagle::ShaderCreateInfo skyboxShaderCreateInfo = {
            context->main_render_pass(),
            {
                    {eagle::ShaderStage::VERTEX, "shaders/skybox/skybox.vert.spv"},
                    {eagle::ShaderStage::FRAGMENT, "shaders/skybox/skybox.frag.spv"},
            }
    };
    //position
    skyboxShaderCreateInfo.vertexLayout.add(0, eagle::Format::R32G32B32_SFLOAT);
    skyboxShaderCreateInfo.depthTesting = true;
    auto skyboxShader = engine->assets().shader_pool().insert(skyboxShaderCreateInfo, "skybox");


    auto hdrImage = engine->assets().texture_pool().insert("images/skybox/sunrise.hdr");

    eagle::TextureCreateInfo cubemapCreateInfo = {};
    cubemapCreateInfo.imageCreateInfo.width = 512;
    cubemapCreateInfo.imageCreateInfo.height = 512;
    cubemapCreateInfo.imageCreateInfo.format = eagle::Format::R32G32B32A32_SFLOAT;
    cubemapCreateInfo.imageCreateInfo.mipLevels = 6;
    cubemapCreateInfo.imageCreateInfo.arrayLayers = 6;
    cubemapCreateInfo.imageCreateInfo.tiling = eagle::ImageTiling::OPTIMAL;
    cubemapCreateInfo.imageCreateInfo.memoryProperties = {eagle::MemoryProperty::DEVICE_LOCAL};
    cubemapCreateInfo.imageCreateInfo.aspects = {eagle::ImageAspect::COLOR};
    cubemapCreateInfo.imageCreateInfo.usages = {eagle::ImageUsage::SAMPLED, eagle::ImageUsage::TRANSFER_DST, eagle::ImageUsage::TRANSFER_SRC, eagle::ImageUsage::STORAGE};
    cubemapCreateInfo.imageCreateInfo.layout = eagle::ImageLayout::GENERAL;
    cubemapCreateInfo.filter = eagle::Filter::LINEAR;
    cubemapCreateInfo.imageCreateInfo.type = eagle::ImageType::CUBE;
    auto skyboxImage = engine->assets().texture_pool().insert(cubemapCreateInfo);

    auto computeShader = context->create_compute_shader("shaders/compute/unwrap_skybox.comp.spv");
    computeShader->update_image(0, skyboxImage->get()->image());
    computeShader->update_image(1, *hdrImage);
    computeShader->update_descriptors();

    computeShader->dispatch(512 / 8, 512 / 8, 1);

    computeShader->join();
    skyboxImage->get()->image()->generate_mipmaps();

    eagle::TextureCreateInfo irradianceMapCreateInfo = {};
    irradianceMapCreateInfo.imageCreateInfo.width = 32;
    irradianceMapCreateInfo.imageCreateInfo.height = 32;
    irradianceMapCreateInfo.imageCreateInfo.format = eagle::Format::R32G32B32A32_SFLOAT;
    irradianceMapCreateInfo.imageCreateInfo.mipLevels = 1;
    irradianceMapCreateInfo.imageCreateInfo.arrayLayers = 6;
    irradianceMapCreateInfo.imageCreateInfo.tiling = eagle::ImageTiling::OPTIMAL;
    irradianceMapCreateInfo.imageCreateInfo.memoryProperties = {eagle::MemoryProperty::DEVICE_LOCAL};
    irradianceMapCreateInfo.imageCreateInfo.aspects = {eagle::ImageAspect::COLOR};
    irradianceMapCreateInfo.imageCreateInfo.usages = {eagle::ImageUsage::SAMPLED, eagle::ImageUsage::STORAGE};
    irradianceMapCreateInfo.imageCreateInfo.layout = eagle::ImageLayout::GENERAL;
    irradianceMapCreateInfo.filter = eagle::Filter::LINEAR;
    irradianceMapCreateInfo.imageCreateInfo.type = eagle::ImageType::CUBE;
    auto irradianceMap = engine->assets().texture_pool().insert(irradianceMapCreateInfo);

    auto irradiannceComputeShader = context->create_compute_shader("shaders/compute/cubemap_irradiance.comp.spv");
    irradiannceComputeShader->update_image(0, irradianceMap->get()->image());
    irradiannceComputeShader->update_image(1, *skyboxImage);
    irradiannceComputeShader->update_descriptors();

    irradiannceComputeShader->dispatch(32 / 8, 32 / 8, 1);

    irradiannceComputeShader->join();

    eagle::TextureCreateInfo prefilterMapCreateInfo = {};
    prefilterMapCreateInfo.imageCreateInfo.width = 128;
    prefilterMapCreateInfo.imageCreateInfo.height = 128;
    prefilterMapCreateInfo.imageCreateInfo.format = eagle::Format::R32G32B32A32_SFLOAT;
    prefilterMapCreateInfo.imageCreateInfo.mipLevels = 1;
    prefilterMapCreateInfo.imageCreateInfo.arrayLayers = 6;
    prefilterMapCreateInfo.imageCreateInfo.tiling = eagle::ImageTiling::OPTIMAL;
    prefilterMapCreateInfo.imageCreateInfo.memoryProperties = {eagle::MemoryProperty::DEVICE_LOCAL};
    prefilterMapCreateInfo.imageCreateInfo.aspects = {eagle::ImageAspect::COLOR};
    prefilterMapCreateInfo.imageCreateInfo.usages = {eagle::ImageUsage::SAMPLED, eagle::ImageUsage::STORAGE};
    prefilterMapCreateInfo.imageCreateInfo.layout = eagle::ImageLayout::GENERAL;
    prefilterMapCreateInfo.filter = eagle::Filter::LINEAR;
    prefilterMapCreateInfo.imageCreateInfo.type = eagle::ImageType::CUBE;
    auto prefilterMap = engine->assets().texture_pool().insert(prefilterMapCreateInfo);
    auto prefilterComputeShader = context->create_compute_shader("shaders/compute/cubemap_prefilter.comp.spv");
    prefilterComputeShader->update_image(0, prefilterMap->get()->image());
    prefilterComputeShader->update_image(1, *skyboxImage);
    prefilterComputeShader->update_descriptors();

    prefilterComputeShader->dispatch(128 / 8, 128 / 8, 1);

    prefilterComputeShader->join();

    eagle::TextureCreateInfo brdfMapCreateInfo = {};
    brdfMapCreateInfo.imageCreateInfo.width = 512;
    brdfMapCreateInfo.imageCreateInfo.height = 512;
    brdfMapCreateInfo.imageCreateInfo.format = eagle::Format::R16G16_SFLOAT;
    brdfMapCreateInfo.imageCreateInfo.mipLevels = 1;
    brdfMapCreateInfo.imageCreateInfo.arrayLayers = 1;
    brdfMapCreateInfo.imageCreateInfo.tiling = eagle::ImageTiling::OPTIMAL;
    brdfMapCreateInfo.imageCreateInfo.memoryProperties = {eagle::MemoryProperty::DEVICE_LOCAL};
    brdfMapCreateInfo.imageCreateInfo.aspects = {eagle::ImageAspect::COLOR};
    brdfMapCreateInfo.imageCreateInfo.usages = {eagle::ImageUsage::SAMPLED, eagle::ImageUsage::STORAGE};
    brdfMapCreateInfo.imageCreateInfo.layout = eagle::ImageLayout::GENERAL;
    brdfMapCreateInfo.filter = eagle::Filter::LINEAR;
    auto brdfMap = engine->assets().texture_pool().insert(brdfMapCreateInfo);
    auto integrateBrdfComputeShader = context->create_compute_shader("shaders/compute/integrate_brdf.comp.spv");
    integrateBrdfComputeShader->update_image(0, brdfMap->get()->image());
    integrateBrdfComputeShader->update_descriptors();

    integrateBrdfComputeShader->dispatch(512 / 8, 512 / 8, 1);

    integrateBrdfComputeShader->join();

    struct MaterialData {
        alignas(16) glm::vec4 albedo;
        float metallic;
        float roughness;
        float ao;
    };


    auto metalMeshAlbedo = engine->assets().texture_pool().insert("images/metal_2/Metal_Tiles_004_basecolor.jpg");
    auto metalMeshMetallic = engine->assets().texture_pool().insert("images/metal_2/Metal_Tiles_004_metallic.jpg");
    auto metalMeshRoughness = engine->assets().texture_pool().insert("images/metal_2/Metal_Tiles_004_roughness.jpg");
    auto metalMeshAO = engine->assets().texture_pool().insert("images/metal_2/Metal_Tiles_004_ambientOcclusion.jpg");
    auto metalMeshNormal = engine->assets().texture_pool().insert("images/metal_2/Metal_Tiles_004_normal.jpg");
    auto woodTexture = engine->assets().texture_pool().insert("images/wood.png");

//    auto cubemap = engine->assets().texture_pool().insert({
//        "images/skybox/right.jpg",
//        "images/skybox/left.jpg",
//        "images/skybox/top.jpg",
//        "images/skybox/bottom.jpg",
//        "images/skybox/front.jpg",
//        "images/skybox/back.jpg"
//    });
    auto skyboxMaterial = engine->assets().material_pool().insert(skyboxShader);
    skyboxMaterial->update_texture(0, prefilterMap);


    auto metalicMaterial = engine->assets().material_pool().insert(shader);
    auto woodMaterial = engine->assets().material_pool().insert(shader);

    shaderCreateInfo.shaderStages[eagle::ShaderStage::FRAGMENT] = "shaders/mesh.frag.spv";
    auto colorShader = engine->assets().shader_pool().insert(shaderCreateInfo, "default");
    auto lightMaterial = engine->assets().material_pool().insert(colorShader);

    lightMaterial->update_uniform(0, glm::vec3(1.0f));


    metalicMaterial->update_uniform(0, MaterialData{glm::vec4(0.7f, 0.3f, 0.3f, 1.0f), 1.0f, 0.7f, 0.2f});

    woodMaterial->update_uniform(0, MaterialData{glm::vec4(0.2f, 0.7f, 0.5f, 1.0f), 0.1f, 0.6f, 8.0f});
    woodMaterial->update_texture(1, woodTexture);
    woodMaterial->update_texture(6, irradianceMap);
    woodMaterial->update_texture(7, prefilterMap);
    woodMaterial->update_texture(8, brdfMap);




    const int entityCount = 100;
    const float range = 30.0f;

    for (int i = 0; i < entityCount; i++){

        auto material = engine->assets().material_pool().insert(shader);
        material->update_uniform(0, MaterialData{glm::vec4(1.0f, 0.3f, 0.3f, 1.0f), float(i % 10) / 9.0f, float(i / 10) / 9.0f, 1.0f});
//        material->update_texture(1, metalMeshAlbedo);
//        material->update_texture(2, metalMeshMetallic);
//        material->update_texture(3, metalMeshRoughness);
//        material->update_texture(4, metalMeshAO);
        material->update_texture(5, metalMeshNormal);
        material->update_texture(6, irradianceMap);
        material->update_texture(7, prefilterMap);
        material->update_texture(8, brdfMap);

        auto e = engine->entities().create();
        e.assign<beagle::Position>((i % 10) * 3, (i / 10) * 3, 0);
        e.assign<beagle::Rotation>();
        e.assign<beagle::Scale>();
        if (i % 2){
            e.assign<Rotator>(glm::vec3(eagle::Random::range(-20.0f, 20.0f), eagle::Random::range(-20.0f, 20.0f), eagle::Random::range(-20.0f, 20.0f)));
        }
        e.assign<beagle::Transform>();
        e.assign<beagle::MeshRenderer>(sphereMesh, material);
    }

//    for (int i  = 0; i < 8; i++){
//        auto e = engine->entities().create();
//        auto position = e.assign<beagle::Position>((i % 4) * 5 + 10, (i / 4) * 5 + 10, -20);
//        e.assign<beagle::PointLight>(glm::vec3(1.0f), 32.0f);
//        e.assign<beagle::Scale>(0.5f);
//        e.assign<beagle::Transform>();
//        auto oscilator = e.assign<Oscilator>(position->vec);
//        oscilator->frequency = eagle::Random::value() * 0.2f;
//        e.assign<beagle::MeshRenderer>(sphereMesh, lightMaterial);
//    }

    auto e = engine->entities().create();
    e.assign<beagle::MeshRenderer>(planeMesh, woodMaterial);
    e.assign<beagle::Position>(0, -range, 0);
    e.assign<beagle::Scale>(40, 1, 40);
    e.assign<beagle::Transform>();
//
//    e = engine->entities().create();
//    e.assign<beagle::DirectionalLight>();
//    e.assign<beagle::Rotation>(30, 40, 0);

    e = engine->entities().create();
    e.assign<beagle::Position>(0.0f, 0.0f, 50.0f);
//    e.assign<beagle::DirectionalLight>(glm::vec3(0.0f, 0.0f, 1.0f));
    e.assign<beagle::Rotation>();
    e.assign<beagle::CameraPerspectiveProjection>(glm::radians(60.0f), window.width() / window.height(), 0.1f, 1000.0f);
    e.assign<beagle::CameraProjection>();
    e.assign<beagle::Transform>();
    auto camera = e.assign<beagle::Camera>(context);
    auto meshFilter = e.assign<beagle::MeshFilter>(context, &engine->assets().mesh_pool());
    auto skyboxFilter = e.assign<beagle::SkyboxFilter>(context, skyboxMaterial);
    camera->secondaryCommandBuffers.emplace_back(meshFilter->commandBuffer);
    camera->secondaryCommandBuffers.emplace_back(skyboxFilter->commandBuffer);
    auto controller = e.assign<CameraController>(&eagle::Application::instance().event_bus());
    controller->speed = 10.0f;
    controller->mouseSpeed = 32.0f;


    auto transformSystem = engine->systems().attach<beagle::TransformSystem>();
    auto skyboxFilterSystem = engine->systems().attach<beagle::SkyboxFilterSystem>();
    auto renderSystem = engine->systems().attach<beagle::RenderSystem>();
    auto meshFilterSystem = engine->systems().attach<beagle::MeshFilterSystem>();
    auto cameraSystem = engine->systems().attach<beagle::CameraSystem>();


    m_oscilatorGroup.attach(&engine->entities());
    m_scalerGroup.attach(&engine->entities());
    m_rotatorGroup.attach(&engine->entities());
    m_quadsGroup.attach(&engine->entities());


    auto rotatorJob = engine->jobs().enqueue<beagle::Job>([this, engine]{
        float t = engine->timer().time();
        for (auto[rotation, rotator] : m_rotatorGroup){
            rotation->quat = glm::quat(glm::radians(t * rotator->frequency));
        }
        return beagle::JobResult::SUCCESS;
    }, "Rotator");

    auto oscilatorJob = engine->jobs().enqueue<beagle::Job>([this, engine]{
        float t = engine->timer().time();

        for (auto[tr, osc] : m_oscilatorGroup){
            tr->vec.x = osc->anchor.x + sinf(t * osc->frequency) * osc->amplitude;
        }
        return beagle::JobResult::SUCCESS;
    }, "Oscilator");

    auto scalerJob = engine->jobs().enqueue<beagle::Job>([this, engine]{
        float t = engine->timer().time();

        for (auto[tr, osc] : m_scalerGroup){
            auto amplitude = osc->amplitude;
            auto frequency = osc->frequency;
            auto scale = tr->vec;
            scale = glm::vec3(1) * abs((sinf(frequency * t) * amplitude));
            tr->vec = scale;
        }
        return beagle::JobResult::SUCCESS;
    }, "Scaler");
    auto cameraControllerJob = engine->jobs().enqueue<CameraControlJob>(&engine->entities(), &engine->timer());
    transformSystem->updateMatricesJob.run_after(cameraControllerJob);
    transformSystem->updateMatricesJob.run_after(scalerJob);
    transformSystem->updateMatricesJob.run_after(oscilatorJob);
    transformSystem->updateMatricesJob.run_after(rotatorJob);

    meshFilterSystem->updateInstanceBufferJob.run_after(transformSystem->updateMatricesJob);
    meshFilterSystem->updateVertexUboJob.run_after(transformSystem->updateMatricesJob);
    meshFilterSystem->updateVertexUboJob.run_after(cameraSystem->updatePerspectiveProjectionJob);
    meshFilterSystem->updateFragmentUboJob.run_after(cameraControllerJob);

    skyboxFilterSystem->updateVertexUboJob.run_after(cameraSystem->updatePerspectiveProjectionJob);
    skyboxFilterSystem->updateVertexUboJob.run_after(transformSystem->updateMatricesJob);

    renderSystem->beginJob.run_after(meshFilterSystem->updateVertexUboJob);
    renderSystem->beginJob.run_after(meshFilterSystem->updateFragmentUboJob);
    renderSystem->beginJob.run_after(meshFilterSystem->updateInstanceBufferJob);
    renderSystem->beginJob.run_after(skyboxFilterSystem->updateVertexUboJob);

    meshFilterSystem->renderJob.run_after(renderSystem->beginJob);
    skyboxFilterSystem->renderJob.run_after(renderSystem->beginJob);

    cameraSystem->renderJob.run_after(meshFilterSystem->renderJob);
    cameraSystem->renderJob.run_after(skyboxFilterSystem->renderJob);

    renderSystem->endJob.run_after(cameraSystem->renderJob);
}

void TemplateGame::step(beagle::Engine* engine) {

}

void TemplateGame::destroy(beagle::Engine* engine) {
    EG_TRACE("template", "destroy called");
}

