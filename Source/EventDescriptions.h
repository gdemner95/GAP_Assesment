/*
  ==============================================================================

    EventDescriptions.h
    Created: 19 Feb 2016 1:30:21pm
    Author:  George Demner

  ==============================================================================
*/

#ifndef EVENTDESCRIPTIONS_H_INCLUDED
#define EVENTDESCRIPTIONS_H_INCLUDED

#include "FMODHeaders.h"

using namespace FMOD::Studio;

#include "RacingCommon.h"

class MainContentComponent;

class Descriptions : public ReferenceCountedObject
{
public:
    typedef HashMap<String, EventDescription*> ObjectDescriptions;
    typedef ReferenceCountedObjectPtr<Descriptions> Ptr;
    
    Descriptions (MainContentComponent* mainComp);
    ~Descriptions();
    
    EventDescription* addDescription (String const& eventNameKey, String const& eventPath, StringArray const& expectedParameters = StringArray());
    EventDescription* getDescription (String const& eventNameKey);
    
private:
    MainContentComponent* mainComp;
    Studio::System* system;
    ObjectDescriptions descriptions;
};

#endif  // EVENTDESCRIPTIONS_H_INCLUDED
