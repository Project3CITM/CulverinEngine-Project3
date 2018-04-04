using CulverinEditor;
using CulverinEditor.Debug;


public class FireSound : CulverinBehaviour
{
    void Start()
    {
        //Play Fire Audio
        GetComponent<CompAudio>().PlayEvent("PlayTorch"); //Change This!!
    }
}