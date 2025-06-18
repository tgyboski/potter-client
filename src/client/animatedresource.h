#pragma once

#include "thing.h"
#include <framework/core/timer.h>
#include <framework/graphics/texture.h>
#include <memory>
#include <string>
#include <cmath>
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

class ThingType;
class AnimatedResource;
using AnimatedResourcePtr = std::shared_ptr<AnimatedResource>;

class AnimatedResource : public Thing {
public:
    // Construtor: inicializa a animação entre duas posições, com duração customizável
    AnimatedResource(const Position& from, const Position& to, const std::string& resourceName, int durationMs = 500);

    // Desenha a animação na tela
    void draw(const Point& dest, bool drawThings = true, const LightViewPtr& lightView = nullptr) override;
    // Desenha a luz (não implementado)
    void drawLight(const Point& dest, const LightViewPtr& lightView) override;

    // Identificadores de tipo
    bool isEffect() override;
    bool isItem() override;
    bool isCreature() override;
    bool isMissile() override;
    bool isTopEffect() const;

    // Retorna o ThingType associado (dummy)
    ThingType* getThingType() const override;

    // Retorna ponteiro compartilhado para AnimatedResource
    AnimatedResourcePtr asAnimatedResource();

    // Indica se a animação terminou
    bool isFinished() const { return m_finished; }

private:
    // Agenda a próxima atualização da animação
    void scheduleNextUpdate();

    Position m_from;
    Position m_to;
    Timer m_timer;
    int m_duration;
    std::string m_resourceName;
    TexturePtr m_texture;
    std::shared_ptr<ThingType> m_thingType;
    bool m_finished = false;
}; 