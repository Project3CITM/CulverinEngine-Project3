﻿using CulverinEditor;
using CulverinEditor.Debug;

class Disengage_Action : Action
{
    public float duration = 1.0f;

    public Disengage_Action()
    {
        action_type = ACTION_TYPE.DISENGAGE_ACTION;
    }

    public override bool ActionStart()
    {
        GetComponent<CompAnimation>().SetTransition("ToSheatheSword");
        //TODO_AI: Audio disengage?
        //GetComponent<CompAudio>().PlayEvent("Dracarys");
        GetComponent<CompAnimation>().SetClipDuration("Sheathe Sword", duration);


        //PLAY COMBAT MUSIC
        //Audio.ChangeState("AmbientMusic", "None");
        //GetLinkedObject("event_manager").GetComponent<PerceptionManager>().player_seen = false;

        return true;
    }

    public override ACTION_RESULT ActionUpdate()
    {
        if (GetComponent<CompAnimation>().IsAnimationStopped("Sheathe Sword"))
            return ACTION_RESULT.AR_SUCCESS;
        return ACTION_RESULT.AR_IN_PROGRESS;
    }

    public override bool ActionEnd()
    {
        interupt = false;
        return true;
    }
}

