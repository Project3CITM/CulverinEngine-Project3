using CulverinEditor;
using CulverinEditor.Debug;

public class Code : CulverinBehaviour
{
    void OnTriggerEnter()
    {
        CompAudio audio = GetComponent<CompAudio>();
        audio.PlayEvent("SwordSlash");
    }
}