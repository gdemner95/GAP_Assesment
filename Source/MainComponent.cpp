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
    ERRCHECK(carEngine->setParameterValue("rpm", 800));
    
    ERRCHECK_EXCEPT (system->getEvent ("event:/car/skid", &desc), errExcept);
    ERRCHECK(desc->createInstance(&carSkid));
    ERRCHECK(carSkid->start());
    
    ERRCHECK_EXCEPT (system->getEvent ("event:/car/tyres", &desc), errExcept);
    ERRCHECK(desc->createInstance(&carTyres));
    ERRCHECK(carTyres->start());
    
    ERRCHECK_EXCEPT (system->getEvent ("event:/environment/atmos", &desc), errExcept);
    ERRCHECK(desc->createInstance(&atmos));
    ERRCHECK(atmos->start());
    
    ERRCHECK_EXCEPT (system->getEvent ("event:/environment/crowd", &desc), errExcept);
    ERRCHECK(desc->createInstance(&crowd));
    ERRCHECK(crowd->start());
    
    ERRCHECK_EXCEPT (system->getEvent ("event:/environment/electric-wires", &desc), errExcept);
    ERRCHECK(desc->createInstance(&wires));
    ERRCHECK(wires->start());
    
    
}

void MainContentComponent::handleDestroy (String const& name, int gameObjectInstanceID)
{
    
}

void MainContentComponent::handleVector (String const& name, int gameObjectInstanceID, String const& param, const Vector3* vector)
{
    //    Logger::outputDebugString (String ("Name: ") + name);
    FMOD_3D_ATTRIBUTES car3D;
    FMOD_3D_ATTRIBUTES wires3D;
    FMOD_3D_ATTRIBUTES missionControl3D;
    FMOD_3D_ATTRIBUTES camera3D;
    ERRCHECK (system->getListenerAttributes(FMOD_MAIN_LISTENER, &camera3D));
    if(name == "car"){
        ERRCHECK (system->getListenerAttributes(FMOD_MAIN_LISTENER, &car3D));

        if(param == "vel") car3D.velocity = *vector;
        if(param == "pos") car3D.position = *vector;
        if(param == "dir") car3D.forward = *vector;
        if(param == "up") car3D.up = *vector;
        
        ERRCHECK(carEngine->set3DAttributes(&car3D));
        ERRCHECK(carSkid->set3DAttributes(&car3D));
        ERRCHECK(carTyres->set3DAttributes(&car3D));
    }
    else if(name == "missioncontrol"){
        ERRCHECK(crowd->get3DAttributes(&missionControl3D));

        if(param == "vel") missionControl3D.velocity = *vector;
        if(param == "pos") missionControl3D.position = *vector;
        if(param == "dir") missionControl3D.forward = *vector;
        if(param == "up") missionControl3D.up = *vector;
    
    }
    else if(name == "wires"){
        ERRCHECK(wires->get3DAttributes(&wires3D));

        if(param == "vel") wires3D.velocity = *vector;
        if(param == "pos") wires3D.position = *vector;
        if(param == "dir") wires3D.forward = *vector;
        if(param == "up") wires3D.up = *vector;
        
    }
//    float wiresX = wires3D.position.x;
//    float wiresY = wires3D.position.y;
//    float wiresZ = wires3D.position.z;
//    float carX = car3D.position.x;
//    float carY = car3D.position.y;
//    float carZ = car3D.position.z;
//    
//    float pythag = powf(wiresX - carX,2) + powf(wiresZ - carZ,2);
//    printf("Pythagoras Theorem = %f\n", pythag);
//    
//
//    
    ERRCHECK (system->setListenerAttributes(FMOD_MAIN_LISTENER, &camera3D));
//    ERRCHECK (system->setListenerAttributes(FMOD_MAIN_LISTENER, &wires3D));
//    ERRCHECK (system->setListenerAttributes(FMOD_MAIN_LISTENER, &missionControl3D));


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
        
        
        //        Logger::outputDebugString (String ("Collision: ")
        //                                   + name
        //                                   + String (" [id = ")
        //                                   + String (gameObjectInstanceID)
        //                                   + String ("]")
        //                                   + String (" other: ")
        //                                   + collision.otherName
        //                                   + String (" velocity = ")
        //                                   + String (collision.velocity));
        
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
    //    Logger::outputDebugString (String ("Bool: ")
    //                               + name
    //                               + String (" [id = ")
    //                               + String (gameObjectInstanceID)
    //                               + String ("]")
    //                               + String (" Param: ")
    //                               + param
    //                               + String (" value = ")
    //                               + String (flag));
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
        
        // if we got a description then deal with it
        if (desc != nullptr)
        {
            printf("gear changed \n");
            DataHeader* vecData = objects.get (getGameInstanceString (name, gameObjectInstanceID));
            
            jassert (vecData != nullptr);
            
            // create an instance
            ERRCHECK (desc->createInstance (&carGear));
            //get parameter named blowOff
            ERRCHECK(carGear->getParameter("blowoff", &gearParameter));
            //asign blowoff the value of the
            float blowOff = value / 4.0;
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
        EventDescription* desc = nullptr;
        
        
        if(param == "skid"){
            ERRCHECK(carSkid->getParameter("amount", &skidParameter));
            float skidValue = value;
            
            ERRCHECK(skidParameter->setValue(skidValue));
            ERRCHECK(skidParameter->getValue(&skidValue));
            
        }
        if(param == "rpm"){
            
            carEngine->setParameterValue("rpm", value);
            
        }
        if(param == "speed"){
            ERRCHECK(carTyres->getParameter("speed", &speedParameter));
            float speedValue = value;
            
            ERRCHECK(speedParameter->setValue(speedValue));
            ERRCHECK(speedParameter->getValue(&speedValue));
        }
        if(param == "force"){
            ERRCHECK(carTyres->getParameter("force", &forceParameter));
            float forceValue = value;
            
            ERRCHECK(forceParameter->setValue(forceValue));
            ERRCHECK(forceParameter->getValue(&forceValue));
        }
        //seems to repeat the set/get param function.
    }
}
void MainContentComponent::handleString (String const& name, int gameObjectInstanceID, String const& param, String const& content)
{
    //    Logger::outputDebugString (String ("String: ")
    //                               + name
    //                               + String (" [id = ")
    //                               + String (gameObjectInstanceID)
    //                               + String ("]")
    //                               + String (" Param: ")
    //                               + param
    //                               + String (" value = ")
    //                               + String (content));
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



