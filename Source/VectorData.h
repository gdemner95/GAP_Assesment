/*
  ==============================================================================

    VectorData.h
    Created: 19 Feb 2016 1:19:42pm
    Author:  George Demner

  ==============================================================================
*/

#ifndef VECTORDATA_H_INCLUDED
#define VECTORDATA_H_INCLUDED

#include "JuceHeader.h"
#include "FMODHeaders.h"
#include "RacingCommon.h"
using namespace FMOD::Studio;

class DataHeader
{
public:
    
    DataHeader(){
        //creates a zero'd object and y points upwards
        memset (&attr3d, 0, sizeof (attr3d));
        attr3d.up.y = 1.0f;
        attr3d.forward.z = 1.0f;
        
        // ignore errors if objects need 3D and aren't
        err3DExcept.add (FMOD_ERR_NEEDS3D);
        
        // we'll ignore errors if parameters are missing
        errParamExcept.add (FMOD_ERR_EVENT_NOTFOUND);
    }
    
    ~DataHeader(){}
    
    /** Add an EventInstance playing at this object position. */
    void addEvent (EventInstance* event)
    {
        // add to the array
        events.add (event);
        
        // set its vectors
//        ERRCHECK_EXCEPT (event->set3DAttributes (&attr3d), err3DExcept);
    }
    
    /** Remove an EventInstance manually. */
    void removeEvent (EventInstance* event)
    {
        events.removeFirstMatchingValue (event);
    }
    
    /** Call start() on all of the EventInstance objects. */
    void startEvents()
    {
        for (int i = events.size(); --i >= 0;)
            ERRCHECK (events.getUnchecked (i)->start());
    }
    void doStandardImpact (String const& objectNameKey, Collision const& collision){
        
    }
    
private:
    FMOD_3D_ATTRIBUTES attr3d;    ///< The object vectors for position, velocity, forward (orientation), and "up" (also orientation)
    Array<EventInstance*> events; ///< The array of event instances
    
    Array<FMOD_RESULT> err3DExcept;
    Array<FMOD_RESULT> errParamExcept;
};

#endif  // VECTORDATA_H_INCLUDED
