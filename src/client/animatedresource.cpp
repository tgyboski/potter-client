#include "animatedresource.h"
#include "game.h"
#include "gameconfig.h"
#include "map.h"
#include <framework/graphics/texturemanager.h>
#include <framework/core/resourcemanager.h>
#include <framework/graphics/drawpoolmanager.h>
#include <framework/core/logger.h>
#include <framework/core/eventdispatcher.h>
#include <cmath>

AnimatedResource::AnimatedResource(const Position& from, const Position& to, int durationMs)
    : m_from(from), m_to(to), m_duration(durationMs)
{
    m_timer.restart();
    scheduleNextUpdate();

    // Carrega a textura da animação
    m_texture = g_textures.getTexture("/images/resources/wood.png");
    if (!m_texture) {
        g_logger.error("Failed to load texture /images/resources/wood.png");
    }
    // Carrega um ThingType dummy apenas para evitar crash em getThingType
    m_thingType = g_things.getThingType(1, ThingCategoryEffect);
    if (!m_thingType) {
        g_logger.error("Failed to load thing type wood");
    }
}

void AnimatedResource::draw(const Point& dest, bool drawThings, const LightViewPtr& lightView)
{
    float progress = m_timer.ticksElapsed() / static_cast<float>(m_duration);
    if (progress >= 1.0f) {
        m_finished = true;
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

void AnimatedResource::drawLight(const Point& dest, const LightViewPtr& lightView)
{
    // Não implementado
}

bool AnimatedResource::isEffect() { return true; }
bool AnimatedResource::isItem() { return false; }
bool AnimatedResource::isCreature() { return false; }
bool AnimatedResource::isMissile() { return false; }
bool AnimatedResource::isTopEffect() const { return true; }

ThingType* AnimatedResource::getThingType() const { return m_thingType.get(); }

AnimatedResourcePtr AnimatedResource::asAnimatedResource() {
    return std::static_pointer_cast<AnimatedResource>(shared_from_this());
}

void AnimatedResource::scheduleNextUpdate()
{
    if (m_timer.ticksElapsed() < m_duration) {
        g_dispatcher.addEvent([this]() {
            scheduleNextUpdate();
        });
    }
}
