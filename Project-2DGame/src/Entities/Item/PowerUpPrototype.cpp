#include "PowerUpPrototype.h"
#include "Mushroom.h"
#include "Flower.h"
#include "Hero.h"

PowerUpPrototype::PowerUpPrototype(float x, float y) : Item(x, y) {
}

std::unique_ptr<Item> PowerUpPrototype::clone(Hero* hero) const {
    if (hero->getFormName() == "Small" || hero->getFormName() == "") {
        return std::make_unique<Mushroom>(position.x, position.y);
    } else {
        return std::make_unique<Flower>(position.x, position.y);
    }
}

std::string PowerUpPrototype::getItemType() const {
    return "PowerUpPrototype";
}