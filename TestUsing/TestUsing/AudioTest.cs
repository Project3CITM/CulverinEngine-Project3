

using CulverinEditor;
using CulverinEditor.Debug;

public class AudioTest : CulverinBehaviour
{

    public CompAudio comp_audio;

    // Use this for initialization
    void Start() {
        comp_audio = GetComponent<CompAudio>();
        comp_audio.PlayEvent("TorchAmbient");
    }
    void Update()
    {
       if( Input.GetKeyDown(KeyCode.E))
        {
            comp_audio.SetAuxiliarySends("Corridor", 10);
        }
        if (Input.GetKeyDown(KeyCode.R))
        {
            comp_audio.SetAuxiliarySends("Corridor", 0);
        }
    }
}
