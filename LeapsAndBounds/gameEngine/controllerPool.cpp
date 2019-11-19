#include "controllerPool.hpp"

ControllerPool::ControllerPool()
{
	this->foundControllers();
}

ControllerPool::~ControllerPool() {}

int ControllerPool::addController( int id )
{
    if( SDL_IsGameController( id ) ) {
        SDL_GameController *pad = SDL_GameControllerOpen( id );

        if( pad )
        {
            SDL_Joystick *joy = SDL_GameControllerGetJoystick( pad );
            int instanceID = SDL_JoystickInstanceID( joy );

			controllers[instanceID] = pad;
            cout << "Branchement du controller " << instanceID << endl;
            return instanceID;

        }
    }
    return -1;
}

int ControllerPool::removeController( int id )
{
    cout << "Debranchement du controller " << id << endl;
    SDL_GameController *pad = controllers[id];
    SDL_GameControllerClose( pad );
	controllers[id] = NULL;
	controllers.erase(id);

	for(int i = 0; i<this->playerControllers.size();i++)
	{
        // On retire l'association player/controller si elle existe
        if(this->playerControllers[i]==id)
        {
            this->playerControllers[i] = -1;
            return i; // Le joueur "i" avait ce controller d'attribué
        }
	}
	return -1; // aucun joueur n'avait ce controller d'attribué
}

void ControllerPool::foundControllers()
{
	for (int i = 0; i < SDL_NumJoysticks(); ++i)
	{
		addController(i);
	}
}

int ControllerPool::associateControllerToPlayer(int playerId, int controllerId)
{
    if(controllers[controllerId]!=NULL)
    {
        if(playerId>=0 && playerId <playerControllers.size())
        {
            playerControllers[playerId] = controllerId;
            return playerId;
        }
        else
        {
            return -1;
        }
    }
    else
    {
        return -1;
    }
}

int ControllerPool::getPlayerFromController(int controllerId)
{
    if(controllerId == -1) return -1;

    for(int i = 0; i < playerControllers.size(); i++)
    {
        if(playerControllers[i] == controllerId)
        {
            return i;
        }
    }

    return -1;
}

int ControllerPool::associateControllerToPlayer(int controllerId, bool add)
{
    if(controllers[controllerId]!=NULL && getPlayerFromController(controllerId)==-1)
    {
        // Si un joueur existant n'avait pas ou plus de controller, on lui affecte celui-ci
        for(int i = 0; i < playerControllers.size(); i++)
        {
            if(playerControllers[i] == -1)
            {
                playerControllers[i] = controllerId;
                return i;
            }
        }
        // Si aucun joueur sans controller n'a été trouvé, on crée un nouveau joueur et on lui affecte le controller
        if(add)
        {
            playerControllers.push_back(controllerId);
            return playerControllers.size()-1; // renvoi le rang du nouveau joueur
        }
    }
    else
    {
        return -1;
    }
}

int ControllerPool::associateKeyboardToPlayer()
{
	if (getPlayerFromController(KEYBOARD_CONTROLLER_ID) == -1) {
		playerControllers.push_back(KEYBOARD_CONTROLLER_ID);
		return playerControllers.size() - 1; // renvoi le rang du nouveau joueur
	}
}

int ControllerPool::getController(int playerId)
{
	if (playerId < this->playerControllers.size()) {
		return this->playerControllers[playerId];
	}

	return -1;
}

int ControllerPool::countPlayersWithController()
{
    int n = 0;
    for(int i = 0; i < playerControllers.size(); i++)
    {
        if(playerControllers[i] != -1)
        {
            n++;
        }
    }

    return n;
}

void ControllerPool::clearControllerToPlayer()
{
    playerControllers.clear();
}
