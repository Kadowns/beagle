//
// Created by Ricardo on 5/4/2021.
//

#include "asset_manager.h"

using namespace beagle;

AssetManager::AssetManager(eagle::RenderingContext* context) :
    m_texturePool(context),
    m_shaderPool(context),
    m_materialPool(context, &m_texturePool),
    m_meshPool(context) {

}
