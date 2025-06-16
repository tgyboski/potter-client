#pragma once

#include "thing.h"
#include <framework/core/timer.h>
#include <string>

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
        
        // Usar um item existente como base (5901 = wood)
        m_thingType = g_things.getThingType(5901, ThingCategoryItem);
        
        // Verifica se o ThingType foi carregado corretamente
        if (!m_thingType) {
            g_logger.error("Failed to load thing type 5901");
        } else {
            // Carrega a textura explicitamente
            m_thingType->getTexture(0);
            
            if (!m_thingType->hasTexture()) {
                g_logger.error("Thing type 5901 has no texture");
            } else {
                g_logger.info("Successfully loaded thing type 5901");
                g_logger.info(stdext::format("Thing type size: %dx%d", m_thingType->getWidth(), m_thingType->getHeight()));
            }
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
        
        // Usa uma função seno para criar um arco
        // O seno vai de -1 a 1, então multiplicamos por metade da altura do arco
        float arcHeight = g_gameConfig.getSpriteSize() * 0.5f; // Altura do arco
        float y = dest.y + (m_to.y - m_from.y) * g_gameConfig.getSpriteSize() * progress;
        y -= std::sin(progress * M_PI) * arcHeight; // Subtrai para que o arco vá para cima
        
        // Tenta desenhar a sprite do item
        if (m_thingType && m_thingType->hasTexture()) {
            // Escala a sprite para metade do tamanho
            float scale = 0.5f;
            Size spriteSize = m_thingType->getSize() * scale;
            
            // Centraliza a sprite na posição
            Point spritePos(x - spriteSize.width() / 2, y - spriteSize.height() / 2);
            
            // Aplica a escala usando g_drawPool
            g_drawPool.scale(scale);
            
            // Desenha a sprite com alpha total
            m_thingType->draw(spritePos, 0, 0, 0, 0, 0, Color::white, drawThings, lightView);
            
            // Reseta a escala para 1.0
            g_drawPool.scale(1.0f);
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
    ThingTypePtr m_thingType;
}; 