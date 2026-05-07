#ifndef ENTITY_H
#define ENTITY_H
#include <string>
#include <vector>
#include "QuadTree/Point.h"
#include "Sprites/Sprite.h"

class Entity{
protected:
    Point position_; // Vecteur de position
    float orientation_; // Angle degré
    std::vector<Sprites::Sprite*> sprites_; // Objet d'affichage (Pointeurs pour polymorphisme)
    // Animation anim; // Animation d'une entité
public:
    Entity(Point position);
    Entity(Point position, float orientation);
    Entity(Point position, Sprites::Sprite* sprite);
    Entity(Point position, float orientation, Sprites::Sprite* sprite);
    Entity(Point position, float orientation, std::vector<Sprites::Sprite*> sprites);
    ~Entity(); // Destructeur nécessaire pour supprimer les sprites

    inline Point getPosition() const {return position_;}
    inline float getOrientation() const {return orientation_;}
    inline std::vector<Sprites::Sprite*>& getSprites() {return sprites_;}

    void addSprite(Sprites::Sprite* sprite);
    void removeSprite(Sprites::Sprite* sprite);

    virtual void draw(SDL_Renderer *win, float deltaTime, Point offset, float scale, float rot);
    virtual void live(float deltaTime);
    
};


#endif