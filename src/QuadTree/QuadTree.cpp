#include "QuadTree.h"

QuadTree::QuadTree(Rectangle boundary) : 
    boundary_{boundary},
    topLeftTree_{nullptr},
    topRightTree_{nullptr},
    botLeftTree_{nullptr},
    botRightTree_{nullptr},
    lst_enemy_{},
    divided_{false}
    {}

void QuadTree::subDivide(){
    float x_box = boundary_.getX();
    float y_box = boundary_.getY();
    float w_box = boundary_.getW();
    float h_box = boundary_.getH();

    Rectangle tl{x_box - w_box / 4 , y_box - h_box / 4 , w_box / 2 , h_box / 2}; // Top Right 
    Rectangle tr{x_box + w_box / 4 , y_box - h_box / 4 , w_box / 2 , h_box / 2}; // Top left 
    Rectangle bl{x_box - w_box / 4 , y_box + h_box / 4 , w_box / 2 , h_box / 2}; // Bot Right
    Rectangle br{x_box + w_box / 4 , y_box + h_box / 4 , w_box / 2 , h_box / 2}; // Bot left

    topLeftTree_ = std::make_unique<QuadTree>(tl);
    topRightTree_ = std::make_unique<QuadTree>(tr);
    botLeftTree_ = std::make_unique<QuadTree>(bl);
    botRightTree_ = std::make_unique<QuadTree>(br);

    // Redistribuer les ennemies dans chaque section 
    for (const auto& e : lst_enemy_) {
        Point p = e->getPosition();
        if (topLeftTree_->boundary_.contains(p))      
            topLeftTree_->insert(e);
        else if (topRightTree_->boundary_.contains(p)) 
            topRightTree_->insert(e);
        else if (botLeftTree_->boundary_.contains(p))  
            botLeftTree_->insert(e);
        else if (botRightTree_->boundary_.contains(p)) 
            botRightTree_->insert(e);
    }

    lst_enemy_.clear(); // vide la liste points

}

void QuadTree::insert(Enemy* e){

    Point point = e->getPosition();
    if(!boundary_.contains(point)){
        return;
    }

    // If this node is already divided, pass the point down to the correct child.
    if (divided_) {
        if (topLeftTree_->boundary_.contains(point))
            topLeftTree_->insert(e);
        else if (topRightTree_->boundary_.contains(point))
            topRightTree_->insert(e);
        else if (botLeftTree_->boundary_.contains(point))
            botLeftTree_->insert(e);
        else if (botRightTree_->boundary_.contains(point))
            botRightTree_->insert(e);
        return;
    }

    // This is a leaf node, so add the point.
    lst_enemy_.push_back(e);

    // If capacity is now exceeded, subdivide and redistribute all points.
    if(lst_enemy_.size() > capacity_) {
        subDivide();
        divided_ = true;
    }
}

std::vector<Enemy*> QuadTree::query(Point center, float range){
    
    std::vector<Enemy*> res;

    if(!boundary_.checkOverlap(center,range)){ // Pas d'intersection entre le cercle et le rectangle
        return res; // liste vide 
    } else {
        for(const auto& e : lst_enemy_){ 
            Point p = e->getPosition();
            float dx = p.getX() - center.getX();
            float dy = p.getY() - center.getY();
            if (dx*dx + dy*dy <= range*range) {
                res.push_back(e);
            }
        }
    }

    if(divided_){ // Si cela est divisé
        auto tl = topLeftTree_->query(center, range);
        res.insert(res.end(), tl.begin(), tl.end());

        auto tr = topRightTree_->query(center, range);
        res.insert(res.end(), tr.begin(), tr.end());

        auto bl = botLeftTree_->query(center, range);
        res.insert(res.end(), bl.begin(), bl.end());

        auto br = botRightTree_->query(center, range);
        res.insert(res.end(), br.begin(), br.end());

    }

    return res;
}

void QuadTree::remove(Enemy* e){

    // On cherche dans le noeud actuel l'ennemy
    auto find = std::find(lst_enemy_.begin(),lst_enemy_.end(),e);
    if(find != lst_enemy_.end()) {
        lst_enemy_.erase(find);
        return;
    }

    if (!divided_) return;

    // Sinon on le cherche dans les 4 enfants
    topLeftTree_->remove(e); 
    topRightTree_->remove(e); 
    botLeftTree_->remove(e); 
    botRightTree_->remove(e);

}

void QuadTree::print(int level) const {

    // indentation simple
    for (int i = 0; i < level; i++)
        std::cout << "  ";

    // afficher le noeud
    std::cout << "Node("
              << "center=" << boundary_.getX() << "," << boundary_.getY()
              << " size=" << boundary_.getW() << "," << boundary_.getH()
              << " points=" << lst_enemy_.size() << ")";

    if (!lst_enemy_.empty()) {
        std::cout << " [ ";
        for (const auto& e : lst_enemy_) {
            Point p = e->getPosition();
            std::cout << "(" << p.getX() << "," << p.getY() << ") ";
        }

        std::cout << "]";
    }

    std::cout << "\n";

    // si pas subdivisé → stop
    if (!divided_)
        return;

    // enfants
    topLeftTree_->print(level + 1);
    topRightTree_->print(level + 1);
    botLeftTree_->print(level + 1);
    botRightTree_->print(level + 1);
}
