#include "MainComponent.h"
#include "Event Paths.h"
bool MainContentComponent::ERRCHECK_EXCEPT (FMOD_RESULT result, Array<FMOD_RESULT> const& errorExceptions)
{
    if (result == FMOD_OK)                  return true;
    if (errorExceptions.contains (result))  return false;
    
    ERRCHECK (result);
    return false;
}
MainContentComponent::MainContentComponent()
{
    collisionWait = 100;
    FMOD_Debug_Initialize (FMOD_DEBUG_LEVEL_NONE, FMOD_DEBUG_MODE_TTY, nullptr, nullptr);

    setSize (200, 200);
    
    // launches the other, game, app
    launchGame();
}

MainContentComponent::~MainContentComponent()
{
}


void MainContentComponent::initFMODStudio()
{
    //load resource path
    File resourcesPath = getResourcesPath();
    
    //bank file path
    File bankPath = resourcesPath.getChildFile("Master Bank.bank");
    File stringsPath = resourcesPath.getChildFile("Master Bank.strings.bank");
    
    ERRCHECK(Studio::System::create(&system));
    ERRCHECK (system->initialize (64,
                                  FMOD_STUDIO_INIT_LIVEUPDATE | FMOD_STUDIO_INIT_ALLOW_MISSING_PLUGINS,
                                  FMOD_INIT_PROFILE_ENABLE, 0)); // extraDriverData = 0
    
    // Load the bank into the system
    Studio::Bank* bank;
    ERRCHECK (system->loadBankFile (bankPath.getFullPathName().toUTF8(),
                                    FMOD_STUDIO_LOAD_BANK_NORMAL,
                                    &bank));
    
    // Load the strings bank into the system - to translate from GUIDs
    Studio::Bank* stringsBank;
    ERRCHECK (system->loadBankFile (stringsPath.getFullPathName().toUTF8(),
                                    FMOD_STUDIO_LOAD_BANK_NORMAL,
                                    &stringsBank));
    
    
    
}

void MainContentComponent::shutdownFMODStudio()
{
    // release the system
    
    if (system != nullptr)
        if (system->isValid())
            ERRCHECK (system->release());
}

void MainContentComponent::resized()
{
}

void MainContentComponent::handleConnect()
{
    initFMODStudio();
}

void MainContentComponent::handleDisconnect()
{
    shutdownFMODStudio();
    JUCEApplication::getInstance()->systemRequestedQuit();
}

void MainContentComponent::tick()
{
    // we must call call update() regularly
    
    if (collisionWait > 0)
        --collisionWait; // decrement the counter
    
    if (system != nullptr)
        if (system->isValid())
            ERRCHECK (system->update());
}

void MainContentComponent::handleCreate (String const& name, int gameObjectInstanceID)
{
    Logger::outputDebugString(String("Create: ")
                              + name
                              + String ("[id= ")
                              +String(gameObjectInstanceID)
                              +String("]"));
    
    objects.add (getGameInstanceString (name, gameObjectInstanceID), new DataHeader());
    EventDescription* desc = nullptr;
    ERRCHECK(system->getEvent(Strings::carEnginePath, &desc));
    ERRCHECK(desc->createInstance(&carEngine));
    ERRCHECK(carEngine->start());
    
//    ERRCHECK(system->getEvent(Strings::carGearPath, &desc));
//    ERRCHECK(desc->createInstance(&carGear));
//    carGear->start();
    
    
}

void MainContentComponent::handleDestroy (String const& name, int gameObjectInstanceID)
{
    
}

void MainContentComponent::handleVector (String const& name, int gameObjectInstanceID, String const& param, const Vector3* vector)
{
    
}

