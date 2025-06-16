#pragma once

#include "thing.h"
#include <framework/core/timer.h>
#include <string>
#include <framework/graphics/texture.h>
#include <framework/graphics/texturemanager.h>
#include <framework/core/resourcemanager.h>

#include <cmath>
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

class AnimatedResource;
using AnimatedResourcePtr = std::shared_ptr<AnimatedResource>;

class AnimatedResource : public Thing {
public:
    AnimatedResource(const Position& from, const Position& to) 
        : m_from(from), m_to(to) {
        m_timer.restart();
        scheduleNextUpdate();
        
        // Carrega a textura mockada wood.png diretamente da pasta images/resources
        m_texture = g_textures.getTexture("/images/resources/wood.png");
        if (!m_texture) {
            g_logger.error("Failed to load texture images/resources/wood.png");
        } else {
            g_logger.info("Successfully loaded texture images/resources/wood.png");
        }
        // Carrega um ThingType dummy apenas para evitar crash em getThingType
        m_thingType = g_things.getThingType(5901, ThingCategoryItem);
        if (!m_thingType) {
            g_logger.error("Failed to load thing type 5901");
        }
    }

    void draw(const Point& dest, bool drawThings = true, const LightViewPtr& lightView = nullptr) override {
        float progress = m_timer.ticksElapsed() / static_cast<float>(m_duration);
        if (progress >= 1.0f) {
            g_map.removeThing(std::static_pointer_cast<Thing>(asLuaObject()));
            return;
        }
        
        // Calcula a posição atual
        float x = dest.x + (m_to.x - m_from.x) * g_gameConfig.getSpriteSize() * progress;
        float arcHeight = g_gameConfig.getSpriteSize() * 0.5f; // Altura do arco
        float y = dest.y + (m_to.y - m_from.y) * g_gameConfig.getSpriteSize() * progress;
        y -= std::sin(progress * M_PI) * arcHeight;
        
        if (m_texture) {
            // Centraliza a imagem na posição
            Point pos(x, y);
            Size size = m_texture->getSize();
            Rect destRect(pos, size);
            Rect srcRect(Point(0, 0), size);
            g_drawPool.addTexturedRect(destRect, m_texture, srcRect, Color::white);
        } else {
            // Fallback: desenha um quadrado verde
            // g_drawPool.addFilledRect(Rect(Point(x - 2, y - 2), Size(5, 5)), Color::green);
        }
    }

    void drawLight(const Point& dest, const LightViewPtr& lightView) override {}

    bool isEffect() override { return true; }
    bool isItem() override { return false; }
    bool isCreature() override { return false; }
    bool isMissile() override { return false; }

    ThingType* getThingType() const override { return m_thingType.get(); }

    AnimatedResourcePtr asAnimatedResource() { return std::static_pointer_cast<AnimatedResource>(shared_from_this()); }

private:
    void scheduleNextUpdate() {
        if (m_timer.ticksElapsed() < m_duration) {
            g_dispatcher.addEvent([this]() {
                scheduleNextUpdate();
            });
        }
    }

    Position m_from;
    Position m_to;
    Timer m_timer;
    const int m_duration = 500; // 500ms de duração
    TexturePtr m_texture;
    ThingTypePtr m_thingType;
}; 