#ifndef __CONTROLLER_POOL_HPP__
#define __CONTROLLER_POOL_HPP__

#define KEYBOARD_CONTROLLER_ID -2

#include <iostream>
#include <map>
#include <vector>
#include <SDL.h>

using namespace std;

class ControllerPool
{
    public:

    map<int, SDL_GameController *> controllers; ///< Liste des controllers branchés
    vector<int> playerControllers;  ///< Liste des id controller associés aux joueurs

    ControllerPool();
    ~ControllerPool();

    int addController( int id );
    /**
        retire un controller qui a été débranché
        @param id identifiant du controller à retirer
        @return id du joueur auquel le controller était associé ou -1 si pas de joueur associé
    */
    int removeController( int id );
	
	/**
		Référence les controllers branchés
	*/
    void foundControllers();

    /**
        Associe un controller à un joueur
        @param playerId rang du joueur
        @param controllerId Id du controller à lui associer
        @return Id du player ou -1 si l'association n'a pas pu se faire
    */
    int associateControllerToPlayer(int playerId, int controllerId);
    /**
        Associe un controller au prochain joueur qui n'en a pas encore
        @param controllerId Id du controller à lui associer
        @return Id du player ou -1 si l'association n'a pas pu se faire
    */
    int associateControllerToPlayer(int controllerId, bool add);

	int associateKeyboardToPlayer();

	int getController(int playerId);
	/**
		Récupère l'indice du joueur à partir de l'id du contrôller
	*/
    int getPlayerFromController(int controllerId);

	/**
		Retourne le nombre de joueur auxquels on a affecté un controller
	*/
    int countPlayersWithController();
	
	/**
		Supprime l'affectation aux joueurs pour tous les controllers
	*/
    void clearControllerToPlayer();

};

#endif