void MainContentComponent::handleHit (String const& name, int gameObjectInstanceID, Collision const& collision)
{
    if(collisionWait > 0)
        return;
    else{
        EventDescription* desc = nullptr;
        
        if(name == "car"){
            ERRCHECK_EXCEPT(system->getEvent(Strings::carCrashPath, &desc), errExcept);
        }
        
        
        Logger::outputDebugString (String ("Collision: ")
                                   + name
                                   + String (" [id = ")
                                   + String (gameObjectInstanceID)
                                   + String ("]")
                                   + String (" other: ")
                                   + collision.otherName
                                   + String (" velocity = ")
                                   + String (collision.velocity));
        
        // if we got a description then deal with it
        if (desc != nullptr)
        {
            // get our info about the game object
            DataHeader* vecData = objects.get (getGameInstanceString (name, gameObjectInstanceID));
            
            // shouldn't happen as we should have added the object in handleCreate()
            // if we got as far as having a description
            jassert (vecData != nullptr);
            
            // create an instance
            EventInstance* instance = nullptr;
            ERRCHECK (desc->createInstance (&instance));
            
            // some objects have a velocity parameter
            if (name == "Crate" || name == "Ship")
                ERRCHECK_EXCEPT (instance->setParameterValue ("velocity", collision.velocity), errExcept);
            
            // start the event
            ERRCHECK (instance->start());
            // add it to our game object store, we must start it before adding it otherwise
            // our VectorData class might remove it before it gets a chance to play as it
            // will be initially marked as "stopped"
            vecData->addEvent (instance);
        }
    }
    
}

void MainContentComponent::handleBool (String const& name, int gameObjectInstanceID, String const& param, bool flag)
{
        Logger::outputDebugString (String ("Bool: ")
                                   + name
                                   + String (" [id = ")
                                   + String (gameObjectInstanceID)
                                   + String ("]")
                                   + String (" Param: ")
                                   + param
                                   + String (" value = ")
                                   + String (flag));
}

void MainContentComponent::handleInt (String const& name, int gameObjectInstanceID, String const& param, int value)
{
    if(collisionWait > 0)
        return;
    else{
        EventDescription* desc = nullptr;
        
        if(param == "gear"){
            ERRCHECK_EXCEPT(system->getEvent(Strings::carGearPath, &desc), errExcept);
        }
        
        Logger::outputDebugString (String ("Int: ")
                                   + name
                                   + String (" [id = ")
                                   + String (gameObjectInstanceID)
                                   + String ("]")
                                   + String (" param: ")
                                   + param
                                   + String (" value = ")
                                   + String (value));
        
        // if we got a description then deal with it
        if (desc != nullptr)
        {
            printf("gear changed \n");
            // get our info about the game object
            DataHeader* vecData = objects.get (getGameInstanceString (name, gameObjectInstanceID));
            
            // shouldn't happen as we should have added the object in handleCreate()
            // if we got as far as having a description
            jassert (vecData != nullptr);
            
            // create an instance
            ERRCHECK (desc->createInstance (&carGear));
            ERRCHECK(carGear->getParameter("blowOff", &gearParameter));
            float blowOff = value;
            ERRCHECK(gearParameter->setValue(blowOff));
            ERRCHECK(gearParameter->getValue(&blowOff));
            
            // start the event
            ERRCHECK (carGear->start());
            // add it to our game object store, we must start it before adding it otherwise
            // our VectorData class might remove it before it gets a chance to play as it
            // will be initially marked as "stopped"
            vecData->addEvent (carGear);
        }
    }
}

void MainContentComponent::handleReal (String const& name, int gameObjectInstanceID, String const& param, double value)
{
    
    if(collisionWait > 0)
        return;
    else{
        
        if(param == "skid"){
            
            Logger::outputDebugString (String ("Real: ")
                               + name
                               + String (" [id = ")
                               + String (gameObjectInstanceID)
                               + String ("]")
                               + String (" Param: ")
                               + param
                               + String (" value = ")
                               + String (value));
        }
        if(param == "rpm"){
            carEngine->setParameterValue("rpm", value);
            
        }
//
    }
}
void MainContentComponent::handleString (String const& name, int gameObjectInstanceID, String const& param, String const& content)
{
        Logger::outputDebugString (String ("String: ")
                                   + name
                                   + String (" [id = ")
                                   + String (gameObjectInstanceID)
                                   + String ("]")
                                   + String (" Param: ")
                                   + param
                                   + String (" value = ")
                                   + String (content));
}

void MainContentComponent::handleOther (String const& name, String const& t, String const& value)
{
//        Logger::outputDebugString (String ("Other: ")
//                                   + name
//                                   + String (" [id = ")
//                                   + String (t)
//                                   + String ("]")
//                                   + String (" value = ")
//                                   + String (value));
}



