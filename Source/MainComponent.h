#ifndef MAINCOMPONENT_H_INCLUDED
#define MAINCOMPONENT_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include "../../../common/FMODHeaders.h"
#include "Event Paths.h"
#include "DataHeader.h"
#include "CarObject.h"
using namespace FMOD::Studio;

class MainContentComponent  : public Component,
                              public GameEngineServer
{
public:
    MainContentComponent();
    ~MainContentComponent();
	
    void initFMODStudio();
    void shutdownFMODStudio();
	
    void resized() override;
    
    void handleConnect() override;
    void handleDisconnect() override;
    void tick() override;
    
    static bool ERRCHECK_EXCEPT (FMOD_RESULT result, Array<FMOD_RESULT> const& errorExceptions);

    void handleCreate (String const& name, int gameObjectInstanceID) override;
    void handleDestroy (String const& name, int gameObjectInstanceID) override;
    void handleVector (String const& name, int gameObjectInstanceID, String const& param, const Vector3* vector) override;
    void handleHit (String const& name, int gameObjectInstanceID, Collision const& collision) override;
    void handleBool (String const& name, int gameObjectInstanceID, String const& param, bool flag) override;
    void handleInt (String const& name, int gameObjectInstanceID, String const& param, int value) override;
    void handleReal (String const& name, int gameObjectInstanceID, String const& param, double value) override;
    void handleString (String const& name, int gameObjectInstanceID, String const& param, String const& content) override;
    void handleOther (String const& name, String const& t, String const& value) override;
    
    int currentGear = 0;
    int previousGear = 0;
    
private:
    typedef PointerDictionary<DataHeader> dataHeaderDictionary;
    dataHeaderDictionary objects;
    Studio::System* system = nullptr;
    EventInstance* carEngine = nullptr;
    EventInstance* carGear = nullptr;
    Array<FMOD_RESULT> errExcept;
    Studio::ParameterInstance* gearParameter;
    int collisionWait;

};

#endif  // MAINCOMPONENT_H_INCLUDED
