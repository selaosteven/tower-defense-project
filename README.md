# projet tower defense de la mort qui tue de deux boss (Victor et Steven)

# Thèmes 
Aucune idée en sah, tout fait l'affaire.

# Organisation

Comme je l'ai dis, a l'époque quand je codais en java, je foutais tout dans une classe du style dans Animaux.cpp j'avais la classe animal puis tigre , chat , chien , ...

=> Je pense le mieux ducoup, c'est de tout séparer :
    
    main.cpp => pour lancer le jeu
    
    Deux classes entités : 
        - Tower (Tour) => une classe générique car chaque tour ont les mêmes types de stats
        - Enemy (Ennemie) => une classe générique car same que la tower

    La question est :
        - Faire un .cpp pour chaque type d'ennemy / tour ?  

    Une classe qui sera littéralement nous le public :
        - nombre d'argent (pour les améliorations)
        - Une barre de vie 
        - les différentes intéractions claviers

    
# Implémentation 

Tower :

    Méthode :
        - constructeur => mettre les stats en fonction de la tour + increase l'id 
            pcq nb de tour max ?
        - tirer => qui sera différente en fonction des tours
        

    Chaque tour a une spécificité :
        Basic => rien a toucher
        Sniper => sa stat Range qui va increase 
        Canon =>  sa stat Attack speed qui va increase
        Freezing => qui va affecter le stat speed de l'ennemy touché => donc un getter sur les stats des ennemy ?
        Antiair => faire un test pour voir si l'ennemy le plus proche a sa stat fly = true, si c'est le cas, on le tire dessus sinon non

# Réunion du 11 février

Class :
    Map :
    Entity :
        - Enemy
        - Projectile
        - Tower
    Session : Map Physique , Map Entity, Tower, 
    Option :
    Map Physique (Tour) => QuadTree 
    Map Entity (Enemy)=> QuadTree
     