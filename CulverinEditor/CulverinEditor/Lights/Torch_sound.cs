using CulverinEditor;
using CulverinEditor.Debug;

public class Torch_sound : CulverinBehaviour
{
    void Start()
    {
        GetComponent<CompAudio>().PlayEvent("PlayTorch");
    }
}