using CulverinEditor;
using CulverinEditor.Debug;


public class MusicController : CulverinBehaviour
{

    public int current_level = 0;
    void Start()
    {
        switch(current_level)
        {
            case 1:
                GetComponent<CompAudio>().PlayEvent("PlayAmbient");
                break;
            case 2:
                GetComponent<CompAudio>().PlayEvent("PlayChants");
                break;
        }
    }
}

