#include "QuadTree.h"

QuadTree::QuadTree(Rectangle boundary) : 
    boundary_{boundary},
    topLeftTree_{nullptr},
    topRightTree_{nullptr},
    botLeftTree_{nullptr},
    botRightTree_{nullptr},
    points_{},
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

    // Redistribuer les points dans chaque section 
    for (Point p : points_) {
        if (topLeftTree_->boundary_.contains(p))      
            topLeftTree_->insert(p);
        else if (topRightTree_->boundary_.contains(p)) 
            topRightTree_->insert(p);
        else if (botLeftTree_->boundary_.contains(p))  
            botLeftTree_->insert(p);
        else if (botRightTree_->boundary_.contains(p)) 
            botRightTree_->insert(p);
    }

    points_.clear(); // vide la liste points

}

void QuadTree::insert(Enemy e){

    Point point = e.getPosition();
    if(!boundary_.contains(p)){ // les cordoonnées du points n'appartient pas au rectangle
        return;
    }

    // Si jamais il n'y a aucun point dans le rectangle et qu'il n'est pas déjà divisé, on l'insere dans la liste des points 
    if(points_.size() < capacity_ && !divided_) {      
        points_.push_back(point); 
        return;
    } 

    if (!divided_){ // Si il n'est toujours pas divisé, on le divise
        subDivide();
        divided_ = true;
    }

    // Trouver dans quel sous-rectangles placer le point qu'on veut insérer
    if (topLeftTree_->boundary_.contains(point))      
        topLeftTree_->insert(point);
    else if (topRightTree_->boundary_.contains(point)) 
        topRightTree_->insert(point);
    else if (botLeftTree_->boundary_.contains(point))  
        botLeftTree_->insert(point);
    else if (botRightTree_->boundary_.contains(point)) 
        botRightTree_->insert(point);
    
}

std::vector<Point> QuadTree::query(Tower t){

    Point center = t.getPosition();
    float range = t.getRange();
    std::vector<Point> res;
    if(!boundary_.checkOverlap(center,range)){ // Pas d'intersection entre le cercle et le rectangle
        return res; // liste vide 
    } else {
        for(Point p : points_){ 
            float dx = p.getX() - center.getX();
            float dy = p.getY() - center.getY();
            if (dx*dx + dy*dy <= range*range) {
                res.push_back(p);
            }
        }
    }

    if(divided_){ // Si cela est divisé
        std::vector<Point> tl = topLeftTree_->query(center, range);
        res.insert(res.end(), tl.begin(), tl.end());

        std::vector<Point> tr = topRightTree_->query(center, range);
        res.insert(res.end(), tr.begin(), tr.end());

        std::vector<Point> bl = botLeftTree_->query(center, range);
        res.insert(res.end(), bl.begin(), bl.end());

        std::vector<Point> br = botRightTree_->query(center, range);
        res.insert(res.end(), br.begin(), br.end());

    }

    return res;
}

void QuadTree::remove(Enemy e){
    
}

void QuadTree::print(int level) const {

    // indentation simple
    for (int i = 0; i < level; i++)
        std::cout << "  ";

    // afficher le noeud
    std::cout << "Node("
              << "center=" << boundary_.getX() << "," << boundary_.getY()
              << " size=" << boundary_.getW() << "," << boundary_.getH()
              << " points=" << points_.size() << ")";

    if (!points_.empty()) {
        std::cout << " [ ";
        for (const auto& p : points_) {
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

